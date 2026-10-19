#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>
void beginI2cCriteria(WebServer &web,bool (*authorize)(),bool (*idle)());
void evaluateI2cScan(JsonArrayConst devices,JsonObject result);
uint32_t i2cCriteriaRevision();
