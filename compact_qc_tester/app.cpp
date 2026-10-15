#include <Arduino.h>
#if !defined(CONFIG_IDF_TARGET_ESP32)
#error "Provisional V2_R1 profile supports classic ESP32 only. Do not build for ESP32-S3."
#endif
#include "config.h"
#include "ota_manager.h"
#include "qc_hardware.h"
#include "qc_session.h"
#include "rtc_qc.h"
#include "output_state.h"
#include "current_qc.h"
#include "modbus_qc.h"
#include "network_qc.h"
#include "serial_qc.h"
#include "radio_qc.h"
#include "tpl_watchdog.h"
#include "web_ui.h"
#include "qr_asset.h"
#include "wifi_manager.h"
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>
#include <driver/gpio.h>
#include <esp_timer.h>

WebServer server(80);
String ssid, token;
uint32_t lastHeartbeat = 0;
OutputState outputState;
uint32_t outputLimits[OutputState::COUNT]={};
bool mcpReady = false, fault = false;
bool relayReady[2] = {};
uint16_t savedLatch = 0, savedDirection = 0;
const uint8_t addresses[] = {0x68, 0x57, 0x38, 0x20, 0x21, 0x3c, 0x77, 0x28, 0x48};
const char *deviceNames[] = {"RTC",  "EEPROM",    "AHT",          "MCP1", "MCP2",
                             "OLED", "ATtiny404", "PD converter", "ADS"};

