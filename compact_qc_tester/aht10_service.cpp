#include "aht10_service.h"
#include "qc_session.h"
#include <Arduino.h>
#include <Wire.h>
namespace {
enum class Stage { PowerWait, ResetWait, InitWait, Idle, Measuring, Retry };
Stage stage = Stage::PowerWait;
uint32_t since = 0, pollAt = 0, sampleAt = 0;
uint8_t statusByte = 0, retries = 0;
bool valid = false, initialized = false;
float temperature = 0, humidity = 0;
String ahtDetail = "Waiting AHT10 power settling";
bool command(uint8_t cmd, bool args = false) {
  Wire.beginTransmission(0x38); Wire.write(cmd);
  if (args) { Wire.write(cmd == 0xac ? 0x33 : 0x08); Wire.write(0); }
  return Wire.endTransmission() == 0;
}
bool status() {
  if (Wire.requestFrom(uint8_t(0x38), uint8_t(1)) != 1) return false;
  statusByte = Wire.read(); return true;
}
void fail(const String &why) {
  valid = initialized = false; ahtDetail = why;
  qcRecord("AHT10", "FAIL", ahtDetail); stage = Stage::Retry; since = millis();
}
void wait(Stage next, const char *why) {
  stage = next; since = millis(); pollAt = since; ahtDetail = why;
}
}
bool startAht10() {
  valid = initialized = false; retries = 0;
  // Asynchronous reset after >40ms power settling. ACK is not initialization proof.
  wait(Stage::PowerWait, "AHT10 power settle/reset queued");
  qcRecord("AHT10", "TESTING", ahtDetail); return true;
}
void updateAht10() {
  uint32_t now = millis(), age = now - since;
  switch (stage) {
  case Stage::PowerWait:
    if (age < 40) return;
    if (!command(0xba)) { fail("AHT10 reset NACK at0x38"); return; }
    wait(Stage::ResetWait, "AHT10 reset settling"); return;
  case Stage::ResetWait:
    if (age < 20) return;
    if (!command(0xe1, true)) { fail("AHT10 E1 initialization NACK"); return; }
    wait(Stage::InitWait, "AHT10 verifying calibrated status"); return;
  case Stage::InitWait:
    if (age < 10 || now - pollAt < 10) return;
    pollAt = now;
    if (!status()) { if (age >= 300) fail("AHT10 status read missing after init"); return; }
    if ((statusByte & 0x88) == 0x08) {
      initialized = true; retries = 0;
      wait(Stage::Idle, "AHT10 calibrated; first measurement pending"); return;
    }
    if (age >= 300) fail("AHT10 init status not calibrated/busy: " + String(statusByte, HEX));
    return;
  case Stage::Idle:
    if (sampleAt && now - sampleAt < 2000) return;
    if (!command(0xac, true)) { fail("AHT10 measurement trigger NACK"); return; }
    wait(Stage::Measuring, "AHT10 conversion pending"); return;
  case Stage::Measuring: {
    if (age < 80 || now - pollAt < 10) return;
    pollAt = now;
    // Keep waiting while busy, instead of treating the first busy read as FAIL.
    if (!status()) { if (age >= 500) fail("AHT10 conversion status read timeout"); return; }
    if (statusByte & 0x80) { if (age >= 500) fail("AHT10 busy timeout500ms"); return; }
    if (!(statusByte & 8)) { fail("AHT10 lost calibration-enable status"); return; }
    if (Wire.requestFrom(uint8_t(0x38), uint8_t(6)) != 6) {
      if (age >= 500) fail("AHT10 short measurement frame (expected6 bytes)"); return;
    }
    uint8_t b[6]; for (auto &v : b) v = Wire.read();
    if (b[0] & 0x80) return;
    if (!(b[0] & 8)) { fail("AHT10 data status not calibrated"); return; }
    humidity = ((uint32_t(b[1]) << 12) | (uint32_t(b[2]) << 4) | (b[3] >> 4)) * 100.0f / 1048576;
    temperature = (((uint32_t(b[3]) & 15) << 16) | (uint32_t(b[4]) << 8) | b[5]) * 200.0f / 1048576 - 50;
    valid = isfinite(temperature) && temperature >= -40 && temperature <= 85 && humidity >= 0 && humidity <= 100;
    if (!valid) { fail("AHT10 decoded measurement out of range"); return; }
    sampleAt = now;
    ahtDetail = "AHT10 calibrated status + six-byte measurement valid";
    qcRecord("AHT10", "PASS", ahtDetail + "; sensor accuracy requires reference comparison");
    wait(Stage::Idle, ahtDetail.c_str()); return;
  }
  case Stage::Retry:
    if (age >= 2000 && retries < 3) { ++retries; wait(Stage::PowerWait, "AHT10 bounded recovery retry"); }
    return;
  }
}
void aht10State(JsonObject d) {
  bool fresh = valid && millis() - sampleAt < 5000;
  d["ahtVerified"] = initialized; d["ahtValid"] = fresh;
  d["detail"] = ahtDetail; d["ahtStatusByte"] = statusByte;
  d["ahtRetries"] = retries;
  if (sampleAt) d["ahtSampleAgeMs"] = uint32_t(millis() - sampleAt);
  if (fresh) { d["temperatureC"] = temperature; d["humidityRH"] = humidity; }
}
