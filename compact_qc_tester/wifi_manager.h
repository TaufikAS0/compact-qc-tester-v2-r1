#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>

void beginLabWiFi(const char *apSsid);
void updateLabWiFi();
void labWiFiState(JsonObject d);
void beginWiFiSettings(WebServer &web, bool (*authorize)(), bool (*idle)());
