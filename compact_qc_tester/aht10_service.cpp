#include "aht10_service.h"
#include "aht10_math.h"
#include "qc_session.h"
#include <Arduino.h>
#include <Wire.h>
namespace {
enum class Stage { Probe, ResetWait, InitWait, Idle, Measuring, Retry };
Stage stage=Stage::Probe;
uint32_t since=0,pollAt=0,sampleAt=0;
uint8_t statusByte=0,retries=0,lastWireError=0,lastReadBytes=0;
bool valid=false,initialized=false,statusValid=false;
float temperature=0,humidity=0;
String ahtDetail="Waiting AHT10 startup";
const char *stageName() {
  switch(stage) {
  case Stage::Probe:return "PROBE"; case Stage::ResetWait:return "RESET_WAIT";
  case Stage::InitWait:return "INIT_WAIT"; case Stage::Idle:return "IDLE";
  case Stage::Measuring:return "MEASURING"; default:return "RETRY";
  }
}
bool command(uint8_t cmd,bool args=false) {
  Wire.beginTransmission(0x38); Wire.write(cmd);
  if(args) { Wire.write(cmd==0xac?0x33:0x08); Wire.write(0); }
  lastWireError=Wire.endTransmission(); return lastWireError==0;
}
bool status() {
  lastReadBytes=Wire.requestFrom(uint8_t(0x38),uint8_t(1));
  statusValid=lastReadBytes==1;
  if(statusValid) statusByte=Wire.read();
  return statusValid;
}
void wait(Stage next,const String &detail) { stage=next; since=pollAt=millis(); ahtDetail=detail; }
void fail(const String &why) {
  valid=initialized=false;
  wait(Stage::Retry,why);
  qcRecord("AHT10","FAIL",ahtDetail);
}
void initialize() {
  if(!command(0xe1,true)) { fail("AHT10 E1 initialization NACK; Wire error="+String(lastWireError)+"; check model/power/I2C"); return; }
  wait(Stage::InitWait,"AHT10 E1 accepted; settling350ms");
}
}
bool startAht10() {
  valid=initialized=statusValid=false; retries=statusByte=lastWireError=lastReadBytes=0; sampleAt=0;
  wait(Stage::Probe,"AHT10 startup status probe queued; no unconditional reset");
  qcRecord("AHT10","TESTING",ahtDetail); return true;
}
void updateAht10() {
  if(!qcTestEnabled("AHT10")) return;
  uint32_t now=millis(),age=now-since;
  switch(stage) {
  case Stage::Probe:
    if(age<100) return;
    // Calibrated idle sensors need no E1/reset; a fresh measurement still must succeed.
    if(status() && Aht10::calibrated(statusByte)) {
      initialized=true; wait(Stage::Idle,"AHT10 calibrated status; first measurement pending"); return;
    }
    initialize(); return;
  case Stage::ResetWait:
    // Allow350ms after soft reset before E1, rather than only20ms.
    if(age<Aht10::COMMAND_SETTLE_MS) return;
    if(status() && Aht10::calibrated(statusByte)) {
      initialized=true; wait(Stage::Idle,"AHT10 calibrated after reset; measurement pending"); return;
    }
    initialize(); return;
  case Stage::InitWait:
    if(age<Aht10::COMMAND_SETTLE_MS || now-pollAt<20) return;
    pollAt=now;
    if(status() && Aht10::calibrated(statusByte)) {
      initialized=true; wait(Stage::Idle,"AHT10 initialized; first measurement pending"); return;
    }
    if(age>=700) fail(statusValid?"AHT10 init not calibrated/busy; status=0x"+String(statusByte,HEX):"AHT10 init status read missing");
    return;
  case Stage::Idle:
    if(sampleAt && now-sampleAt<2000) return;
    if(!command(0xac,true)) { fail("AHT10 measurement AC3300 trigger NACK; Wire error="+String(lastWireError)); return; }
    wait(Stage::Measuring,"AHT10 conversion pending"); return;
  case Stage::Measuring: {
    if(age<80 || now-pollAt<20) return;
    pollAt=now;
    // One complete status+measurement frame, without a preceding one-byte read.
    lastReadBytes=Wire.requestFrom(uint8_t(0x38),uint8_t(6));
    uint8_t b[6]={};
    for(unsigned i=0;i<lastReadBytes && i<6;++i) b[i]=Wire.read();
    statusValid=lastReadBytes>0;
    if(statusValid) statusByte=b[0];
    if(lastReadBytes!=6) {
      if(age>=Aht10::MEASURE_TIMEOUT_MS) fail("AHT10 short frame: "+String(lastReadBytes)+"/6 bytes"); return;
    }
    if(statusByte&0x80) {
      if(age>=Aht10::MEASURE_TIMEOUT_MS) fail("AHT10 busy timeout500ms"); return;
    }
    if(!Aht10::calibrated(statusByte)) { fail("AHT10 measurement status not calibrated; status=0x"+String(statusByte,HEX)); return; }
    humidity=Aht10::humidity(b); temperature=Aht10::temperature(b);
    valid=isfinite(temperature) && temperature>=-40 && temperature<=85 && humidity>=0 && humidity<=100;
    if(!valid) { fail("AHT10 decoded measurement out of range"); return; }
    initialized=true; sampleAt=now; retries=0;
    wait(Stage::Idle,"AHT10 fresh six-byte measurement valid; sensor accuracy requires reference comparison");
    qcRecord("AHT10","PASS",ahtDetail); return;
  }
  case Stage::Retry:
    if(age>=2000 && retries<3) {
      ++retries;
      if(!command(0xba)) { fail("AHT10 recovery reset NACK; Wire error="+String(lastWireError)); return; }
      wait(Stage::ResetWait,"AHT10 recovery reset accepted; settling350ms");
    }
    return;
  }
}
void aht10State(JsonObject d) {
  bool fresh=valid && millis()-sampleAt<5000;
  d["ahtVerified"]=initialized; d["ahtValid"]=fresh;
  d["ahtDetail"]=ahtDetail; d["ahtStage"]=stageName(); d["ahtStatusByte"]=statusByte;
  d["ahtStatusValid"]=statusValid; d["ahtWireError"]=lastWireError; d["ahtReadBytes"]=lastReadBytes;
  d["ahtRetries"]=retries;
  if(sampleAt) d["ahtSampleAgeMs"]=uint32_t(millis()-sampleAt);
  if(fresh) { d["temperatureC"]=temperature; d["humidityRH"]=humidity; }
}
