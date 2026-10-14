#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
void beginQcProfiles();
bool selectQcProfile(const String &id);
bool qcTestEnabled(const String &name);
bool qcProfileIsSelected();
const char *activeQcProfileName();
uint32_t activeQcProfileRevision();
String activeQcProfileMask();
void qcProfilesState(JsonObject out);
bool saveQcProfile(const String &id, const String &enabledIds, uint32_t expectedRevision, String &detail);
