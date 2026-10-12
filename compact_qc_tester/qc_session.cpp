#include "qc_session.h"
#include "config.h"
#include "ota_manager.h"
#include "qc_hardware.h"
#include <Arduino.h>
#include <Preferences.h>
namespace {
WebServer *web;
bool (*authorized)();
void (*stopSafe)();
QcItem items[160];
size_t count = 0, display = 0;
uint32_t displayAt = 0;
String operatorName, unit, active, kind;
uint32_t activeAt = 0, lastSignalAt = 0;
int expected = 0, inputIndex = -1, observed = -1;
bool onConfirmed = false, observedExpected = false;
float targetMA = 0, tolerance = 0;
String frozenEvidence;
Preferences prefs;
uint32_t baud = 9600;
uint8_t slave = 1, functionCode = 3;
uint16_t regAddress = 0, regCount = 1;
String parity = "N", route = "RS485";
void send(JsonDocument &d, int code = 200) {
  String body;
  serializeJson(d, body);
  web->sendHeader("Cache-Control", "no-store");
  web->send(code, "application/json", body);
}
void error(const char *detail, int code = 409) {
  JsonDocument d;
  d["error"] = detail;
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
bool number(const char *arg, long low, long high, long &value) {
  if (!web->hasArg(arg))
    return false;
  String s = web->arg(arg);
  if (s.isEmpty())
    return false;
  for (auto c : s)
    if (!isDigit(c))
      return false;
  value = s.toInt();
  return value >= low && value <= high;
}
bool decimal(const char *arg, float low, float high, float &v) {
  if (!web->hasArg(arg))
    return false;
  String s = web->arg(arg);
  char *end;
  v = strtof(s.c_str(), &end);
  return end != s.c_str() && *end == 0 && isfinite(v) && v >= low && v <= high;
}
void reset() {
  count = 0;
  active = "";
  kind = "";
  onConfirmed = false;
  observedExpected = false;
  frozenEvidence = "";
  display = 0;
  const char *modules[] = {
      "OLED",      "I2C scan",     "RTC", "EEPROM", "AHT",          "MCP1",
      "MCP2",      "ADS / 4-20mA", "4mA", "12mA",   "20mA",         "STUSB4500",
      "ATtiny404", "Ethernet",     "SD",  "LoRa",   "Reset button", "TPL physical reset"};
  for (auto name : modules)
    qcRecord(name, "BELUM DIUJI", "");
  for (size_t i = 0; i < inputCount(); i++)
    qcRecord(String("Input:") + inputName(i), "BELUM DIUJI", "");
  for (int i = 0; i < 11; i++)
    qcRecord("LED:" + String(i), "BELUM DIUJI", "");
  for (int i = 0; i < 2; i++)
    qcRecord("Relay:" + String(i), "BELUM DIUJI", "");
  qcRecord("RS485", "DITUNDA", "External device test deferred by team");
  qcRecord("RS232", "DITUNDA", "External device test deferred by team");
}
void start() {
  if (!guard())
    return;
  if (operatorName.isEmpty()) {
    error("Start inspection with operator identity first");
    return;
  }
  stopSafe();
  onConfirmed = false;
  observedExpected = false;
  observed = -1;
  inputIndex = -1;
  frozenEvidence = "";
  active = "";
  kind = web->arg("kind");
  activeAt = millis();
  if (kind == "input") {
    long i, on;
    if (!number("index", 0, inputCount() - 1, i) || !number("expectedOn", 0, 1, on)) {
      error("Input index/ON level invalid", 400);
      return;
    }
    inputIndex = i;
    expected = on;
    active = "Input:" + String(inputName(i));
  } else if (kind == "current") {
    if (!decimal("target", 4, 20, targetMA) || !decimal("tolerance", 0.01, 2, tolerance)) {
      error("Current target/tolerance invalid", 400);
      return;
    }
    float ma;
    int16_t raw;
    uint32_t age;
    if (!currentSample(ma, raw, age)) {
      error("Verify ADS1115 profile and wait for fresh sample");
      return;
    }
    active = "ADS / 4-20mA";
    if (targetMA == 4 || targetMA == 12 || targetMA == 20)
      qcRecord(String((int)targetMA) + "mA", "MENUNGGU USER",
               "Apply calibrator signal, then confirm");
  } else if (kind == "manual") {
    String item = web->arg("item");
    bool found = false;
    for (size_t i = 0; i < count; i++)
      if (items[i].name == item)
        found = true;
    if (!found || item.startsWith("Input:") || item.startsWith("Relay:") ||
        item.startsWith("LED:") || item == "RS485" || item == "RS232" ||
        (item == "ADS / 4-20mA" || item == "4mA" || item == "12mA" || item == "20mA")) {
      error("Unsupported manual item", 400);
      return;
    }
    active = item;
  } else {
    error("Unknown test kind", 400);
    return;
  }
  qcRecord(active, "MENUNGGU USER",
           kind == "input" ? "Apply ON stimulus; hold stable, then confirm"
                           : "Apply stimulus and compare with external reference");
  JsonDocument d;
  d["ok"] = true;
  send(d);
}
void feedback() {
  if (!guard())
    return;
  if (active.isEmpty()) {
    error("No active test");
    return;
  }
  String verdict = web->arg("verdict");
  if (verdict != "PASS" && verdict != "FAIL") {
    error("PASS/FAIL required", 400);
    return;
  }
  if ((uint32_t)(millis() - activeAt) > 180000) {
    error("Test expired; start again");
    return;
  }
  String detail = web->arg("note");
  if (detail.length() > 240) {
    error("Note too long", 400);
    return;
  }
  if (verdict == "FAIL") {
    stopSafe();
    if (kind == "current" && (targetMA == 4 || targetMA == 12 || targetMA == 20))
      qcRecord(String((int)targetMA) + "mA", "FAIL", detail);
    qcRecord(active, "FAIL", frozenEvidence + "; " + detail);
    active = "";
  } else if (kind == "input") {
    int live;
    if (!readInput(inputIndex, live) || live != expected || !observedExpected) {
      error("Expected input not observed stable for 300ms");
      return;
    }
    frozenEvidence += String(onConfirmed ? " OFF=" : " ON=") + live;
    if (!onConfirmed) {
      onConfirmed = true;
      expected = 1 - expected;
      observedExpected = false;
      observed = -1;
      activeAt = millis();
      qcRecord(active, "MENUNGGU OFF", frozenEvidence + "; apply opposite stimulus, confirm again");
    } else {
      qcRecord(active, "PASS", frozenEvidence + "; operator confirmed both states; " + detail);
      active = "";
    }
  } else if (kind == "current") {
    float ma;
    int16_t raw;
    uint32_t age;
    if (!currentSample(ma, raw, age) || fabs(ma - targetMA) > tolerance) {
      error("Fresh measured current outside target tolerance");
      return;
    }
    String evidence = "Target=" + String(targetMA, 3) + "mA, measured=" + String(ma, 3) +
                      "mA, raw=" + String(raw) + ", tolerance=" + String(tolerance, 3) +
                      "mA; operator confirmed; " + detail;
    String point = targetMA == 4    ? "4mA"
                   : targetMA == 12 ? "12mA"
                   : targetMA == 20 ? "20mA"
                                    : "Current custom point";
    qcRecord(point, "PASS", evidence);
    bool all = true;
    for (const char *name : {"4mA", "12mA", "20mA"}) {
      bool pass = false;
      for (size_t i = 0; i < count; i++)
        if (items[i].name == name)
          pass = items[i].status == "PASS";
      all &= pass;
    }
    qcRecord(active, all ? "PASS" : "INCOMPLETE",
             all ? "4/12/20mA measured + confirmed" : evidence + "; complete 4/12/20mA points");
    active = "";
  } else if (kind == "output") {
    if (!onConfirmed) {
      onConfirmed = true;
      stopSafe();
      qcRecord(active, "MENUNGGU OFF", "ON confirmed; outputs stopped; confirm physical OFF");
      activeAt = millis();
    } else {
      qcRecord(active, "PASS", "Operator confirmed ON and OFF; " + detail);
      active = "";
    }
  } else {
    if (detail.isEmpty()) {
      error("Describe observed result/reference in note", 400);
      return;
    }
    qcRecord(active, "PASS", "Operator observation: " + detail);
    active = "";
  }
  JsonDocument d;
  d["ok"] = true;
  send(d);
}
} // namespace
void qcRecord(const String &name, const String &status, const String &detail) {
  size_t i = 0;
  for (; i < count; i++)
    if (items[i].name == name)
      break;
  if (i == count) {
    if (count >= 160)
      return;
    count++;
  }
  items[i] = {name, status, detail};
  display = i;
  displayAt = millis();
}
void qcOutputAttempt(const String &type, int index, bool on) {
  active = (type == "led" ? "LED:" : "Relay:") + String(index);
  kind = "output";
  activeAt = millis();
  onConfirmed = false;
  qcRecord(active, "MENUNGGU USER",
           on ? "Command sent; confirm physical ON, then OFF" : "Confirm physical OFF");
}
const QcItem &qcDisplayItem() { return items[display]; }
const char *qcDisplayDetail() {
  return active.isEmpty() ? "Review all QC items"
                          : (onConfirmed ? "Confirm OFF" : "Confirm observation");
}
void updateQcSession() {
  uint32_t now = millis();
  if (now - displayAt > 2500 && count) {
    displayAt = now;
    display = (display + 1) % count;
  }
  if (!active.isEmpty() && now - activeAt > 180000) {
    stopSafe();
    qcRecord(active, "TIMEOUT", "No operator completion in 180 seconds");
    active = "";
  }
  if (kind == "input" && !active.isEmpty()) {
    int value;
    if (!readInput(inputIndex, value)) {
      observedExpected = false;
      observed = -1;
      lastSignalAt = now;
      return;
    }
    if (value != observed) {
      observed = value;
      lastSignalAt = now;
      observedExpected = false;
    }
    if (value == expected && now - lastSignalAt >= 300)
      observedExpected = true;
  }
}
void qcState(JsonObject d) {
  d["unit"] = unit;
  d["operator"] = operatorName;
  d["active"] = active;
  d["kind"] = kind;
  d["phase"] = onConfirmed ? "OFF" : "ON";
  d["targetMA"] = targetMA;
  d["expected"] = expected;
  d["signalReady"] = observedExpected;
  auto a = d["tests"].to<JsonArray>();
  bool failed = false, all = true;
  for (size_t i = 0; i < count; i++) {
    auto item = a.add<JsonObject>();
    item["name"] = items[i].name;
    item["status"] = items[i].status;
    item["detail"] = items[i].detail;
    failed |= items[i].status == "FAIL";
    all &= items[i].status == "PASS";
  }
  d["overall"] = failed ? "FAIL" : all ? "PASS" : "INCOMPLETE";
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
    if (name.isEmpty() || name.length() > 64) {
      error("Operator required, maximum 64 characters", 400);
      return;
    }
    stopSafe();
    reset();
    operatorName = name;
    unit = unitSerial();
    JsonDocument d;
    d["ok"] = true;
    send(d);
  });
  server.on("/api/qc/start", HTTP_POST, start);
  server.on("/api/qc/feedback", HTTP_POST, feedback);
  server.on("/api/identity", HTTP_POST, [] {
    if (!guard())
      return;
    stopSafe();
    String detail;
    bool ok = saveIdentity(web->arg("top"), web->arg("bottom"), detail);
    qcRecord("EEPROM", ok ? "PASS" : "FAIL", detail);
    JsonDocument d;
    d["status"] = ok ? "PASS" : "FAIL";
    d["detail"] = detail;
    send(d, ok ? 200 : 409);
  });
  server.on("/api/aht/config", HTTP_POST, [] {
    if (!guard())
      return;
    if (web->arg("model") != "AHT20" || web->arg("verified") != "yes") {
      error("Confirm AHT20 fitted first", 400);
      return;
    }
    if (!configureAht20()) {
      error("AHT20 initialization failed");
      return;
    }
    JsonDocument d;
    d["ok"] = true;
    send(d);
  });
  server.on("/api/current/config", HTTP_POST, [] {
    if (!guard())
      return;
    stopSafe();
    float shunt, gain, offset;
    if (!decimal("shunt", 1, 1000, shunt) || !decimal("gain", 0.001, 100, gain) ||
        !decimal("offset", -6, 6, offset) || web->arg("model") != "ADS1115" ||
        web->arg("verified") != "yes") {
      error("Verify ADS1115 and physical transfer function first", 400);
      return;
    }
    if (!configureCurrent(true, shunt, gain, offset)) {
      error("ADS configuration/readback failed");
      return;
    }
    for (const char *name : {"ADS / 4-20mA", "4mA", "12mA", "20mA"})
      qcRecord(name, "BELUM DIUJI", "ADC profile changed; repeat calibration points");
    JsonDocument d;
    d["ok"] = true;
    send(d);
  });
  server.on("/api/ethernet", HTTP_POST, [] {
    if (!guard())
      return;
    stopSafe();
    String detail;
    bool ok = testEthernet(detail);
    qcRecord("Ethernet", ok ? "MENUNGGU USER" : "FAIL", detail);
    if (ok) {
      active = "Ethernet";
      kind = "manual";
      activeAt = millis();
    }
    JsonDocument d;
    d["detail"] = detail;
    d["ok"] = ok;
    send(d);
  });
  server.on("/api/pd", HTTP_POST, [] {
    if (!guard())
      return;
    stopSafe();
    long mv, ma;
    if (!number("mv", 5000, 20000, mv) || !number("ma", 10, 5000, ma)) {
      error("PD voltage/current invalid", 400);
      return;
    }
    String detail;
    bool ok = requestPd(mv, ma, detail);
    qcRecord("STUSB4500", ok ? "MENUNGGU USER" : "FAIL", detail);
    if (ok) {
      active = "STUSB4500";
      kind = "manual";
      activeAt = millis();
    }
    JsonDocument d;
    d["detail"] = detail;
    d["ok"] = ok;
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
