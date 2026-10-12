#include "tpl_watchdog.h"
#include "config.h"
#include <Wire.h>

namespace {
// MCP23017 BANK=0, use one-byte access so SEQOP does not matter.
constexpr uint8_t IODIRB = 0x01, IOCONA = 0x0a, IOCONB = 0x0b, OLATB = 0x15;
bool ready = false, high = false;
uint32_t edgeAt = 0, pulses = 0;
const char *error = "Not initialized";

bool readByte(uint8_t reg, uint8_t &value) {
  Wire.beginTransmission(MCP1_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0)
    return false;
  if (Wire.requestFrom(MCP1_ADDR, static_cast<uint8_t>(1)) != 1)
    return false;
  value = Wire.read();
  return true;
}

bool writeByte(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MCP1_ADDR);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool setDone(bool level) {
  uint8_t latch;
  if (!readByte(OLATB, latch))
    return false;
  const uint8_t next = level ? (latch | TPL_DONE_MASK) : (latch & ~TPL_DONE_MASK);
  return writeByte(OLATB, next);
}

void fail(const char *reason) {
  error = reason;
  ready = false;
  // Best effort LOW. Do not keep feeding a watchdog after an I2C failure.
  setDone(false);
  Serial.printf("TPL watchdog ERROR: %s\n", reason);
}
} // namespace

void beginTplWatchdog() {
  if (!TPL_MCP23017_CONFIRMED) {
    error = "MCP23017 identity not confirmed";
    Serial.printf("TPL watchdog LOCKED: %s\n", error);
    return;
  }
  uint8_t a, b, direction;
  if (!readByte(IOCONA, a) || !readByte(IOCONB, b)) {
    error = "MCP1 not responding";
    Serial.printf("TPL watchdog ERROR: %s\n", error);
    return;
  }
  // Requires known BANK=0; do not normalize registers on an unknown profile.
  if (a != b || (a & 0x81) != 0) {
    error = "MCP1 BANK=0 profile unavailable";
    Serial.printf("TPL watchdog ERROR: %s\n", error);
    return;
  }
  if (!readByte(IODIRB, direction) || !setDone(false) ||
      !writeByte(IODIRB, direction & ~TPL_DONE_MASK)) {
    fail("Cannot initialize DONE B7");
    return;
  }
  uint8_t verified;
  if (!readByte(IODIRB, verified) || (verified & TPL_DONE_MASK) != 0) {
    fail("DONE direction readback failed");
    return;
  }
  ready = true;
  error = "";
  edgeAt = millis() - TPL_DONE_INTERVAL_MS;
  updateTplWatchdog();
  Serial.println("TPL5010 DONE: MCP1B7; interval 25ms, HIGH >=1ms");
}

void updateTplWatchdog() {
  if (!ready)
    return;
  const uint32_t now = millis();
  if (high) {
    if (static_cast<uint32_t>(now - edgeAt) < TPL_DONE_HIGH_MS)
      return;
    if (!setDone(false)) {
      fail("DONE falling edge failed");
      return;
    }
    high = false;
    return;
  }
  if (static_cast<uint32_t>(now - edgeAt) < TPL_DONE_INTERVAL_MS)
    return;
  if (!setDone(true)) {
    fail("DONE rising edge failed");
    return;
  }
  high = true;
  edgeAt = millis();
  ++pulses;
}

bool tplWatchdogReady() { return ready; }
const char *tplWatchdogError() { return error; }
uint32_t tplWatchdogPulses() { return pulses; }
