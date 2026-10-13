#pragma once
#include <WebServer.h>
#include <ArduinoJson.h>
void beginNetworkQc(WebServer &web, bool (*authorize)(), void (*stop)());
void updateNetworkQc();
void networkQcState(JsonObject out);
bool networkQcBusy();
void cancelNetworkQc();
void resetNetworkQc();
