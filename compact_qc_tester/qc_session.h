#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>
struct QcItem {
  String name, status, detail;
};
void beginQcSession(WebServer &server, bool (*authorize)(), void (*stop)());
void updateQcSession();
void qcState(JsonObject out);
void qcOutputAttempt(const String &kind, int index, bool on);
const QcItem &qcDisplayItem();
const char *qcDisplayDetail();
void qcRecord(const String &name, const String &status, const String &detail);

void qcOutputsStopped();
bool qcProfileSelected();
bool qcPdEnabled();
const char *qcVariantName();
