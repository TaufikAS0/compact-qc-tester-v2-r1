#pragma once
#include <WebServer.h>
void beginQcExport(WebServer &server);

#include <ArduinoJson.h>
String qcSnapshotCsv(JsonDocument &snapshot, String date, String pic, String production);
String qcWarrantyCsv(JsonDocument &snapshot,const String &date,const String &pic,const String &production);
