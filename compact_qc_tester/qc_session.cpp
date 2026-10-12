#include "qc_session.h"
#include "config.h"
#include "ota_manager.h"
#include "qc_hardware.h"
#include "current_calibration.h"
#include "qc_math.h"
#include <Preferences.h>
namespace {
WebServer *web;
bool (*authorized)();
void (*stopSafe)();
QcItem items[80];
size_t count = 0, display = 0;
uint32_t displayAt = 0;
String operatorName, unit;
InputCycles cycles[7];
bool live[13] = {}, attempted[13] = {}, onConfirmed[13] = {}, offConfirmed[13] = {};
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
  count = display = 0;
  for (auto &c : cycles)
    c = {};
  for (int i = 0; i < 13; ++i)
    live[i] = attempted[i] = onConfirmed[i] = offConfirmed[i] = false;
  for (const char *m : {"OLED", "I2C scan", "RTC", "EEPROM", "AHT10", "ADS / 4-20mA", "STUSB4500",
                        "Ethernet", "SD"})
    qcRecord(m, "BELUM DIUJI", "");
  for (size_t i = 0; i < 7; ++i)
    qcRecord("Input:" + String(inputName(i)), "MENUNGGU SIGNAL",
             "Dua siklus HIGH-LOW-HIGH, debounce 50ms; tanpa konfirmasi operator");
  for (int i = 0; i < 13; ++i)
    qcRecord(outputName(i), "BELUM DIUJI", "");
  qcRecord("RS485", "DITUNDA", "External device test deferred by user");
  qcRecord("RS232", "DITUNDA", "External device test deferred by user");
}
} // namespace
void qcRecord(const String &name, const String &status, const String &detail) {
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
void qcOutputAttempt(const String &type, int index, bool on) {
  int n = type == "led" ? index : 11 + index;
  if (n < 0 || n >= 13)
    return;
  live[n] = on;
  if (on) {
    attempted[n] = true;
    onConfirmed[n] = offConfirmed[n] = false;
    qcRecord(outputName(n), "MENUNGGU USER", "Command ON; konfirmasi bahwa output fisik menyala");
  }
}
const QcItem &qcDisplayItem() { return items[display]; }
const char *qcDisplayDetail() { return "Input auto 2 cycles"; }
void updateQcSession() {
  uint32_t now = millis();
  if (now - displayAt > 2500 && count) {
    displayAt = now;
    display = (display + 1) % count;
  }
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
  auto a = d["tests"].to<JsonArray>();
  bool fail = false, all = true;
  for (size_t i = 0; i < count; ++i) {
    auto p = a.add<JsonObject>();
    p["name"] = items[i].name;
    p["status"] = items[i].status;
    p["detail"] = items[i].detail;
    fail |= items[i].status == "FAIL";
    all &= items[i].status == "PASS";
  }
  d["overall"] = fail ? "FAIL" : all ? "PASS" : "INCOMPLETE";
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
  }
  auto m = d["modbus"].to<JsonObject>();
  m["baud"] = baud;
  m["parity"] = parity;
  m["slave"] = slave;
  m["function"] = functionCode;
  m["register"] = regAddress;
  m["count"] = regCount;
  m["route"] = route;
  m["testStatus"] = "DITUNDA";
}
void beginQcSession(WebServer &server, bool (*authorize)(), void (*stop)()) {
  web = &server;
  authorized = authorize;
  stopSafe = stop;
  reset();
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
    stopSafe();
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
    if (type == "oled") {
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
      String expected = web->arg("state");
      if (expected != "on" && expected != "off") {
        error("Expected state required", 400);
        return;
      }
      if (!attempted[n] || (expected == "on" && !live[n]) ||
          (expected == "off" && (live[n] || !onConfirmed[n]))) {
        error("Confirm only attempted current state; ON confirmation required before OFF");
        return;
      }
      if (verdict == "FAIL") {
        stopSafe();
        qcRecord(outputName(n), "FAIL",
                 "User physical observation differs from commanded " + expected);
      } else {
        if (expected == "on")
          onConfirmed[n] = true;
        else
          offConfirmed[n] = true;
        qcRecord(outputName(n), "PASS",
                 offConfirmed[n] ? "User confirmed physical ON and OFF"
                                 : "User confirmed physical ON; OFF remains unconfirmed");
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
    stopSafe();
    long mv, ma;
    if (!number("mv", 5000, 20000, mv) || !number("ma", 10, 5000, ma)) {
      error("Invalid PDO", 400);
      return;
    }
    String detail;
    JsonDocument calibration;
    calibrationState(calibration.to<JsonObject>());
    if (calibration["pending"].as<int>() >= 0) {
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
    // User deferred external communication tests. Only routing/configuration is enabled.
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
    d["detail"] = "Config saved; MCP2A2 selected. No UART/Modbus traffic until external fixture "
                  "test is enabled.";
    send(d);
  });
}
