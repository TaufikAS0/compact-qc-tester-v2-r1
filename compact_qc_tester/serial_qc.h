#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>
void beginSerialQc(WebServer &server, bool (*authorize)(), void (*stopOutputs)());
void updateSerialQc();
void serialQcState(JsonObject out);
bool serialQcActive();
void cancelSerialQc(const char *reason);
void resetSerialQc();
void resetSerialQcItem(const String &name);
