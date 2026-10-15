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
void qcNoteI2cScan(unsigned devices);
bool qcProfileSelected();
bool qcPdEnabled();
bool qcTestEnabled(const String &name);
const char *qcVariantName();
String qcSessionId();
void qcCaptureProgress(JsonObject out);
bool qcValidateProgress(JsonObjectConst in,String &why);
void qcRestoreProgress(JsonObjectConst in);
bool qcSaveProgress(String &why);
bool qcResumeProgress(String &why,const String &upgradeFromBuild=String());
void beginQcProgress();
void updateQcProgress();
void qcProgressDirty();
void qcProgressNewSession();
void qcProgressState(JsonObject out);
void qcSavedProgress(JsonObject out);
void qcReleaseProgressCache();
void qcRetest(const String &name);
bool qcImportLegacy(JsonDocument &snapshot,String &why);
