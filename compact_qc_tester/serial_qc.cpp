#include "serial_qc.h"
#include "serial_qc_protocol.h"
#include "config.h"
#include "qc_hardware.h"
#include "qc_session.h"
#include "ota_manager.h"
#include <HardwareSerial.h>

namespace {
HardwareSerial uart(1);
bool active = false, waitingAck = false, incrementAfterTx = false, waitingFirst = true;
uint32_t runId = 0, baudRate = 9600, timeoutMs = 2000, lastProgress = 0;
uint16_t target = 100, completed = 0;
uint32_t rxBytes = 0, txBytes = 0, errors = 0;
uint8_t rx[SerialQc::MAX_FRAME], tx[SerialQc::MAX_FRAME];
size_t rxSize = 0, txSize = 0, txAt = 0;
String mode = "RS232";
String status = "BELUM DIUJI", serialDetail = "USB-RS232 external fixture required";
void record(const char *s, const String &d) {
  status = s; serialDetail = d; qcRecord(mode, status, serialDetail);
}
void end(const char *s, const String &d) {
  active = false; uart.end(); rxSize = txSize = txAt = 0;
  record(s, d);
}
void fail(const char *d) { ++errors; end("FAIL", d); }
bool number(WebServer &web, const char *key, uint32_t low, uint32_t high, uint32_t &out) {
  String s = web.arg(key);
  if (!s.length() || s.length() > 9) return false;
  for (char c : s) if (c < '0' || c > '9') return false;
  out = strtoul(s.c_str(), nullptr, 10);
  return out >= low && out <= high;
}
void packet() {
  using namespace SerialQc;
  if (!valid(rx, rxSize)) { fail("Frame/CRC invalid; test failed, check fixture and baud"); return; }
  if (get32(rx + 4) != runId || get16(rx + 8) != completed) {
    fail("Wrong run ID or packet sequence; stale/duplicate data rejected"); return;
  }
  if (!waitingAck) {
    if (rx[3] != REQUEST || get16(rx + 10) != payloadSize(completed)) {
      fail("Unexpected request type or payload length"); return;
    }
    for (size_t i = 0; i < payloadSize(completed); ++i)
      if (rx[HEADER + i] != pattern(runId, completed, i)) {
        fail("Laptop-to-device payload mismatch"); return;
      }
    txSize = frame(tx, RESPONSE, runId, completed);
    waitingAck = true;
  } else {
    if (rx[3] != ACK || get16(rx + 10) != 0) {
      fail("Expected laptop ACK of transformed response"); return;
    }
    txSize = frame(tx, ACK_REPLY, runId, completed);
    incrementAfterTx = true;
  }
  rxSize = txAt = 0;
  lastProgress = millis();
}
}
bool serialQcActive() { return active; }
void resetSerialQc() {
  cancelSerialQc("Session reset"); runId = completed = rxBytes = txBytes = errors = 0;
  status = "BELUM DIUJI"; serialDetail = "USB-RS232 external fixture required";
}
void cancelSerialQc(const char *reason) {
  if (active) end("INCOMPLETE", reason);
}
void serialQcState(JsonObject out) {
  char id[9]; snprintf(id, sizeof(id), "%08lX", (unsigned long)runId);
  out["mode"] = mode; out["active"] = active; out["status"] = status; out["detail"] = serialDetail;
  out["runId"] = id; out["baud"] = baudRate; out["format"] = "8N1";
  out["packets"] = target; out["completed"] = completed; out["errors"] = errors;
  out["rxBytes"] = rxBytes; out["txBytes"] = txBytes; out["timeoutMs"] = timeoutMs;
  out["rxPin"] = SERIAL_RX; out["txPin"] = SERIAL_TX;
}
void beginSerialQc(WebServer &web, bool (*authorize)(), void (*stop)()) {
  auto startHandler = [&web, authorize, stop] {
    if (!authorize()) return;
    JsonDocument d;
    uint32_t b, n, t;
    String selected = web.hasArg("mode") ? web.arg("mode") : "RS232";
    if (active || webOtaBusy() || pdNegotiating()) d["error"] = "Wait for active test/OTA/PD";
    else if ((selected != "RS232" && selected != "RS485") || !number(web, "baud", 1200, 115200, b) ||
             !number(web, "packets", 10, 1000, n) ||
             !number(web, "timeoutMs", 500, 10000, t)) d["error"] = "Invalid RS232 settings";
    else {
      stop();
      mode = selected;qcRetest(mode);
      completed = rxBytes = txBytes = errors = 0; runId = 0;
      record("TESTING", "Selecting " + mode + " route");
      uint8_t gpio = 0, dir = 0;
      if (!mcpBit(MCP2_ADDR, 2, mode == "RS232") || !qcReadReg(MCP2_ADDR, 0x12, &gpio, 1) ||
          !qcReadReg(MCP2_ADDR, 0x00, &dir, 1) || bool(gpio & 4) != (mode == "RS232") || (dir & 4))
        d["error"] = "MCP2A2 route write/readback failed";
      else {
        baudRate = b; target = n; timeoutMs = t; runId = esp_random();
        completed = rxBytes = txBytes = errors = 0;
        rxSize = txSize = txAt = 0; waitingAck = incrementAfterTx = false; waitingFirst = true;
        uart.setRxBufferSize(512); uart.setTxBufferSize(256);
        uart.begin(baudRate, SERIAL_8N1, SERIAL_RX, SERIAL_TX);
        if (!uart) { uart.end(); record("FAIL", "UART1 initialization failed"); d["error"] = serialDetail; }
        else {
          active = true; lastProgress = millis();
          record("MENUNGGU LAPTOP", mode + " selected; start laptop test within 30 seconds");
          serialQcState(d.to<JsonObject>()); d["ok"] = true;
        }
      }
    }
    if (!active && !d["error"].isNull() && status == "TESTING") record("FAIL", d["error"].as<String>());
    String body; serializeJson(d, body);
    web.send(d["error"].isNull() ? 200 : 409, "application/json", body);
  };
  web.on("/api/serial/start", HTTP_POST, startHandler);
  web.on("/api/rs232/start", HTTP_POST, startHandler);
  auto stopHandler = [&web, authorize] {
    if (!authorize()) return;
    cancelSerialQc("RS232 test cancelled by user");
    web.send(200, "application/json", "{\"ok\":true}");
  };
  web.on("/api/serial/stop", HTTP_POST, stopHandler);
  web.on("/api/rs232/stop", HTTP_POST, stopHandler);
}
void updateSerialQc() {
  if (!active) return;
  if (webOtaBusy()) { cancelSerialQc("Cancelled for OTA"); return; }
  const uint32_t now = millis();
  // Deadline is per valid protocol phase, not extended by noise or partial bytes.
  if (completed < target && now - lastProgress >
      (waitingFirst ? 30000u : timeoutMs)) {
    fail("Serial QC timeout; no verified transaction. Check adapter/cable/baud/device before diagnosing hardware");
    return;
  }
  if (txAt < txSize) {
    int free = uart.availableForWrite();
    size_t n = min(size_t(free > 0 ? free : 0), min(size_t(64), txSize - txAt));
    if (n) { size_t sent = uart.write(tx + txAt, n); txAt += sent; txBytes += sent; }
    if (txAt == txSize) {
      txAt = txSize = 0;
      if (incrementAfterTx) {
        incrementAfterTx = waitingAck = false; ++completed;
        if (completed >= target) {
          // Driver queue is allowed to drain before end(); do not discard final ACK reply.
          record("FINALIZING", "All responses acknowledged; draining final UART reply");
        } else record("TESTING", String(completed) + "/" + target + " matched round trips");
      }
    }
    return;
  }
  if (completed >= target) {
    // No blocking flush: 256-byte TX ring + FIFO drains within 3s even at 1200 baud.
    if (now - lastProgress >= 3000) end("PASS", String(completed) + "/" + target +
      " CRC/pattern/sequence verified; laptop acknowledged transformed responses (8N1, " + baudRate + " baud)");
    return;
  }
  for (int budget = 0; budget < 256 && uart.available(); ++budget) {
    int c = uart.read(); if (c < 0) break;
    if (waitingFirst) { waitingFirst = false; lastProgress = now; }
    ++rxBytes;
    if (rxSize >= sizeof(rx)) { fail("RX frame overflow"); return; }
    rx[rxSize++] = c;
    if ((rxSize == 1 && rx[0] != 'Q') || (rxSize == 2 && rx[1] != 'C')) {
      fail("Unexpected bytes/framing; fixture is not sending QC protocol"); return;
    }
    if (rxSize >= SerialQc::HEADER) {
      size_t len = SerialQc::get16(rx + 10);
      if (len > SerialQc::MAX_PAYLOAD) { fail("Frame length outside bound"); return; }
      if (rxSize == SerialQc::HEADER + len + 2) { packet(); return; }
    }
  }
}
