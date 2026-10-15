#include "radio_qc.h"
#include "config.h"
#include "qc_hardware.h"
#include "qc_session.h"
#include "ota_manager.h"
#include <HardwareSerial.h>

namespace {
HardwareSerial radio(2);
bool active = false, saved = false, entered = false, matchedEntry = false, applyConfig = false;
uint8_t oldDir = 0, oldLatch = 0;
uint32_t at = 0;
int stage = 0, command = 0;
String buffer, line, version, commStatus = "BELUM DIUJI", configStatus = "BELUM DIUJI", radioDetail;
// Manufacturer manual pp17-20. Writes only when explicit apply=1; no guessed LoRa parameters.
const char *keys[] = {"CNCFG", "WMCFG", "TMCFG", "PIDCFG", "UBCFG", "UPCFG", "PWCFG"};
const char *labels[] = {"Channel", "WMCFG", "TMCFG", "PIDCFG", "UBCFG", "UPCFG", "PWCFG"};
const int maxima[] = {63, 4, 1, 65535, 7, 2, 3};
int expected[7] = {0, 4, 1, 65535, 7, 0, 0}, actual[7];
bool restore() {
  if (!saved) return true;
  return mcpRestorePortBits(MCP1_ADDR,1,0x30,oldLatch,oldDir);
}
void publish() {
  qcRecord("Radio communication", commStatus, radioDetail);
  qcRecord("Radio configuration", configStatus, radioDetail);
}
void finish() {
  radio.end(); active = false;
  if (!restore()) { commStatus = configStatus = "FAIL"; radioDetail += "; MCP mode restore failed"; }
  saved = false; publish();
}
void exitMode() {
  // Exit is mode-only, not a persistent parameter mutation.
  radio.print("AT+EXIT\r\n"); buffer = line = ""; stage = 4; at = millis();
}
void evaluate() {
  bool missing = false, mismatch = false;
  radioDetail = "E70 UART readback:";
  for (int i = 0; i < 7; ++i) {
    radioDetail += String(" ") + keys[i] + "=" + (actual[i] < 0 ? "UNREADABLE" : String(actual[i]));
    if (actual[i] < 0) missing = true;
    else if (actual[i] != expected[i]) mismatch = true;
  }
  commStatus = entered && actual[0] >= 0 ? "PASS" : "FAIL";
  configStatus = mismatch || missing ? "FAIL" : "PASS";
  if (applyConfig) radioDetail += "; explicit AT writes acknowledged + settings readback; some parameters need radio restart to become active";
  radioDetail += "; SF/BW/CR/LoRa CRC NOT SUPPORTED; RF link not tested";
  exitMode();
}
void sendWrite() {
  buffer = line = ""; radio.print(String("AT+")+keys[command]+"="+String(expected[command])+"\r\n"); at=millis(); stage=5;
}
void sendQuery() {
  buffer = line = "";
  String request = command < 7 ? String("AT+") + keys[command] + "=?\r\n" : "AT+VER\r\n";
  radio.print(request); at = millis(); stage = 3;
}
void consumeLine() {
  line.trim();
  if (line == "Enter AT Mode") matchedEntry = true;
  if (stage == 3 && command < 7) {
    String prefix = String(labels[command]) + ":";
    // Some manual tables use U+FF1A full-width colon.
    String normalized = line; normalized.replace("：", ":");
    if (normalized.startsWith(prefix)) {
      String val = normalized.substring(prefix.length()); val.trim();
      bool digits = val.length() > 0 && val.length() <= 5;
      for (char c : val) if (c < '0' || c > '9') digits = false;
      if (digits && val.toInt() <= maxima[command]) actual[command] = val.toInt();
    }
  } else if (stage == 3 && command == 7 && line.length() && !line.startsWith("AT+") &&
             line != "OK" && !line.startsWith("ERROR")) version = line;
  line = "";
}
}
bool radioQcActive() { return active; }
void resetRadioQc() {
  if (active) return;
  commStatus = configStatus = "BELUM DIUJI"; radioDetail = version = "";
  for (auto &v : actual) v = -1;
}
void cancelRadioQc(const char *reason) {
  if (!active) return;
  commStatus = configStatus = "INCOMPLETE"; radioDetail = reason;
  // Stop callback also runs before OTA flash: restore pins immediately, never defer it
  // until a main loop that may restart. Exit is best effort; cancellation never PASSes.
  radio.print("AT+EXIT\r\n");
  finish();
}
void radioQcState(JsonObject out) {
  out["active"] = active; out["model"] = "E70-915NW14S"; out["technology"] = "CC1310 / 15.4 star (not LoRa)";
  out["communication"] = commStatus; out["configuration"] = configStatus;
  out["detail"] = radioDetail; out["versionResponse"] = version;
  out["sf"] = out["bandwidth"] = out["codingRate"] = out["loraCRC"] = "NOT SUPPORTED";
  auto params = out["parameters"].to<JsonArray>();
  for (int i = 0; i < 7; ++i) {
    auto p = params.add<JsonObject>(); p["name"] = keys[i]; p["target"] = expected[i];
    if (actual[i] >= 0) p["read"] = actual[i];
    p["status"] = actual[i] < 0 ? "UNVERIFIED" : actual[i] == expected[i] ? "MATCH" : "MISMATCH";
  }
  if (actual[0] >= 0 && actual[2] >= 0) out["frequencyMHz"] = 908.2 + .2 * actual[0];
  out["ack"] = digitalRead(34); out["aux"] = digitalRead(35); out["link"] = digitalRead(36);
}
void beginRadioQc(WebServer &web, bool (*authorize)(), void (*stop)()) {
  for (auto &v : actual) v = -1;
  web.on("/api/radio/start", HTTP_POST, [&web, authorize, stop] {
    if (!authorize()) return;
    JsonDocument d;
    int next[7]; bool valid = true;
    String apply = web.arg("apply");
    if (apply.length() && apply != "0" && apply != "1") valid=false;
    for (int i = 0; i < 7; ++i) {
      String s = web.arg(keys[i]);
      if (!s.length() || s.length() > 5) valid = false;
      for (char c : s) if (c < '0' || c > '9') valid = false;
      next[i] = s.toInt(); if (next[i] > maxima[i]) valid = false;
    }
    if (active || webOtaBusy() || pdNegotiating()) d["error"] = "Wait for active radio/OTA/PD";
    else if (!valid) d["error"] = "Invalid E70 targets";
    else {
      stop();
      qcRetest("Radio communication");qcRetest("Radio configuration");
      commStatus = configStatus = "TESTING"; radioDetail = "Selecting radio configuration pin mode";
      for (auto &v : actual) v = -1;
      publish();
      uint8_t bank;
      if (!qcReadReg(MCP1_ADDR, 0x0a, &bank, 1) || (bank & 0xa0) ||
          !qcReadReg(MCP1_ADDR, 0x01, &oldDir, 1) || !qcReadReg(MCP1_ADDR, 0x15, &oldLatch, 1))
        d["error"] = "MCP1 mode mapping unavailable";
      else {
        saved = true;
        uint8_t gpio = 0, dir = 0;
        if (!mcpBit(MCP1_ADDR, RADIO_M0_BIT, true) || !mcpBit(MCP1_ADDR, RADIO_M1_BIT, true) ||
            !qcReadReg(MCP1_ADDR, 0x13, &gpio, 1) || !qcReadReg(MCP1_ADDR, 0x01, &dir, 1) ||
            (gpio & 0x30) != 0x30 || (dir & 0x30)) {
          bool restored = restore(); saved = false;
          d["error"] = restored ? "Cannot enter E70 configuration pin mode" : "Mode entry AND restore failed";
        } else {
          for (int i = 0; i < 7; ++i) { expected[i] = next[i]; actual[i] = -1; }
          radio.setRxBufferSize(512); radio.begin(115200, SERIAL_8N1, RADIO_RX, RADIO_TX);
          if (!radio) {
            radio.end(); bool restored = restore(); saved = false;
            d["error"] = "Radio UART2 init failed";
            commStatus = "FAIL"; configStatus = "UNVERIFIED";
            radioDetail = restored ? "Radio UART2 init failed" : "Radio UART init AND mode restore failed"; publish();
            String body; serializeJson(d, body); web.send(409, "application/json", body); return;
          }
          applyConfig = apply == "1";
          entered = matchedEntry = false; command = stage = 0; buffer = line = version = "";
          active = true; at = millis(); commStatus = configStatus = "TESTING";
          radioDetail = applyConfig ? "Explicit E70 AT configuration writes; temporary M0/M1 HIGH, readback required" : "Read-only E70 AT test; temporary M0/M1 HIGH, no configuration writes"; publish();
          d["ok"] = true;
        }
      }
    }
    if (!d["error"].isNull() && commStatus == "TESTING") {
      commStatus = "FAIL"; configStatus = "UNVERIFIED"; radioDetail = d["error"].as<String>(); publish();
    }
    String body; serializeJson(d, body); web.send(d["error"].isNull() ? 200 : 409, "application/json", body);
  });
}
void updateRadioQc() {
  if (!active) return;
  uint32_t now = millis();
  for (int n = 0; n < 128 && radio.available(); ++n) {
    char c = radio.read();
    if (buffer.length() < 512) buffer += c;
    if (c == '\r' || c == '\n') consumeLine();
    else if (line.length() < 128) line += c;
  }
  if (stage == 0 && now - at >= 200) { radio.write(uint8_t(0)); stage = 1; at = now; }
  else if (stage == 1 && now - at >= 120) { radio.print("+++"); stage = 2; at = now; }
  else if (stage == 2) {
    if (matchedEntry || buffer.indexOf("Enter AT Mode") >= 0) { entered = true; if(applyConfig)sendWrite();else sendQuery(); }
    else if (now - at >= 1500) {
      commStatus = "FAIL"; configStatus = "FAIL"; radioDetail = "No valid Enter AT Mode response; verify E70 power/UART/mode"; exitMode();
    }
  } else if (stage == 5) {
    if (buffer.indexOf("+OK") >= 0) {
      if (++command < 7) sendWrite(); else { command=0; sendQuery(); }
    } else if (buffer.indexOf("ERROR") >= 0 || now-at >= 1500) {
      commStatus="UNVERIFIED"; configStatus="FAIL";
      radioDetail="AT write rejected/timeout at "+String(keys[command])+"; earlier settings may have changed, read/reapply before use"; exitMode();
    }
  } else if (stage == 3 && ((command < 7 && actual[command] >= 0) || now - at >= 1500)) {
    if (line.length()) consumeLine();
    if (++command <= 7) sendQuery(); else evaluate();
  } else if (stage == 4 && (buffer.indexOf("Exit AT Mode") >= 0 || now - at >= 500)) {
    if (buffer.indexOf("Exit AT Mode") < 0 && entered) {
      commStatus = configStatus = "FAIL"; radioDetail += "; AT exit unconfirmed; module may need power cycle";
    }
    finish();
  }
}
