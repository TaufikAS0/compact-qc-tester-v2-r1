#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>
void beginRtcQc(WebServer &web,bool (*authorize)(),void (*stop)());
void rtcQcState(JsonObject out);
void updateRtcQc();
void resetRtcQc();
bool resetRtcEvidence(String &why);
bool rtcRunActive();
uint32_t rtcClockRevision();
String rtcEvidenceIdentity();
void rtcExportEvidence(JsonObject out);
bool rtcRestoreEvidence(JsonObjectConst saved);
