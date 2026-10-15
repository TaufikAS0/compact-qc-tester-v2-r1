#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
void beginQcHardware();
void updateQcHardware();
void hardwareState(JsonObject out);
void scanBus(JsonArray out);
bool mcpBit(uint8_t address, uint8_t bit, bool level);
bool readInput(size_t index, int &value);
size_t inputCount();
const char *inputName(size_t index);
bool inputPin(size_t index, int &pin);
bool saveIdentity(const String &top, const String &bottom, String &detail);
const char *unitSerial();
bool testEthernet(String &detail);
bool configureAht10();
bool configureCurrent(bool verified, float shunt, float gain, float offset);
bool currentSample(float &ma, int16_t &raw, uint32_t &age);
bool requestPd(uint16_t mv, uint16_t ma, String &detail);

bool readIdentity(String &detail);
bool rawCurrentSample(int16_t &raw, uint32_t &sampleAt);
bool qcReadReg(uint8_t address, uint8_t reg, uint8_t *data, size_t n);
bool qcWriteReg(uint8_t address, uint8_t reg, const uint8_t *data, size_t n);
void rtcState(JsonObject out);
void updatePd();
void pdState(JsonObject out);

bool pdNegotiating();
void resetPdQcEvidence();

bool identityBusReadable();

bool mcpRestorePortBits(uint8_t address,uint8_t port,uint8_t mask,uint8_t oldLatch,uint8_t oldDir);
