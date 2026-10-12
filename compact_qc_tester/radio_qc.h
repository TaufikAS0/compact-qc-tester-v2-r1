#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>
void beginRadioQc(WebServer &server, bool (*authorize)(), void (*stop)());
void updateRadioQc();
void radioQcState(JsonObject out);
bool radioQcActive();
void cancelRadioQc(const char *reason);
void resetRadioQc();
