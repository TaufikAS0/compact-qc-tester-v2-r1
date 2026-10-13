#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>
void beginCurrentQc(WebServer &web, bool (*authorize)());
void updateCurrentQc();
void resetCurrentQc();
void currentQcState(JsonObject out);

bool currentQcBusy();
