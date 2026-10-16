#pragma once
#include <ArduinoJson.h>

void beginLabWiFi(const char *apSsid);
void updateLabWiFi();
void labWiFiState(JsonObject d);