void reply(JsonDocument &doc, int code = 200) {
  if(doc["hardware"]["rtc"]["schema"]=="COMPACT-RTC-1"){
    int64_t now=esp_timer_get_time();doc["hardware"]["rtc"]["snapshotResponseUptimeUs"]=now;
    doc["hardware"]["rtc"]["sampleAgeUpperBoundMs"]=(now-doc["hardware"]["rtc"]["sampleUptimeUs"].as<int64_t>()+999)/1000;
  }
  String body;
  serializeJson(doc, body);
  server.sendHeader("Cache-Control", "no-store");
  server.send(code, "application/json", body);
}
void failure(const char *why, int code = 409) {
  JsonDocument d;
  d["error"] = why;
  reply(d, code);
}
bool authorized() {
  if (!server.hasArg("token") || server.arg("token") != token) {
    failure("Invalid session token", 403);
    return false;
  }
  if (!qcProfileSelected() && server.uri() != "/api/qc/new" && server.uri() != "/api/heartbeat" &&
      server.uri() != "/api/stop" && server.uri() != "/api/qc/profile" && server.uri() != "/api/qc/recipe" && server.uri() != "/api/qc/resume" && server.uri() != "/api/qc/import-legacy" && server.uri() != "/api/rtc/run/release") {
    failure("Pilih profil device sebelum QC");
    return false;
  }
  if (networkQcBusy() && server.uri() != "/api/heartbeat" && server.uri() != "/api/stop") {
    failure("Network worker owns SPI/settings; wait or Stop before other QC actions"); return false;
  }
  if (modbusQcActive() && server.uri() != "/api/heartbeat" && server.uri() != "/api/stop" && server.uri() != "/api/modbus/stop") {
    failure("Modbus owns UART/routing; wait or Stop before other QC actions"); return false;
  }
  if (serialQcActive() && server.uri() != "/api/heartbeat" && server.uri() != "/api/stop" &&
      server.uri() != "/api/rs232/stop" && server.uri() != "/api/serial/stop" && server.uri() != "/api/qc/new") {
    failure("RS232 test owns UART/routing; stop it before other QC actions");
    return false;
  }
  if (radioQcActive() && server.uri() != "/api/heartbeat" && server.uri() != "/api/stop") {
    failure("Radio test/mode restoration active; wait before other QC actions");
    return false;
  }
  const String uri=server.uri();
  String test;
  if(uri.startsWith("/api/rtc/")) test="RTC";
  else if(uri=="/api/aht/config") test="AHT10";
  else if(uri.startsWith("/api/identity")) test="EEPROM";
  else if(uri.startsWith("/api/pd")) test="STUSB4500";
  else if(uri=="/api/i2c") test="I2C scan";
  else if(uri=="/api/ethernet") test="Ethernet";
  else if(uri=="/api/sd") test="SD";
  else if(uri.startsWith("/api/current")) test="ADS / 4-20mA";
  else if(uri=="/api/serial/route" || uri=="/api/modbus/config") test=server.arg("route");
  else if(uri.startsWith("/api/modbus") && !uri.endsWith("/stop")) test="RS485";
  else if(uri=="/api/rs232/start") test="RS232";
  else if(uri=="/api/serial/start") test=server.hasArg("mode")?server.arg("mode"):"RS232";
  else if(uri=="/api/network/test") {
    const String kind=server.arg("kind");
    test=kind=="wifi"?"WiFi network":kind=="ethernet"?"Ethernet network":kind=="mqtt"?"MQTT":"";
  } else if(uri=="/api/radio/start") {
    if(server.arg("apply")=="1") test="Radio configuration";
    else if(!qcTestEnabled("Radio configuration")) test="Radio communication";
  } else if(uri=="/api/output" && server.arg("state")!="off") {
    test=(server.arg("kind")=="led"?String("LED:"):String("Relay:"))+server.arg("index");
  } else if(uri=="/api/qc/confirm") {
    const String kind=server.arg("kind");
    if(kind=="i2c") test="I2C scan";
    else if(kind=="oled") test="OLED";
    else if(kind=="led" || kind=="relay") test=(kind=="led"?String("LED:"):String("Relay:"))+server.arg("index");
  }
  if(test.length() && !qcTestEnabled(test)) {
    failure("Test OFF in selected QC profile; enable it in Konfigurasi QC first"); return false;
  }
  return true;
}
bool readMcp(uint8_t reg, uint16_t &value) {
  Wire.beginTransmission(MCP1_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0)
    return false;
  if (Wire.requestFrom(MCP1_ADDR, (uint8_t)2) != (uint8_t)2)
    return false;
  value = Wire.read();
  value |= (uint16_t)Wire.read() << 8;
  return true;
}
bool writeMcp(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(MCP1_ADDR);
  Wire.write(reg);
  Wire.write((uint8_t)value);
  Wire.write((uint8_t)(value >> 8));
  return Wire.endTransmission() == 0;
}
bool writeLedBits(uint16_t bits) {
  uint16_t current;
  if (!readMcp(0x14, current))
    return false;
  return writeMcp(0x14, (current & ~LED_MASK) | (bits & LED_MASK));
}
void stopOutputs() {
  cancelModbusQc(); cancelNetworkQc();
  cancelSerialQc("Cancelled by stop/new session/OTA");
  cancelRadioQc("Cancelled by stop/OTA");
  if (OUTPUT_PROFILE_VERIFIED) {
    for (int i = 0; i < 2; ++i)
      if (relayReady[i]) digitalWrite(RELAY_PINS[i], relayLevel(i, false));
    if (mcpReady) {
      const uint16_t off = LED_ACTIVE_HIGH ? 0 : LED_MASK;
      if (!writeLedBits(off))
        fault = true;
    }
  }
  outputState.clear();
  qcOutputsStopped();
}
void initOutputs() {
  if (!OUTPUT_PROFILE_VERIFIED)
    return;
  for (int i = 0; i < 2; ++i) {
    relayReady[i] = initializeRelay(i,
      [](int pin, int level) { return gpio_set_level((gpio_num_t)pin, level) == ESP_OK; },
      [](int pin) { pinMode(pin, OUTPUT); },
      [](int pin, int level) {
        digitalWrite(pin, level);
        return gpio_get_level((gpio_num_t)pin) == level;
      });
    if (!relayReady[i]) fault = true;
  }
  if (!MCP23017_PROFILE_VERIFIED)
    return;
  // Validate expected BANK=0/SEQOP=0 before changing only LED bits.
  Wire.beginTransmission(MCP1_ADDR);
  Wire.write(0x0a);
  if (Wire.endTransmission(false) != 0)
    return;
  if (Wire.requestFrom(MCP1_ADDR, (uint8_t)1) != 1)
    return;
  if ((Wire.read() & 0xa0) != 0)
    return;
  if (!readMcp(0x00, savedDirection) || !readMcp(0x14, savedLatch))
    return;
  const uint16_t off = LED_ACTIVE_HIGH ? 0 : LED_MASK;
  if (!writeLedBits(off))
    return;
  if (!writeMcp(0x00, savedDirection & ~LED_MASK))
    return;
  mcpReady = true;
}
void stateHandler() {
  JsonDocument d;
  d["board"] = "TMC TOP V2_R1 / BOTTOM V3_R0";
  d["version"] = FW_VERSION;
  d["buildUTC"] = FW_BUILD_UTC;
  d["buildIdentity"] = FW_BUILD_ID;
  d["appMD5"] = ESP.getSketchMD5();
  d["chip"] = ESP.getChipModel();
  d["ssid"] = ssid;
  d["staSsid"] = STA_SSID;
  d["staConnected"] = WiFi.status() == WL_CONNECTED;
  d["staIP"] = WiFi.localIP().toString();
  d["apIP"] = WiFi.softAPIP().toString();
  d["token"] = token;
  d["uptime"] = millis();
  d["otaSupported"] = true;
  d["otaBusy"] = webOtaBusy();
  d["tplReady"] = tplWatchdogReady();
  d["tplError"] = tplWatchdogError();
  d["tplPulses"] = tplWatchdogPulses();
  d["relayEnabled"] = OUTPUT_PROFILE_VERIFIED && !fault;
  d["ledEnabled"] = OUTPUT_PROFILE_VERIFIED && mcpReady && !fault;
  d["outputActive"] = outputState.any();
  d["outputFault"] = fault;
  d["watchdogPhysicalResetVerified"] = false;
  auto a = d["inputs"].to<JsonArray>();
  for (size_t i = 0; i < inputCount(); i++) {
    auto v = a.add<JsonObject>();
    v["name"] = inputName(i);
    int value = 0, pin;
    v["valid"] = readInput(i, value);
    v["value"] = value;
    if (inputPin(i, pin))
      v["pin"] = pin;
    else
      v["pin"] = "GPIO2 LOW & GPIO39 LOW";
  }
  auto l = d["leds"].to<JsonArray>();
  for (auto n : LED_NAMES)
    l.add(n);
  hardwareState(d["hardware"].to<JsonObject>());
  qcState(d["qc"].to<JsonObject>());
  for (int i = 0; i < 2; ++i) {
    auto p = d["qc"]["outputs"][11 + i].as<JsonObject>();
    p["pin"] = RELAY_PINS[i];
    p["activeHigh"] = RELAY_ACTIVE_HIGH[i];
    p["gpioReady"] = relayReady[i];
    p["expectedLevel"] = relayLevel(i, outputState.on[11 + i]);
    if (relayReady[i]) {
      int level = gpio_get_level((gpio_num_t)RELAY_PINS[i]);
      p["gpioLevel"] = level;
      p["gpioMatchesCommand"] = level == relayLevel(i, outputState.on[11 + i]);
    }
  }
  reply(d);
}
void scanHandler() {
  if (!authorized())
    return;
  stopOutputs();
  JsonDocument d;
  scanBus(d["devices"].to<JsonArray>());
  qcNoteI2cScan(d["devices"].size());
  reply(d);
}
void sdHandler() {
  if (!authorized())
    return;
  qcRetest("SD");
  stopOutputs();
  JsonDocument d;
  if (!SD.begin(SD_CS, SPI, 4000000)) {
    SD.end();
    d["status"] = "FAIL";
    d["detail"] = "SD mount failed";
    qcRecord("SD", "FAIL", "SD mount failed");
    reply(d);
    return;
  }
  char path[48];
  bool unique = false;
  for (int attempt = 0; attempt < 8; attempt++) {
    snprintf(path, sizeof(path), "/qc_%08lx.tmp", (unsigned long)esp_random());
    if (!SD.exists(path)) {
      unique = true;
      break;
    }
  }
  if (!unique) {
    SD.end();
    failure("Cannot allocate unique SD test file");
    return;
  }
  String payload = "COMPACT-QC:" + String(esp_random(), HEX);
  File f = SD.open(path, FILE_WRITE);
  bool ok = false;
  if (f) {
    ok = f.print(payload) == payload.length();
    f.flush();
    f.close();
  }
  String readback;
  f = SD.open(path, FILE_READ);
  if (f) {
    while (f.available() && readback.length() <= payload.length())
      readback += (char)f.read();
    f.close();
  }
  ok = ok && readback == payload;
  bool clean = SD.remove(path);
  SD.end();
  d["status"] = (ok && clean) ? "PASS" : "FAIL";
  qcRecord("SD", (ok && clean) ? "PASS" : "FAIL",
           (ok && clean) ? "Unique temporary file write/read + cleanup verified"
                         : "SD write/read or cleanup failed");
  d["detail"] = !clean ? String("Cleanup failed; test file: ") + path
                : ok   ? "Write/read matched; temporary file removed"
                       : "Write/read mismatch; temporary file removed";
  reply(d);
}
void outputHandler() {
  if (!authorized())
    return;
  if (webOtaBusy()) {
    failure("OTA in progress");
    return;
  }
  if (!OUTPUT_PROFILE_VERIFIED || fault) {
    failure("Outputs locked: verify config.h and wiring");
    return;
  }
  String raw = server.arg("index");
  if (raw.length() == 0) {
    failure("Missing index", 400);
    return;
  }
  for (unsigned i = 0; i < raw.length(); i++)
    if (!isDigit(raw[i])) {
      failure("Invalid index", 400);
      return;
    }
  long index = raw.toInt();
  String kind = server.arg("kind");
  if (kind != "relay" && kind != "led") {
    failure("Unknown output kind", 400);
    return;
  }
  if (index < 0 || index >= (kind == "relay" ? 2 : 11)) {
    failure("Index outside range", 400);
    return;
  }
  if (kind == "led" && !mcpReady) {
    failure("MCP profile unavailable");
    return;
  }
  String state = server.arg("state");
  if (state != "on" && state != "off") {
    failure("Output state on/off required", 400);
    return;
  }
  // An authenticated explicit command is fresh operator activity. Heartbeat loss
  // still turns active outputs OFF; OFF must never require a preceding heartbeat.
  uint32_t limit=OUTPUT_PULSE_MS;
  if(server.hasArg("seconds")) {
    String seconds=server.arg("seconds");
    if(seconds.length()!=1 || seconds[0]<'1' || seconds[0]>'9') {
      if(seconds!="10"){failure("Step timeout1..10 seconds",400);return;}
    }
    limit=seconds.toInt()*1000;
  }
  lastHeartbeat = millis();
  const bool on = state == "on";
  if (kind == "relay") {
    if (!relayReady[index]) { failure("DO GPIO initialization failed"); return; }
    digitalWrite(RELAY_PINS[index], relayLevel(index, on));
    if (gpio_get_level((gpio_num_t)RELAY_PINS[index]) != relayLevel(index, on)) {
      fault = true;
      stopOutputs();
      failure("DO GPIO level mismatch; check pin/driver/wiring. Outputs stopped");
      return;
    }
  }
  else if (!mcpBit(MCP1_ADDR, LED_BITS[index], on == LED_ACTIVE_HIGH)) {
    fault = true;
    stopOutputs();
    failure("MCP command/readback failed; outputs stopped");
    return;
  }
  outputLimits[kind=="led"?index:11+index]=limit;
  outputState.set(kind == "led" ? index : 11 + index, on, millis());
  qcOutputAttempt(kind, index, on);
  JsonDocument d;
  d["ok"] = true;
  d["commandedOn"] = on;
  reply(d);
}
void beginApp() {
  Serial.begin(115200);
  for (int p : INPUT_PINS)
    pinMode(p, INPUT);
  Wire.begin(SDA_PIN, SCL_PIN, 100000);
  Wire.setTimeOut(40);
  // Keep Ethernet deselected for SD tests. Boot-time strap levels need hardware verification.
  digitalWrite(ETH_CS, HIGH);
  pinMode(ETH_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  pinMode(SD_CS, OUTPUT);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  initOutputs();
  beginTplWatchdog();
  beginQcSession(server, authorized, stopOutputs);
  beginQcHardware();
  beginRtcQc(server,authorized,stopOutputs);
  beginCurrentQc(server, authorized);
  beginSerialQc(server, authorized, stopOutputs);
  beginModbusQc(server, authorized, stopOutputs);
  beginNetworkQc(server, authorized, stopOutputs);
  beginRadioQc(server, authorized, stopOutputs);
  uint64_t mac = ESP.getEfuseMac();
  char name[32];
  snprintf(name, sizeof(name), "COMPACT-QC-%04X", (unsigned)(mac & 0xffff));
  ssid = name;
  token = String(esp_random(), HEX) + String(esp_random(), HEX);
  beginLabWiFi(ssid.c_str());
  server.on("/", HTTP_GET, [] { server.send_P(200, "text/html; charset=utf-8", WEB_UI); });
  server.on("/assets/qrcode.js",HTTP_GET,[]{server.send_P(200,"application/javascript; charset=utf-8",QR_ASSET);});
  server.on("/api/state", HTTP_GET, stateHandler);
  server.on("/api/i2c", HTTP_POST, scanHandler);
  server.on("/api/sd", HTTP_POST, sdHandler);
  server.on("/api/output", HTTP_POST, outputHandler);
  server.on("/api/heartbeat", HTTP_POST, [] {
    if (!authorized())
      return;
    lastHeartbeat = millis();
    JsonDocument d;
    d["ok"] = true;
    reply(d);
  });
  server.on("/api/stop", HTTP_POST, [] {
    if (!authorized())
      return;
    stopOutputs();
    JsonDocument d;
    d["ok"] = !fault;
    reply(d, fault ? 500 : 200);
  });
  server.onNotFound([] { failure("Not found", 404); });
  beginWebOta(server, stopOutputs);
  server.begin();
  Serial.printf("Serial: %s\nBuild UTC: %s\n", unitSerial(), FW_BUILD_UTC);
  Serial.println(FW_BUILD_ID);
  Serial.printf("\nCompact QC %s\nMCU: %s\nAP: %s\nURL: http://192.168.4.1\nOutputs: %s\n",
                FW_VERSION, ESP.getChipModel(), ssid.c_str(),
                OUTPUT_PROFILE_VERIFIED ? "verified configuration" : "LOCKED");
}
void updateApp() {
  if (outputState.any() && uint32_t(millis() - lastHeartbeat) >= SESSION_TIMEOUT_MS)
    stopOutputs();
  for (int n = 0; n < OutputState::COUNT; ++n) {
    if (!outputState.expired(n, millis(), outputLimits[n]?outputLimits[n]:OUTPUT_PULSE_MS)) continue;
    if (n >= 11) digitalWrite(RELAY_PINS[n - 11], relayLevel(n - 11, false));
    else if (!mcpBit(MCP1_ADDR, LED_BITS[n], !LED_ACTIVE_HIGH)) {
      fault = true; stopOutputs(); break;
    }
    outputState.set(n, false, millis());
    qcOutputAttempt(n < 11 ? "led" : "relay", n < 11 ? n : n - 11, false);
  }
  updateWebOta();
  updateLabWiFi();
  updateQcSession();
  updateModbusQc();
  updateNetworkQc();
  updateSerialQc();
  updateRadioQc();
  if (!webOtaBusy())
    updateQcHardware();
  updateTplWatchdog();
  server.handleClient();
  updateTplWatchdog();
  yield();
}
