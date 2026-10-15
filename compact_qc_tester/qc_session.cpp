#include "qc_session.h"
#include "rtc_qc.h"
#include "qc_profiles.h"
#include "qc_recipes.h"
#include "config.h"
#include "ota_manager.h"
#include "qc_hardware.h"
#include "current_calibration.h"
#include "current_qc.h"
#include "modbus_qc.h"
#include "network_qc.h"
#include "qc_math.h"
#include "qc_report_math.h"
#include "qc_reset_scope.h"
#include "qc_export.h"
#include "serial_qc.h"
#include "radio_qc.h"
#include <Preferences.h>
namespace {
WebServer *web;
bool (*authorized)();
void (*stopSafe)();
QcItem items[80];
bool retained[80]={};
JsonDocument retainedEvidence;
JsonDocument lastReset;
size_t count = 0, display = 0;
uint32_t displayAt = 0;
String operatorName, unit,pcbNumber;

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
  pcbNumber="";
  lastReset.clear();
  qcProgressNewSession();resetRtcQc();retainedEvidence.clear();for(auto &b:retained)b=false;
  resetSerialQc(); resetRadioQc(); resetCurrentQc(); resetModbusQc(); resetNetworkQc();
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
    qcRecord(m, "BELUM DIUJI", "");
  for (size_t i = 0; i < 7; ++i)
    qcRecord("Input:" + String(inputName(i)), "MENUNGGU SIGNAL",
             "Dua siklus HIGH-LOW-HIGH, debounce 50ms; tanpa konfirmasi operator");
  for (int i = 0; i < 13; ++i)
    qcRecord(outputName(i), "BELUM DIUJI", "");
  for (const char *name : {"Modbus tag:1", "Modbus tag:2", "WiFi network", "Ethernet network", "MQTT"}) qcRecord(name, "BELUM DIUJI", "");
  qcRecord("RS485", "BELUM DIUJI", "Laptop USB-RS485; automatic direction confirmed by user");
  qcRecord("RS232", "BELUM DIUJI", "Laptop USB-RS232 QC protocol required");
  qcRecord("Radio communication", "BELUM DIUJI", "E70-915NW14S UART AT readback");
  qcRecord("Radio configuration", "BELUM DIUJI", "Compare supported parameters; LoRa SF/BW/CR unsupported");
}
} // namespace
bool qcProfileSelected() { return qcProfileIsSelected(); }
bool qcPdEnabled() { return qcProfileSelected() && qcTestEnabled("STUSB4500"); }
const char *qcVariantName() { return activeQcProfileName(); }
void qcRecord(const String &name, const String &status, const String &detail) {
  String effectiveStatus = status, effectiveDetail = detail;
  if (qcProfileSelected() && !qcTestEnabled(name)) {
    effectiveStatus = "IGNORED BY PROFILE";
    effectiveDetail = String(qcVariantName()) + " rev" + activeQcProfileRevision() + ": test OFF; excluded from QC, not PASS";
  }
  size_t i = 0;
  for (; i < count; ++i)
    if (items[i].name == name)
      break;
  if (i == count) {
    if (count >= 80)
      return;
    ++count;
  }
  if(retained[i] && name!="RTC")return;
  if (items[i].status != effectiveStatus || items[i].detail != effectiveDetail) {
    qcProgressDirty();
    display = i;
    displayAt = millis();
  }
  items[i] = {name, effectiveStatus, effectiveDetail};
}
void qcOutputsStopped() {
  for (auto &b : live)
    b = false;
}
void qcNoteI2cScan(unsigned devices) {
  qcRetest("I2C scan");
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
  updateRtcQc();updateQcProgress();
  if (!qcProfileSelected())
    return;
  for (size_t i = 0; i < 7; ++i) {
    if (!qcTestEnabled("Input:" + String(inputName(i)))) continue;
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
  d["pcbNumber"] = pcbNumber;
  d["lastReset"].set(lastReset.as<JsonObject>());
  d["variant"] = qcVariantName();
  d["profileSelected"] = qcProfileSelected();
  qcProfilesState(d);
  qcRecipesState(d);
  d["sessionId"] = sessionId;
  qcProgressState(d["progress"].to<JsonObject>());
  rtcExportEvidence(d["rtcEvidence"].to<JsonObject>());
  auto scan = d["i2c"].to<JsonObject>();
  scan["scanned"] = i2cScanned; scan["devices"] = i2cDevices;
  auto a = d["tests"].to<JsonArray>();
  unsigned passed = 0, failed = 0, pending = 0, skipped = 0;
  String canonical = String("QC1|") + FW_VERSION + "|" + FW_BUILD_UTC + "|" + qcVariantName() +
                     "|" + unitSerial() + "|" + sessionId + "|" + activeQcProfileRevision() + "|" + activeQcProfileMask()+"|RECIPE:"+activeQcRecipeIdentity()+"|PCB:"+String(pcbNumber.length())+":"+pcbNumber;
  auto hashField = [&](const String &value) {
    canonical += "|" + String(value.length()) + ":" + value;
  };
  for (size_t i = 0; i < count; ++i) {
    auto p = a.add<JsonObject>();
    p["name"] = items[i].name;
    p["status"] = items[i].status;
    p["detail"] = items[i].detail;
    p["restoredEvidence"]=retained[i];
    auto affected=p["resetTargets"].to<JsonArray>();
    for(size_t j=0;j<count;++j)if(qcResetAffects(items[i].name.c_str(),items[j].name.c_str()))affected.add(items[j].name);
    bool applicable = qcProfileSelected() && qcTestEnabled(items[i].name);
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
  hashField(rtcEvidenceIdentity());
  String resetIdentity;serializeJson(lastReset,resetIdentity);hashField(resetIdentity);
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
  m["testStatus"] = "Modbus master2 tags; CRC response and user confirmation";
  modbusQcState(m);
  networkQcState(d["network"].to<JsonObject>());
  serialQcState(d["serial"].to<JsonObject>());
  radioQcState(d["radio"].to<JsonObject>());
  if(retainedEvidence.size())d["checkpointEvidence"].set(retainedEvidence.as<JsonObject>());
}
void beginQcSession(WebServer &server, bool (*authorize)(), void (*stop)()) {
  beginQcProfiles();
  beginQcRecipes();
  web = &server;
  authorized = authorize;
  stopSafe = stop;
  reset();
  beginQcExport(server);
  beginQcProgress();
  prefs.begin("qc-settings", false);
  baud = prefs.getUInt("baud", 9600);
  parity = prefs.getString("parity", "N");
  slave = prefs.getUChar("slave", 1);
  functionCode = prefs.getUChar("fn", 3);
  regAddress = prefs.getUShort("reg", 0);
  regCount = prefs.getUShort("count", 1);
  server.on("/api/qc/checkpoint",HTTP_POST,[]{
    if(!guard())return;if(web->arg("confirm")!="1"||web->arg("sessionId")!=sessionId){error("Checkpoint confirm/session required",400);return;}
    stopSafe();String why;bool ok=qcSaveProgress(why);JsonDocument d;d["ok"]=ok;d["detail"]=why;send(d,ok?200:507);
  });
  server.on("/api/qc/resume",HTTP_POST,[]{
    if(!guard())return;if(web->arg("confirm")!="1"){error("Explicit resume confirmation required",400);return;}
    stopSafe();String why;bool ok=qcResumeProgress(why,web->arg("upgradeFromBuild"));JsonDocument d;d["ok"]=ok;d["detail"]=why;send(d,ok?200:409);
  });
  server.on("/api/qc/checkpoint",HTTP_GET,[]{JsonDocument d;qcSavedProgress(d.to<JsonObject>());send(d);});
  server.on("/api/qc/import-legacy",HTTP_POST,[]{
    if(!guard())return;if(web->arg("confirm")!="1"){error("Explicit legacy import confirmation required",400);return;}
    if(qcProfileSelected()){error("Legacy import cannot replace an active QC session");return;}
    // Free the cold NVS cache before copying/parsing a large HTTP argument; reload it for validation.
    qcReleaseProgressCache();String payload=web->arg("snapshot");
    if(payload.isEmpty()||payload.length()>50000){payload=String();beginQcProgress();error("Bounded legacy snapshot required",400);return;}
    JsonDocument filter;for(const char *k:{"version","buildUTC","buildIdentity"})filter[k]=true;
    filter["hardware"]["serial"]=true;for(const char *k:{"active","gain","offset"})filter["hardware"]["calibration"][k]=true;
    filter["hardware"]["currentQc"]=true;
    for(const char *k:{"variant","profileSelected","sessionId","profileRevision","profileMask","operator","overall","qcCode","modbus","network","serial","radio"})filter["qc"][k]=true;
    for(const char *k:{"revision","crc"})filter["qc"]["recipe"][k]=true;
    for(const char *k:{"name","status","detail"})filter["qc"]["tests"][0][k]=true;
    for(const char *k:{"finalized","onConfirmed","offConfirmed"})filter["qc"]["outputs"][0][k]=true;
    filter["qc"]["inputs"][0]["cycles"]=true;
    JsonDocument legacy;auto parse=deserializeJson(legacy,payload,DeserializationOption::Filter(filter));payload=String();filter.clear();beginQcProgress();
    if(parse){legacy.clear();String message=String("Legacy JSON parse: ")+parse.c_str();error(message.c_str(),parse==DeserializationError::NoMemory?507:400);return;}
    stopSafe();String why;bool ok=qcImportLegacy(legacy,why);JsonDocument d;d["ok"]=ok;d["detail"]=why;send(d,ok?200:409);
  });
  server.on("/api/qc/profile", HTTP_POST, [] {
    if (!guard()) return;
    if (rtcRunActive()) {error("Abort/finish RTC portal run before profile changes");return;}
    if (pdNegotiating()) { error("Wait for PD negotiation before profile changes"); return; }
    long revision;
    if (!number("revision", 0, 2147483647, revision) || !web->hasArg("enabled")) {
      error("Revision/test list required",400); return;
    }
    String id=web->arg("id"), detail;
    bool active=qcProfileSelected() && id==qcVariantName();
    stopSafe();
    bool ok=saveQcProfile(id,web->arg("enabled"),revision,detail);
    if(ok && active) {
      if(!selectQcProfile(id)) { error("Saved profile cannot be selected"); return; }
      reset(); resetCurrentPoints();
      if(qcTestEnabled("AHT10")) configureAht10();
    }
    JsonDocument d; d["ok"]=ok; d["detail"]=detail; send(d,ok?200:409);
  });
  server.on("/api/qc/recipe",HTTP_POST,[] {
    if(!guard())return;
    if(web->arg("confirm")!="1"){error("Konfirmasi preset diperlukan",400);return;}
    if(rtcRunActive()){error("Abort/finish RTC run before preset changes");return;}
    long revision;
    if(!number("revision",0,2147483647,revision)||pdNegotiating()||currentQcBusy()){error("Revisi/pending test preset tidak valid");return;}
    JsonDocument c;calibrationState(c.to<JsonObject>());
    if(c["pending"].as<int>()>=0){error("Tunggu capture selesai");return;}
    String id=web->arg("id"),why;bool active=qcProfileSelected()&&id==qcVariantName();
    stopSafe();bool ok=saveQcRecipe(id,revision,web->arg("settings"),why);
    if(ok&&active){reset();resetCurrentPoints();}
    JsonDocument out;out["ok"]=ok;out["detail"]=why;send(out,ok?200:409);
  });
  server.on("/api/qc/new", HTTP_POST, [] {
    if (!guard())
      return;
    if(rtcRunActive()){error("Abort/finish RTC run before starting a new QC session");return;}
    String name = web->arg("operator");
    name.trim();
    if (name.length() > 64) {
      error("Operator max64", 400);
      return;
    }
    if (pdNegotiating()) {
      error("Wait for PD negotiation before changing/resetting device");
      return;
    }
    if (!qcRecipesReady()) { error("Preset storage unavailable; periksa konfigurasi sebelum QC"); return; }
    if (!selectQcProfile(web->arg("variant"))) {
      error("Unknown profile or profile storage unavailable", 400); return;
    }
    stopSafe();
    reset();
    operatorName = name;
    resetCurrentPoints();
    if(qcTestEnabled("AHT10")) configureAht10();
    JsonDocument d;
    d["ok"] = true;
    send(d);
  });
  server.on("/api/qc/sequence/observe",HTTP_POST,[] {
    if(!guard())return;
    String kind=web->arg("kind"),expected=web->arg("state"),verdict=web->arg("verdict");long index;
    if((kind!="led"&&kind!="relay")||!number("index",0,kind=="led"?10:1,index)||(expected!="on"&&expected!="off")||(verdict!="PASS"&&verdict!="FAIL")) {error("Invalid sequence observation",400);return;}
    int n=kind=="led"?index:11+index;
    if(web->arg("sessionId")!=sessionId||!qcTestEnabled(outputName(n))||finalized[n]||!attempted[n]||live[n]!=(expected=="on")) {error("State/profile/session changed or QC final; observe only current attempted state");return;}
    if(verdict=="FAIL"){finalized[n]=true;stopSafe();qcRecord(outputName(n),"FAIL","Guided sequence: user observes mismatch at "+expected);}
    else {if(expected=="on")onConfirmed[n]=true;else offConfirmed[n]=true;
      finalized[n]=onConfirmed[n]&&offConfirmed[n];
      qcRecord(outputName(n),finalized[n]?"PASS":"INCOMPLETE",finalized[n]?"Guided sequence: user observed both ON and OFF sesuai":"Guided sequence: one observation recorded; ON and OFF both required");}
    JsonDocument d;d["ok"]=true;d["detail"]="Observasi urutan dicatat; bukan feedback beban otomatis";send(d);
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
      qcRetest("I2C scan");
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
      qcRetest("OLED");
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
        error("QC result already final; reset this module before repeating QC");
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
    qcRetest("EEPROM");
    qcRecord("EEPROM", ok ? "PASS" : identityBusReadable() ? "DETECTED" : "FAIL", detail);
    JsonDocument d;
    d["ok"] = ok;
    d["detail"] = detail;
    hardwareState(d["hardware"].to<JsonObject>());
    // An empty identity is a successful transport read, not a failed API call.
    d["ok"] = identityBusReadable();
    d["identityValid"] = ok;
    send(d);
  });
  server.on("/api/identity", HTTP_POST, [] {
    if (!guard())
      return;
    stopSafe();
    String detail;
    bool ok = saveIdentity(web->arg("top"), web->arg("bottom"), detail);
    qcRetest("EEPROM");
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
    if(ok){qcRetest("ADS / 4-20mA");qcRecord("ADS / 4-20mA","BELUM DIUJI","Calibration changed; repeat five-point QC");}
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
    qcRetest("AHT10");
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
    qcRetest("Ethernet");
    bool ok = testEthernet(detail);
    qcRecord("Ethernet", ok ? "PASS" : detail.indexOf("UNKNOWN") >= 0 ? "UNVERIFIED" : "FAIL", detail);
    JsonDocument d;
    d["ok"] = ok;
    d["detail"] = detail;
    send(d);
  });
  server.on("/api/qc/reset", HTTP_POST, [] {
    if(!guard())return;
    if(web->arg("confirm")!="1"){error("Explicit reset confirmation required",400);return;}
    String target=web->arg("target");bool found=false;
    for(size_t i=0;i<count;++i)if(items[i].name==target)found=true;
    if(!found){error("Unknown QC module",400);return;}
    if(!qcProfileSelected()||!qcTestEnabled(target)||web->arg("sessionId")!=sessionId||web->arg("profileRevision")!=String(activeQcProfileRevision())){error("Module disabled or session/profile changed");return;}
    JsonDocument cal;calibrationState(cal.to<JsonObject>());
    if(pdNegotiating()||currentQcBusy()||cal["pending"].as<int>()>=0||serialQcActive()||radioQcActive()||modbusQcActive()||networkQcBusy()){error("Finish active test/capture before module reset");return;}
    JsonDocument current;qcState(current.to<JsonObject>());
    if(web->arg("expectedQcCode").isEmpty()||web->arg("expectedQcCode")!=current["qcCode"].as<String>()){error("QC evidence changed; review reset again");return;}
    if(target=="RTC"){
      if(web->arg("clockRevision")!=String(rtcClockRevision())){error("RTC revision changed");return;}
      String why;if(!resetRtcEvidence(why)){error(why.c_str());return;}
    }
    stopSafe();
    if(target=="ADS / 4-20mA"){resetCurrentQc();retainedEvidence.remove("analog");}
    if(qcResetAffects(target.c_str(),"RS485")){resetModbusQc();resetSerialQcItem("RS485");retainedEvidence.remove("modbus");}
    if(target=="RS232")resetSerialQcItem(target);
    if((target=="RS232"||qcResetAffects(target.c_str(),"RS485"))&&qcResetAffects(target.c_str(),(retainedEvidence["serial"]["mode"]|"")))retainedEvidence.remove("serial");
    if(qcResetAffects(target.c_str(),"Radio communication")){resetRadioQc();retainedEvidence.remove("radio");}
    if(target=="WiFi network"||target=="Ethernet network"||target=="MQTT")resetNetworkQcItem(target);
    if(target=="I2C scan"){i2cScanned=false;i2cDevices=0;}
    if(target=="STUSB4500")resetPdQcEvidence();
    for(int i=0;i<7;++i)if(target=="Input:"+String(inputName(i)))cycles[i]={};
    for(int i=0;i<13;++i)if(target==outputName(i))live[i]=attempted[i]=onConfirmed[i]=offConfirmed[i]=finalized[i]=false;
    uint32_t previous=lastReset["sequence"]|0u;lastReset.clear();lastReset["sequence"]=previous+1;lastReset["target"]=target;lastReset["uptimeMs"]=millis();auto affected=lastReset["affected"].to<JsonArray>();
    for(size_t i=0;i<count;++i)if(qcResetAffects(target.c_str(),items[i].name.c_str())){
      affected.add(items[i].name);retained[i]=false;
      qcRecord(items[i].name,items[i].name.startsWith("Input:")?"MENUNGGU SIGNAL":"BELUM DIUJI","QC modul direset oleh operator; ulangi tes dengan bukti baru");
    }
    qcProgressDirty();String why;bool ok=qcSaveProgress(why);JsonDocument d;d["ok"]=ok;d["resetApplied"]=true;d["affected"].set(affected);d["detail"]=String("Reset modul diterapkan; hasil lain tetap. ")+why;send(d,ok?200:507);
  });
  server.on("/api/qc/pcb", HTTP_POST, [] {
    if(!guard())return;
    if(web->arg("confirm")!="1"){error("Explicit PCB metadata confirmation required",400);return;}
    if(!web->hasArg("pcbNumber")){error("pcbNumber field required; empty value clears it explicitly",400);return;}
    if(web->arg("sessionId")!=sessionId||web->arg("profileRevision")!=String(activeQcProfileRevision())||!web->hasArg("expectedPcbNumber")||web->arg("expectedPcbNumber")!=pcbNumber){error("PCB/session/profile changed; reload and review");return;}
    String value=web->arg("pcbNumber");value.trim();
    if(!qcPcbNumberValid(std::string_view(value.c_str(),value.length()))){error("PCB number max64 bytes, no control characters",400);return;}
    stopSafe();pcbNumber=value;qcProgressDirty();String why;bool ok=qcSaveProgress(why);JsonDocument d;d["ok"]=ok;d["pcbNumber"]=pcbNumber;d["detail"]=why;send(d,ok?200:507);
  });
  server.on("/api/qc/report", HTTP_POST, [] {
    if (!guard()) return;
    String pic=web->arg("operator"), date=web->arg("qcDate"), production=web->arg("productionDate");
    pic.trim();
    if (pic.isEmpty() || pic.length()>64 || date.isEmpty() || !qcIsoDate(date.c_str()) || !qcIsoDate(production.c_str())) {
      error("Operator wajib(max64), tanggal QC YYYY-MM-DD; production date opsional",400); return;
    }
    if(web->arg("sessionId")!=sessionId || web->arg("revision")!=String(activeQcProfileRevision())) {
      error("Sesi/profil berubah; tinjau laporan ulang"); return;
    }
    if(web->hasArg("pcbNumber")&&web->arg("pcbNumber")!=pcbNumber){error("PCB changed; save and review report again");return;}
    JsonDocument calibration;
    calibrationState(calibration.to<JsonObject>());
    if(pdNegotiating() || currentQcBusy() || calibration["pending"].as<int>()>=0 || serialQcActive() || radioQcActive() || modbusQcActive() || networkQcBusy()) {
      error("Tunggu tes/capture selesai sebelum menyimpan laporan"); return;
    }
    stopSafe();
    operatorName=pic;
    JsonDocument snapshot;
    snapshot["board"]="TMC TOP V2_R1 / BOTTOM V3_R0";
    snapshot["version"]=FW_VERSION; snapshot["buildUTC"]=FW_BUILD_UTC; snapshot["buildIdentity"]=FW_BUILD_ID;
    hardwareState(snapshot["hardware"].to<JsonObject>());
    qcState(snapshot["qc"].to<JsonObject>());
    if(web->arg("qcCode").isEmpty() || web->arg("qcCode") != snapshot["qc"]["qcCode"].as<String>()) {
      error("Hasil QC berubah; tinjau snapshot ulang"); return;
    }
    snapshot["outputStopEvidence"]="OFF command sent before snapshot; physical load feedback not verified";
    snapshot["reportSchema"]="QC-REPORT-1";
    snapshot["qcDate"]=date; snapshot["productionDate"]=production; snapshot["capturedUptimeMs"]=millis();
    snapshot["snapshotOnly"]=true;
    JsonDocument response;
    response["ok"]=true;
    String checkpointWhy;response["checkpointSaved"]=qcSaveProgress(checkpointWhy);response["checkpointDetail"]=checkpointWhy;
    response["snapshot"].set(snapshot.as<JsonObject>());
    response["csv"]=qcSnapshotCsv(snapshot,date,pic,production);
    response["detail"]="Snapshot QC diambil; simpan arsip laptop. Sesi tetap tersedia, hasil berikutnya tidak mengubah snapshot.";
    send(response);
  });
  server.on("/api/pd", HTTP_POST, [] {
    if (!guard())
      return;
    if (!qcPdEnabled()) {
      error("PD test OFF in selected QC profile");
      return;
    }
    if (web->arg("confirm") != "1") {
      error("Explicit PD voltage confirmation required", 400);
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
    qcRetest("STUSB4500");
    bool ok = requestPd(mv, ma, detail);
    qcRecord("STUSB4500", ok ? "NEGOTIATING" : "FAIL", detail);
    JsonDocument d;
    d["detail"] = detail;
    send(d, ok ? 200 : 409);
  });
  server.on("/api/serial/route", HTTP_POST, [] {
    if (!guard()) return;
    String next = web->arg("route"); if (next != "RS232" && next != "RS485") { error("Choose RS232/RS485", 400); return; }
    stopSafe(); if (!mcpBit(MCP2_ADDR, 2, next == "RS232")) { error("MCP2A2 route readback failed"); return; }
    route=next; JsonDocument d; d["detail"]="Route selected; this is not communication PASS"; send(d);
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


String qcSessionId(){return sessionId;}
void qcRetest(const String &name){for(size_t i=0;i<count;++i)if(items[i].name==name){retained[i]=false;break;}}
void qcCaptureProgress(JsonObject out){
 JsonDocument h;hardwareState(h.to<JsonObject>());
 out["sessionId"]=sessionId;out["operator"]=operatorName;out["pcbNumber"]=pcbNumber;out["savedUptimeMs"]=millis();out["lastReset"].set(lastReset.as<JsonObject>());
 auto a=out["tests"].to<JsonArray>();for(size_t i=0;i<count;++i){auto p=a.add<JsonObject>();p["name"]=items[i].name;p["status"]=items[i].status;p["detail"]=items[i].detail;}
 auto c=out["cycles"].to<JsonArray>();for(auto &p:cycles)c.add(p.cycles);
 auto o=out["outputs"].to<JsonArray>();for(int i=0;i<13;++i){auto p=o.add<JsonObject>();p["finalized"]=finalized[i];p["onConfirmed"]=onConfirmed[i];p["offConfirmed"]=offConfirmed[i];}
 auto kept=[&](const String& name){for(size_t i=0;i<count;++i)if(items[i].name==name)return retained[i];return false;};
 if(retainedEvidence.size())out["evidence"].set(retainedEvidence.as<JsonObject>());
 if(!kept("ADS / 4-20mA"))out["evidence"]["analog"].set(h["currentQc"]);
 JsonDocument q;qcState(q.to<JsonObject>());
 out["evidence"]["modbus"].set(q["modbus"]);
 for(int i=0;i<2;++i)if(kept("Modbus tag:"+String(i+1))&&!retainedEvidence["modbus"]["tags"][i].isNull())out["evidence"]["modbus"]["tags"][i].set(retainedEvidence["modbus"]["tags"][i]);
 if(!kept("WiFi network")&&!kept("Ethernet network")&&!kept("MQTT"))out["evidence"]["network"].set(q["network"]);
 for(const char *name:{"WiFi network","Ethernet network","MQTT"})if(!kept(name)){const char *key=String(name)=="WiFi network"?"wifiStatus":String(name)=="Ethernet network"?"ethernetStatus":"mqttStatus";out["evidence"]["network"][key].set(q["network"][key]);}
 if(!kept("RS485")&&!kept("RS232"))out["evidence"]["serial"].set(q["serial"]);
 if(!kept("Radio configuration")&&!kept("Radio communication"))out["evidence"]["radio"].set(q["radio"]);
 out["evidence"]["capturedBuildIdentity"]=FW_BUILD_ID;out["evidence"]["capturedSessionId"]=sessionId;
}
bool qcValidateProgress(JsonObjectConst d,String &why){
 if(!d["pcbNumber"].isNull()&&(!d["pcbNumber"].is<const char*>()||!qcPcbNumberValid(d["pcbNumber"].as<const char*>()))){why="Invalid PCB metadata";return false;}
 auto a=d["tests"].as<JsonArrayConst>();String sid=d["sessionId"].as<String>();
 if(sid.length()!=8||a.size()!=count||d["cycles"].size()!=7||d["outputs"].size()!=13){why="Invalid checkpoint shape";return false;}
 for(char c:sid)if(!isxdigit((unsigned char)c)){why="Invalid checkpoint session";return false;}
 for(size_t i=0;i<count;++i)if(a[i]["name"].as<String>()!=items[i].name||a[i]["detail"].as<String>().length()>3000||a[i]["status"].as<String>().length()>40){why="Checkpoint test schema/name differs";return false;}
 return true;
}
void qcRestoreProgress(JsonObjectConst d){
 reset();sessionId=d["sessionId"].as<String>();operatorName=d["operator"].as<String>();pcbNumber=d["pcbNumber"]|"";lastReset.set(d["lastReset"]);
 for(size_t i=0;i<count;++i){String status=d["tests"][i]["status"].as<String>();if((status=="PASS"||status=="FAIL")&&items[i].name!="RTC"){items[i].status=status;items[i].detail=d["tests"][i]["detail"].as<String>();retained[i]=true;}}
 for(int i=0;i<7;++i){cycles[i]={};cycles[i].cycles=std::min(2u,d["cycles"][i].as<unsigned>());}
 for(int i=0;i<13;++i){int index=0;while(index<int(count)&&items[index].name!=outputName(i))++index;finalized[i]=index<int(count)&&retained[index]&&d["outputs"][i]["finalized"].as<bool>();onConfirmed[i]=finalized[i]&&d["outputs"][i]["onConfirmed"].as<bool>();offConfirmed[i]=finalized[i]&&d["outputs"][i]["offConfirmed"].as<bool>();live[i]=attempted[i]=false;}
 retainedEvidence.set(d["evidence"]);qcProgressDirty();
}
