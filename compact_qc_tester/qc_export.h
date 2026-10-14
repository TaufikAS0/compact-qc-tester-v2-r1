#pragma once
#include <WebServer.h>
void beginQcExport(WebServer &server);

#include <ArduinoJson.h>
String qcSnapshotCsv(JsonDocument &snapshot, String date, String pic, String production);
