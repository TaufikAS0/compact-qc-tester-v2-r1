#include "qc_hardware.h"
#include "qc_session.h"
#include "tpl_watchdog.h"
#include "pd_math.h"
namespace {
uint32_t targetMV = 0, requestedAt = 0, lastPoll = 0, source[7] = {}, rdo = 0;
uint8_t sourceCount = 0, fsm = 0, sinkCount = 0;
uint32_t sink[3] = {};
bool resetQueued = false;
bool pending = false, registersValid = false, attached = false;
String status = "BELUM REQUEST", pdDetail = "Runtime PDO; tidak menulis NVM IC";
uint32_t decode(const uint8_t *b) {
  return (uint32_t)b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
}
bool softReset() {
  uint8_t header[] = {0x0d, 0}, command = 0x26;
  return qcWriteReg(0x28, 0x51, header, 2) && qcWriteReg(0x28, 0x1a, &command, 1);
}
} // namespace
bool requestPd(uint16_t mv, uint16_t ma, String &out) {
  if (mv < 5000 || mv > 20000 || mv % 50 || ma < 10 || ma > 5000 || ma % 10) {
    out = "Fixed PDO 5..20V/50mV; 10..5000mA/10mA";
    return false;
  }
  if (pending) {
    out = "Negotiation still active; wait for result";
    return false;
  }
  uint8_t id, port, count, old[12];
  if (!qcReadReg(0x28, 0x2f, &id, 1) || (id != 0x25 && id != 0x21) ||
      !qcReadReg(0x28, 0x0e, &port, 1) || !(port & 1)) {
    out = "STUSB4500 ID/attachment unavailable";
    return false;
  }
  if (!qcReadReg(0x28, 0x85, old, 12) || !qcReadReg(0x28, 0x70, &count, 1)) {
    out = "PDO snapshot failed";
    return false;
  }
  // Disable optional PDOs while editing. Preserve reserved count bits.
  uint8_t one = (count & ~3u) | 1, desired = activeSinkCount(count, mv);
  uint32_t pdo[3] = {(decode(old) & 0x3ff00000u) | (100u << 10) | (ma / 10u), fixedSinkPDO(mv, ma),
                     fixedSinkPDO(mv, ma)};
  bool ok = qcWriteReg(0x28, 0x70, &one, 1);
  for (int i = 0; i < 3 && ok; ++i) {
    uint8_t b[4], verify[4];
    for (int j = 0; j < 4; ++j)
      b[j] = pdo[i] >> (8 * j);
    ok = qcWriteReg(0x28, 0x85 + 4 * i, b, 4) && qcReadReg(0x28, 0x85 + 4 * i, verify, 4) &&
         !memcmp(b, verify, 4);
  }
  uint8_t vc;
  ok = ok && qcWriteReg(0x28, 0x70, &desired, 1) && qcReadReg(0x28, 0x70, &vc, 1) &&
       (vc & 3) == (desired & 3);
  if (!ok) {
    bool restored = true;
    for (int i = 0; i < 3; ++i)
      restored = qcWriteReg(0x28, 0x85 + 4 * i, old + 4 * i, 4) && restored;
    restored = qcWriteReg(0x28, 0x70, &count, 1) && restored;
    uint8_t verify[12], n;
    restored = restored && qcReadReg(0x28, 0x85, verify, 12) && !memcmp(old, verify, 12) &&
               qcReadReg(0x28, 0x70, &n, 1) && n == count;
    out = restored ? "PDO write/readback failed; original registers restored"
                   : "PDO write/readback AND restoration failed";
    return false;
  }
  // Clear old alerts before reset; capture only the new source capabilities.
  uint8_t clear[10];
  qcReadReg(0x28, 0x0d, clear, 10);
  sourceCount = 0;
  rdo = 0;
  targetMV = mv;
  resetQueued = true; // Transmit from the main loop, after HTTP response has completed.
  pending = true;
  requestedAt = millis();
  lastPoll = 0;
  status = "NEGOTIATING";
  pdDetail = out = "Semua sink PDO ditulis/readback; PDO3 lama tidak dapat memilih 20V. Menunggu "
                   "kontrak SOURCE; ukur VBUS fisik.";
  return true;
}
bool pdNegotiating() { return pending; }
void updatePd() {
  if (resetQueued) {
    resetQueued = false;
    if (!softReset()) {
      pending = false;
      status = "FAIL";
      pdDetail = "Soft-reset command failed";
      qcRecord("STUSB4500", status, pdDetail);
      return;
    }
    requestedAt = millis();
  }
  uint32_t now = millis();
  if (now - lastPoll < (pending ? 5u : 1000u))
    return;
  lastPoll = now;
  uint8_t port, prt, header[2], data[28], b[12], n, r[4];
  registersValid = qcReadReg(0x28, 0x0e, &port, 1);
  attached = registersValid && (port & 1);
  if (!registersValid) {
    if (pending && now - requestedAt > 5000) {
      pending = false;
      status = "FAIL";
      pdDetail = "I2C lost during negotiation";
      qcRecord("STUSB4500", status, pdDetail);
    }
    return;
  }
  if (!attached) {
    sourceCount = 0;
    rdo = 0;
    if (!pending && targetMV) {
      status = "DETACHED";
      pdDetail = "USB-C detached; previous contract no longer active";
      qcRecord("STUSB4500", "UNVERIFIED", pdDetail);
    }
  }
  // PRT_STATUS read clears incoming-message event. RX type 1 with data = Source_Capabilities.
  if (qcReadReg(0x28, 0x16, &prt, 1) && (prt & 4) && qcReadReg(0x28, 0x31, header, 2)) {
    uint16_t h = header[0] | header[1] << 8;
    uint8_t objects = (h >> 12) & 7;
    if ((h & 31) == 1 && objects && !(h & 0x8000) && qcReadReg(0x28, 0x33, data, objects * 4)) {
      sourceCount = objects;
      for (int i = 0; i < objects; ++i)
        source[i] = decode(data + 4 * i);
    }
  }
  qcReadReg(0x28, 0x29, &fsm, 1);
  if (qcReadReg(0x28, 0x91, r, 4))
    rdo = decode(r);
  if (qcReadReg(0x28, 0x70, &n, 1))
    sinkCount = n & 3;
  if (qcReadReg(0x28, 0x85, b, 12))
    for (int i = 0; i < 3; ++i)
      sink[i] = decode(b + 4 * i);
  if (!pending && targetMV && attached &&
      (sinkCount != (targetMV == 5000 ? 1 : 2) || ((sink[1] >> 10) & 1023) * 50 != targetMV)) {
    status = "PROFILE CHANGED";
    pdDetail = "Sink profile changed/reset: NVM defaults may have reloaded; request again";
    qcRecord("STUSB4500", "UNVERIFIED", pdDetail);
  }
  uint8_t pos = (rdo >> 28) & 7;
  bool fixed = pos && pos <= sourceCount && (source[pos - 1] >> 30) == 0;
  uint32_t negotiated = sourceContractMV(rdo, source, sourceCount);
  if (pending && attached && fsm == 0x18 && fixed && negotiated == targetMV &&
      !(rdo & (1u << 26))) {
    pending = false;
    status = "CONTRACT MATCH";
    pdDetail = "SOURCE PDO/RDO/PE ready cocok target; belum mengukur tegangan fisik";
    qcRecord("STUSB4500", "CONTRACT MATCH", pdDetail);
  } else if (pending && now - requestedAt > 5000) {
    pending = false;
    status = fixed ? "MISMATCH" : "UNVERIFIED";
    pdDetail = !attached ? "USB-C terlepas"
               : fixed
                   ? "Kontrak SOURCE berbeda target; cek PDO/arus adaptor"
                   : "Source capabilities tidak tertangkap; kontrak target belum terverifikasi. "
                     "Coba request ulang.";
    qcRecord("STUSB4500", status, pdDetail);
  }
}
void pdState(JsonObject d) {
  d["valid"] = registersValid;
  d["attached"] = attached;
  d["pending"] = pending;
  d["status"] = status;
  d["detail"] = pdDetail;
  d["requestedMV"] = targetMV;
  d["fsm"] = fsm;
  d["rdo"] = rdo;
  d["sinkCount"] = sinkCount;
  uint8_t pos = (rdo >> 28) & 7;
  d["sourcePosition"] = pos;
  d["capabilityMismatch"] = (rdo & (1u << 26)) != 0;
  if (attached && fsm == 0x18 && pos && pos <= sourceCount && (source[pos - 1] >> 30) == 0)
    d["contractMV"] = sourceContractMV(rdo, source, sourceCount);
  auto a = d["sinkPDOs"].to<JsonArray>();
  for (int i = 0; i < 3; ++i) {
    auto p = a.add<JsonObject>();
    p["active"] = i < sinkCount;
    p["mv"] = ((sink[i] >> 10) & 1023) * 50;
    p["ma"] = (sink[i] & 1023) * 10;
  }
  auto src = d["sourcePDOs"].to<JsonArray>();
  for (int i = 0; i < sourceCount; ++i) {
    auto p = src.add<JsonObject>();
    p["position"] = i + 1;
    p["raw"] = source[i];
    p["type"] = source[i] >> 30;
    if ((source[i] >> 30) == 0) {
      p["mv"] = ((source[i] >> 10) & 1023) * 50;
      p["ma"] = (source[i] & 1023) * 10;
    }
  }
}
