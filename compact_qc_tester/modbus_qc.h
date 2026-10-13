#pragma once
#include <WebServer.h>
#include <ArduinoJson.h>
void beginModbusQc(WebServer &web, bool (*authorize)(), void (*stop)());
void updateModbusQc();
void modbusQcState(JsonObject out);
void resetModbusQc();
bool modbusQcActive();
void cancelModbusQc();
