#include "qc_export.h"
#include "qc_session.h"
#include "qc_hardware.h"
#include "qc_report_math.h"
#include "config.h"
#include <vector>
namespace {
// RFC4180 quoting plus Excel formula-injection protection for user metadata.
String csvCell(String value) {
  std::vector<char> buffer(value.length() * 2 + 5);
  qcCsvQuote(std::string_view(value.c_str(), value.length()), buffer.data(), buffer.size());
  return String(buffer.data());
}
} // namespace
void beginQcExport(WebServer &server) {
  server.on("/export/qc.csv", HTTP_GET, [&server] {
    server.sendHeader("Cache-Control", "no-store");
    if (!qcProfileSelected()) {
      server.send(409, "application/json", "{\"error\":\"Pilih profil device dahulu\"}");
      return;
    }
    String date = server.arg("qcDate"), pic = server.arg("qcPic"),
           production = server.arg("productionDate");
    if (!qcIsoDate(date.c_str()) || !qcIsoDate(production.c_str()) || pic.length() > 64) {
      server.send(400, "application/json",
                  "{\"error\":\"Tanggal YYYY-MM-DD dan PIC maksimal 64 karakter\"}");
      return;
    }
    JsonDocument snapshot;
    hardwareState(snapshot["hardware"].to<JsonObject>());
    qcState(snapshot["qc"].to<JsonObject>());
    JsonObject h = snapshot["hardware"], q = snapshot["qc"];
    if (pic.isEmpty())
      pic = q["operator"].as<String>();
    String header, row;
    header.reserve(2500);
    row.reserve(8000);
    auto add = [&](const String &name, const String &value) {
      if (header.length()) {
        header += ',';
        row += ',';
      }
      header += csvCell(name);
      row += csvCell(value);
    };
    // First ten columns retain the Longhi QC/inventory import order.
    add("serialNumber", unitSerial());
    add("productName", "Compact QC Tester");
    add("productType", q["variant"].as<String>());
    add("productionDate", production);
    add("qcStatus", q["overall"].as<String>());
    add("qcDate", date);
    add("qcPic", pic);
    add("status", "");
    add("deliveredAt", "");
    add("deliveredTo", "");
    add("qcCode", q["qcCode"].as<String>());
    add("qcCodeSchema", "QC1");
    add("sessionId", q["sessionId"].as<String>());
    add("profileRevision",q["profileRevision"].as<String>());
    add("profileMask",q["profileMask"].as<String>());
    add("firmwareVersion", FW_VERSION);
    add("buildUTC", FW_BUILD_UTC);
    add("hwTop", h["hwTop"].as<String>());
    add("hwBottom", h["hwBottom"].as<String>());
    add("identityValid", h["identityValid"].as<bool>() ? "true" : "false");
    add("calibrated", h["currentCalibrated"].as<bool>() ? "true" : "false");
    add("adcRaw", h["adcValid"].as<bool>() ? h["adcRaw"].as<String>() : "");
    add("currentMA", h["adcValid"].as<bool>() ? h["currentMA"].as<String>() : "");
    for (const char *key :
         {"applicableCount", "passCount", "failCount", "pendingCount", "notApplicableCount"})
      add(key, q[key].as<String>());
    for (JsonObject test : q["tests"].as<JsonArray>()) {
      String name = test["name"].as<String>();
      add(name + ".status", test["status"].as<String>());
      add(name + ".detail", test["detail"].as<String>());
      add(name + ".applicable", test["applicable"].as<bool>() ? "true" : "false");
    }
    int pointIndex=0;
    for(JsonObject point:h["currentQc"]["points"].as<JsonArray>()) {
      String prefix="AnalogPoint"+String(++pointIndex)+".";
      for(const char *key:{"captured","status","targetMA","measuredMA","errorMA","raw","noise","toleranceMA","calibrationId","uptimeMs"})add(prefix+key,point[key].isNull()?"":point[key].as<String>());
    }
    int tagIndex=0;
    for(JsonObject tag:q["modbus"]["tags"].as<JsonArray>()) {
      String prefix="ModbusTag"+String(++tagIndex)+".";
      for(const char *key:{"name","baud","parity","slave","function","address","count","type","order","timeout","valid","status","value","requestId","uptimeMs"})add(prefix+key,tag[key].isNull()?"":tag[key].as<String>());
      String raw;serializeJson(tag["raw"],raw);add(prefix+"raw",raw);
    }
    for(const char *key:{"staSSID","staIP","staConnected","lastIP","mqttCode"})add(String("Network.")+key,q["network"][key].isNull()?"":q["network"][key].as<String>());
    server.sendHeader("Content-Disposition", "attachment; filename=Compact_QC_" +
                                                 String(qcVariantName()) + "_" + unitSerial() +
                                                 ".csv");
    server.send(200, "text/csv; charset=utf-8",
                String("\xef\xbb\xbf") + header + "\r\n" + row + "\r\n");
  });
}
