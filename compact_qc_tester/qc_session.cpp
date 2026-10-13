#include "qc_session.h"
#include "config.h"
#include "ota_manager.h"
#include "qc_hardware.h"
#include "current_calibration.h"
#include "current_qc.h"
#include "qc_math.h"
#include "qc_report_math.h"
#include "qc_export.h"
#include "serial_qc.h"
#include "radio_qc.h"
#include <Preferences.h>
namespace {
WebServer *web;
bool (*authorized)();
void (*stopSafe)();
QcItem items[80];
size_t count = 0, display = 0;
uint32_t displayAt = 0;
String operatorName, unit;
DeviceVariant variant = DeviceVariant::Unselected;
String sessionId;
InputCycles cycles[7];
bool live[13] = {}, attempted[13] = {}, onConfirmed[13] = {}, offConfirmed[13] = {};
bool finalized[13] = {};
bool i2cScanned = false;
unsigned i2cDevices = 0;
Preferences prefs;
uint32_t baud = 9600;
uint8_t slave = 1, functionCode = 3;
uint16_t regAddress = 0, regCount = 1;
String parity = "N", route = "RS485";
void send(JsonDocument &d, int code = 200) {
  String b;
  serializeJson(d, b);
  web->sendHeader("Cache-Control", "no-store");
  web->send(code, "application/json", b);
}
void error(const char *s, int code = 409) {
  JsonDocument d;
  d["error"] = s;
  send(d, code);
}
bool guard() {
  if (!authorized())
    return false;
  if (webOtaBusy()) {
    error("OTA in progress");
    return false;
  }
  return true;
}
bool number(const char *arg, long low, long high, long &v) {
  String s = web->arg(arg);
  if (s.isEmpty() || s.length() > 10)
    return false;
  for (auto c : s)
    if (!isDigit(c))
      return false;
  char *end;
  long long n = strtoll(s.c_str(), &end, 10);
  if (*end || n < low || n > high)
    return false;
  v = n;
  return true;
}
bool decimal(const char *arg, float low, float high, float &v) {
  String s = web->arg(arg);
  char *end;
  v = strtof(s.c_str(), &end);
  return end != s.c_str() && !*end && isfinite(v) && v >= low && v <= high;
}
String outputName(int n) { return n < 11 ? "LED:" + String(n) : "Relay:" + String(n - 11); }
void reset() {
  resetSerialQc(); resetRadioQc(); resetCurrentQc();
  char id[9];
  snprintf(id, sizeof(id), "%08lX", static_cast<unsigned long>(esp_random()));
  sessionId = id;
  count = display = 0;
  i2cScanned = false; i2cDevices = 0;
  for (auto &c : cycles)
    c = {};
  for (int i = 0; i < 13; ++i)
    live[i] = attempted[i] = onConfirmed[i] = offConfirmed[i] = finalized[i] = false;
  for (const char *m : {"OLED", "I2C scan", "RTC", "EEPROM", "AHT10", "ADS / 4-20mA", "STUSB4500",
                        "Ethernet", "SD"})
    qcRecord(m,
             variant == DeviceVariant::M1 && String(m) == "STUSB4500" ? "NOT APPLICABLE"
                                                                      : "BELUM DIUJI",
             variant == DeviceVariant::M1 && String(m) == "STUSB4500"
                 ? "M1: PD Sync unpopulated; excluded from QC"
                 : "");
  for (size_t i = 0; i < 7; ++i)
    qcRecord("Input:" + String(inputName(i)), "MENUNGGU SIGNAL",
             "Dua siklus HIGH-LOW-HIGH, debounce 50ms; tanpa konfirmasi operator");
  for (int i = 0; i < 13; ++i)
    qcRecord(outputName(i), "BELUM DIUJI", "");
  qcRecord("RS485", "BELUM DIUJI", "Laptop USB-RS485; automatic direction confirmed by user");
  qcRecord("RS232", "BELUM DIUJI", "Laptop USB-RS232 QC protocol required");
  qcRecord("Radio communication", "BELUM DIUJI", "E70-915NW14S UART AT readback");
  qcRecord("Radio configuration", "BELUM DIUJI", "Compare supported parameters; LoRa SF/BW/CR unsupported");
}
} // namespace
bool qcProfileSelected() { return variant != DeviceVariant::Unselected; }
bool qcPdEnabled() { return pdApplicable(variant); }
const char *qcVariantName() {
  return variant == DeviceVariant::M0 ? "M0" : variant == DeviceVariant::M1 ? "M1" : "UNSELECTED";
}
void qcRecord(const String &name, const String &status, const String &detail) {
  if (name == "STUSB4500" && variant == DeviceVariant::M1 && status != "NOT APPLICABLE")
    return;
  size_t i = 0;
  for (; i < count; ++i)
    if (items[i].name == name)
      break;
  if (i == count) {
    if (count >= 80)
      return;
    ++count;
  }
  if (items[i].status != status || items[i].detail != detail) {
    display = i;
    displayAt = millis();
  }
  items[i] = {name, status, detail};
}
void qcOutputsStopped() {
  for (auto &b : live)
    b = false;
}
void qcNoteI2cScan(unsigned devices) {
  i2cScanned = true; i2cDevices = devices;
  qcRecord("I2C scan", devices ? "DETECTED" : "FAIL",
           devices ? "ACK list ready; awaiting user PASS/FAIL, not protocol proof"
                   : "No I2C addresses acknowledged");
}
void qcOutputAttempt(const String &type, int index, bool on) {
  int n = type == "led" ? index : 11 + index;
  if (n < 0 || n >= 13)
    return;
  live[n] = on;
  if (on && !finalized[n]) {
    attempted[n] = true;
    onConfirmed[n] = offConfirmed[n] = false;
    qcRecord(outputName(n), "MENUNGGU USER", "Command ON; konfirmasi bahwa output fisik menyala");
  }
}
const QcItem &qcDisplayItem() { return items[display]; }
const char *qcDisplayDetail() { return qcVariantName(); }
void updateQcSession() {
  uint32_t now = millis();
  if (now - displayAt > 2500 && count) {
    displayAt = now;
    display = (display + 1) % count;
  }
  if (!qcProfileSelected())
    return;
  for (size_t i = 0; i < 7; ++i) {
    int level = -1;
    readInput(i, level);
    unsigned before = cycles[i].cycles;
    cycles[i].update(level, now);
    if (before != cycles[i].cycles)
      qcRecord("Input:" + String(inputName(i)), cycles[i].cycles >= 2 ? "PASS" : "MENUNGGU SIGNAL",
               String(cycles[i].cycles) + "/2 complete HIGH-LOW-HIGH cycles, stable >=50ms");
  }
}
void qcState(JsonObject d) {
  d["unit"] = unitSerial();
  d["operator"] = operatorName;
  d["variant"] = qcVariantName();
  d["profileSelected"] = qcProfileSelected();
  d["sessionId"] = sessionId;
  auto scan = d["i2c"].to<JsonObject>();
  scan["scanned"] = i2cScanned; scan["devices"] = i2cDevices;
  auto a = d["tests"].to<JsonArray>();
  unsigned passed = 0, failed = 0, pending = 0, skipped = 0;
  String canonical = String("QC1|") + FW_VERSION + "|" + FW_BUILD_UTC + "|" + qcVariantName() +
                     "|" + unitSerial() + "|" + sessionId;
  auto hashField = [&](const String &value) {
    canonical += "|" + String(value.length()) + ":" + value;
  };
  for (size_t i = 0; i < count; ++i) {
    auto p = a.add<JsonObject>();
    p["name"] = items[i].name;
    p["status"] = items[i].status;
    p["detail"] = items[i].detail;
    bool applicable = testApplicable(variant, items[i].name.c_str());
    p["applicable"] = applicable;
    if (!applicable)
      ++skipped;
    else if (items[i].status == "FAIL")
      ++failed;
    else if (items[i].status == "PASS")
      ++passed;
    else
      ++pending;
    hashField(items[i].name);
    hashField(items[i].status);
    hashField(items[i].detail);
  }
  const char *overall = !qcProfileSelected() ? "INCOMPLETE"
                        : failed             ? "FAIL"
                        : !pending && passed ? "PASS"
                                             : "INCOMPLETE";
  d["overall"] = overall;
  d["applicableCount"] = passed + failed + pending;
  d["passCount"] = passed;
  d["failCount"] = failed;
  d["pendingCount"] = pending;
  d["notApplicableCount"] = qcProfileSelected() ? skipped : 0;
  if (qcProfileSelected()) {
    hashField(overall);
    char crc[9];
    snprintf(crc, sizeof(crc), "%08lX", static_cast<unsigned long>(qcCrc(canonical.c_str())));
    d["qcCode"] = String("QC1-") + qcVariantName() + "-" + unitSerial() + "-" + overall + "-" +
                  sessionId + "-" + crc;
  } else
    d["qcCode"] = "";
  d["qcCodeSchema"] = "QC1";
  auto in = d["inputs"].to<JsonArray>();
  for (int i = 0; i < 7; ++i) {
    auto p = in.add<JsonObject>();
    p["cycles"] = cycles[i].cycles;
    p["stable"] = cycles[i].stable;
  }
  auto outputs = d["outputs"].to<JsonArray>();
  for (int i = 0; i < 13; ++i) {
    auto p = outputs.add<JsonObject>();
    p["commandedOn"] = live[i];
    p["attempted"] = attempted[i];
    p["onConfirmed"] = onConfirmed[i];
    p["offConfirmed"] = offConfirmed[i];
    p["finalized"] = finalized[i];
  }
  auto m = d["modbus"].to<JsonObject>();
  m["baud"] = baud;
  m["parity"] = parity;
  m["slave"] = slave;
  m["function"] = functionCode;
  m["register"] = regAddress;
  m["count"] = regCount;
  m["route"] = route;
  m["testStatus"] = "MODBUS DITUNDA; raw serial QC available";
  serialQcState(d["serial"].to<JsonObject>());
  radioQcState(d["radio"].to<JsonObject>());
}
void beginQcSession(WebServer &server, bool (*authorize)(), void (*stop)()) {
  web = &server;
  authorized = authorize;
  stopSafe = stop;
  reset();
  beginQcExport(server);
  prefs.begin("qc-settings", false);
  baud = prefs.getUInt("baud", 9600);
  parity = prefs.getString("parity", "N");
  slave = prefs.getUChar("slave", 1);
  functionCode = prefs.getUChar("fn", 3);
  regAddress = prefs.getUShort("reg", 0);
  regCount = prefs.getUShort("count", 1);
  server.on("/api/qc/new", HTTP_POST, [] {
    if (!guard())
      return;
    String name = web->arg("operator");
    name.trim();
    if (name.length() > 64) {
      error("Operator max64", 400);
      return;
    }
    DeviceVariant selected = parseVariant(web->arg("variant").c_str());
    if (selected == DeviceVariant::Unselected) {
      error("Pilih variant M0 atau M1", 400);
      return;
    }
    if (pdNegotiating()) {
      error("Wait for PD negotiation before changing/resetting device");
      return;
    }
    stopSafe();
    variant = selected;
    reset();
    operatorName = name;
    resetCurrentPoints();
    JsonDocument d;
    d["ok"] = true;
    send(d);
  });
  server.on("/api/qc/confirm", HTTP_POST, [] {
    if (!guard())
      return;
    String type = web->arg("kind"), verdict = web->arg("verdict");
    if (verdict != "PASS" && verdict != "FAIL") {
      error("PASS/FAIL required", 400);
      return;
    }
    if (type == "i2c") {
      if (!i2cScanned || (verdict == "PASS" && !i2cDevices)) {
        error("Run I2C scan first; empty scan cannot PASS"); return;
      }
      qcRecord("I2C scan", verdict,
               "User confirms scanned ACK list " + String(verdict == "PASS" ? "sesuai" : "tidak sesuai") +
               "; " + String(i2cDevices) + " addresses; no IC/protocol acceptance inferred");
    } else if (type == "oled") {
      JsonDocument h;
      hardwareState(h.to<JsonObject>());
      if (verdict == "PASS" && !h["oledReady"].as<bool>()) {
        error("OLED initialization unavailable");
        return;
      }
      qcRecord("OLED", verdict,
               verdict == "PASS" ? "User confirms OLED text visible"
                                 : "User reports blank/unreadable OLED");
    } else if (type == "led" || type == "relay") {
      long index;
      if (!number("index", 0, type == "led" ? 10 : 1, index)) {
        error("Invalid index", 400);
        return;
      }
      int n = type == "led" ? index : 11 + index;
      if (finalized[n]) {
        error("QC result already final; start a new session to repeat QC");
        return;
      }
      String expected = web->arg("state");
      if (expected != "on" && expected != "off") {
        error("Expected state required", 400);
        return;
      }
      if (!attempted[n] || (expected == "on") != live[n]) {
        error("Confirm only attempted current state");
        return;
      }
      finalized[n] = true;
      if (verdict == "FAIL") {
        stopSafe();
        qcRecord(outputName(n), "FAIL",
                 "User physical observation differs from commanded " + expected);
      } else {
        if (expected == "on")
          onConfirmed[n] = true;
        else
          offConfirmed[n] = true;
        qcRecord(outputName(n), "PASS", "User confirms output sesuai commanded " + expected);
      }
    } else {
      error("Only OLED/LED/output accepts visual confirmation", 400);
      return;
    }
    JsonDocument d;
    d["ok"] = true;
    send(d);
  });
  server.on("/api/identity/read", HTTP_POST, [] {
    if (!guard())
      return;
    String detail;
    bool ok = readIdentity(detail);
    qcRecord("EEPROM", ok ? "DATA VALID" : "FAIL", detail);
    JsonDocument d;
    d["ok"] = ok;
    d["detail"] = detail;
    hardwareState(d["hardware"].to<JsonObject>());
    send(d);
  });
  server.on("/api/identity", HTTP_POST, [] {
    if (!guard())
      return;
    stopSafe();
    String detail;
    bool ok = saveIdentity(web->arg("top"), web->arg("bottom"), detail);
    qcRecord("EEPROM", ok ? "PASS" : "FAIL", detail);
    JsonDocument d;
    d["ok"] = ok;
    d["detail"] = detail;
    send(d, ok ? 200 : 409);
  });
  server.on("/api/current/capture", HTTP_POST, [] {
    if (!guard())
      return;
    long i;
    float ma;
    if (!number("index", 0, 3, i) || !decimal("ma", 4, 20, ma)) {
      error("Invalid calibration point", 400);
      return;
    }
    String detail;
    if (pdNegotiating()) {
      error("Wait for PD negotiation before ADC capture");
      return;
    }
    bool ok = captureCurrentPoint(i, ma, detail);
    JsonDocument d;
    d["detail"] = detail;
    send(d, ok ? 200 : 409);
  });
  server.on("/api/current/save", HTTP_POST, [] {
    if (!guard())
      return;
    float residual;
    if (!decimal("residual", .01, 1, residual)) {
      error("Residual limit .01..1mA", 400);
      return;
    }
    String detail;
    bool ok = saveCurrentCalibration(residual, detail);
    JsonDocument d;
    d["detail"] = detail;
    send(d, ok ? 200 : 409);
  });
  server.on("/api/current/reset", HTTP_POST, [] {
    if (!guard())
      return;
    resetCurrentPoints();
    JsonDocument d;
    d["ok"] = true;
    send(d);
  });
  server.on("/api/aht/config", HTTP_POST, [] {
    if (!guard())
      return;
    bool ok = configureAht10();
    JsonDocument d;
    d["ok"] = ok;
    d["detail"] = ok ? "AHT10 init sent; waiting measurement" : "AHT10 init failed";
    send(d, ok ? 200 : 409);
  });
  server.on("/api/ethernet", HTTP_POST, [] {
    if (!guard())
      return;
    stopSafe();
    String detail;
    bool ok = testEthernet(detail);
    qcRecord("Ethernet", ok ? "DETECTED" : "FAIL", detail);
    JsonDocument d;
    d["ok"] = ok;
    d["detail"] = detail;
    send(d);
  });
  server.on("/api/pd", HTTP_POST, [] {
    if (!guard())
      return;
    if (!qcPdEnabled()) {
      error("M1: PD Sync unpopulated; not applicable");
      return;
    }
    stopSafe();
    long mv, ma;
    if (!number("mv", 5000, 20000, mv) || !number("ma", 10, 5000, ma)) {
      error("Invalid PDO", 400);
      return;
    }
    String detail;
    JsonDocument calibration;
    calibrationState(calibration.to<JsonObject>());
    if (currentQcBusy() || calibration["pending"].as<int>() >= 0) {
      error("Wait for ADC capture before PD negotiation");
      return;
    }
    bool ok = requestPd(mv, ma, detail);
    qcRecord("STUSB4500", ok ? "NEGOTIATING" : "FAIL", detail);
    JsonDocument d;
    d["detail"] = detail;
    send(d, ok ? 200 : 409);
  });
  server.on("/api/modbus/config", HTTP_POST, [] {
    if (!guard())
      return;
    stopSafe();
    long b, s, f, r, c;
    String p = web->arg("parity"), mode = web->arg("route");
    if (!number("baud", 1200, 115200, b) || !number("slave", 1, 247, s) ||
        !number("function", 3, 4, f) || !number("register", 0, 65535, r) ||
        !number("count", 1, 125, c) || (uint32_t)r + c > 65536 ||
        (p != "N" && p != "E" && p != "O") || (mode != "RS485" && mode != "RS232")) {
      error("Invalid Modbus parameters", 400);
      return;
    }
    // Modbus transactions remain deferred; laptop serial QC selects its own route.
    if (!mcpBit(MCP2_ADDR, 2, mode == "RS232")) {
      error("MCP2A2 route write failed");
      return;
    }
    baud = b;
    slave = s;
    functionCode = f;
    regAddress = r;
    regCount = c;
    parity = p;
    route = mode;
    prefs.putUInt("baud", baud);
    prefs.putString("parity", parity);
    prefs.putUChar("slave", slave);
    prefs.putUChar("fn", functionCode);
    prefs.putUShort("reg", regAddress);
    prefs.putUShort("count", regCount);
    JsonDocument d;
    d["status"] = "DITUNDA";
    d["detail"] = "Config saved; MCP2A2 selected. Modbus remains deferred; use laptop serial QC for byte transactions.";
    send(d);
  });
}
