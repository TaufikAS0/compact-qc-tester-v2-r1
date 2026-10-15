#include "qc_export.h"
#include "qc_session.h"
#include "qc_hardware.h"
#include "qc_report_math.h"
#include "config.h"
#include "ota_manager.h"
#include <vector>
namespace {
// RFC4180 quoting plus Excel formula-injection protection for user metadata.
String csvCell(String value) {
  std::vector<char> buffer(value.length() * 2 + 5);
  qcCsvQuote(std::string_view(value.c_str(), value.length()), buffer.data(), buffer.size());
  return String(buffer.data());
}
} // namespace
String qcSnapshotCsv(JsonDocument &snapshot, String date, String pic, String production) {
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
  add("recipeRevision",q["recipe"]["revision"].as<String>());
  add("recipeCRC",q["recipe"]["crc"].as<String>());
  add("rtcClockRevision",q["rtcEvidence"]["clockRevision"].as<String>());
  add("rtcTickPassed",q["rtcEvidence"]["tickPassed"].as<String>());
  for(const char *key:{"rtcEvidence","checkpointEvidence"}){String evidence;serializeJson(q[key],evidence);add(String(key)+".json",evidence);}
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
    add(name + ".restoredEvidence",test["restoredEvidence"].as<bool>()?"true":"false");
  }
  auto historical=[&](const String &name){for(JsonObject t:q["tests"].as<JsonArray>())if(t["name"]==name)return t["restoredEvidence"].as<bool>();return false;};
  int pointIndex=0;
  JsonArray analogPoints=historical("ADS / 4-20mA")?q["checkpointEvidence"]["analog"]["points"].as<JsonArray>():h["currentQc"]["points"].as<JsonArray>();
  for(JsonObject point:analogPoints) {
    String prefix="AnalogPoint"+String(++pointIndex)+".";
    for(const char *key:{"captured","status","targetMA","measuredMA","errorMA","raw","noise","toleranceMA","calibrationId","uptimeMs"})add(prefix+key,point[key].isNull()?"":point[key].as<String>());
  }
  int tagIndex=0;
  for(JsonObject tag:q["modbus"]["tags"].as<JsonArray>()) {
    if(historical("Modbus tag:"+String(tagIndex+1))&&!q["checkpointEvidence"]["modbus"]["tags"][tagIndex].isNull())tag=q["checkpointEvidence"]["modbus"]["tags"][tagIndex].as<JsonObject>();
    String prefix="ModbusTag"+String(++tagIndex)+".";
    for(const char *key:{"name","baud","parity","slave","function","address","count","type","order","timeout","valid","status","value","requestId","uptimeMs"})add(prefix+key,tag[key].isNull()?"":tag[key].as<String>());
    String raw;serializeJson(tag["raw"],raw);add(prefix+"raw",raw);
  }
  JsonObject network=(historical("WiFi network")||historical("Ethernet network")||historical("MQTT"))?q["checkpointEvidence"]["network"].as<JsonObject>():q["network"].as<JsonObject>();
  for(const char *key:{"staSSID","staIP","staConnected","lastIP","mqttCode"})add(String("Network.")+key,network[key].isNull()?"":network[key].as<String>());
  // Append only: preserve every existing column and the first-ten Longhi import order.
  add("pcbNumber",q["pcbNumber"]|"");
  return String("\xef\xbb\xbf") + header + "\r\n" + row + "\r\n";
}
String qcWarrantyCsv(JsonDocument &snapshot,const String &date,const String &pic,const String &production){
  String csv="serialNumber,productName,productType,productionDate,qcStatus,qcDate,qcPic,status,deliveredAt,deliveredTo\r\n";
  const String values[]={snapshot["hardware"]["serial"].as<String>(),"Telemetric Module Compact","Monitoring and Control",production,snapshot["qc"]["overall"].as<String>(),date,pic,"INVENTORY","",""};
  for(unsigned i=0;i<10;++i){if(i)csv+=',';csv+=csvCell(values[i]);}return csv+"\r\n";
}
void beginQcExport(WebServer &server) {
  server.on("/export/warranty.csv",HTTP_GET,[&server]{
    server.sendHeader("Cache-Control","no-store");
    if(!qcProfileSelected()){server.send(409,"application/json","{\"error\":\"Pilih profil device dahulu\"}");return;}
    String date=server.arg("qcDate"),production=server.arg("productionDate"),pic=server.arg("qcPic");pic.trim();
    if(pic.isEmpty()||pic.length()>64||!qcWarrantyDates(date.c_str(),production.c_str())){server.send(400,"application/json","{\"error\":\"Warranty perlu PIC, tanggal QC dan produksi valid; produksi tidak boleh setelah QC\"}");return;}
    JsonDocument snapshot;hardwareState(snapshot["hardware"].to<JsonObject>());qcState(snapshot["qc"].to<JsonObject>());auto q=snapshot["qc"].as<JsonObject>();
    if(!qcWarrantyStatus((q["overall"]|""))||webOtaBusy()||snapshot["hardware"]["pd"]["pending"]==true||snapshot["hardware"]["rtc"]["runningTest"]==true||(snapshot["hardware"]["rtc"]["externalRunId"]|"")[0]!='\0'||snapshot["hardware"]["calibration"]["pending"].as<int>()>=0||snapshot["hardware"]["currentQc"]["pending"].as<int>()>=0||q["serial"]["active"]==true||q["radio"]["active"]==true||q["network"]["active"]==true||q["modbus"]["active"]==true){server.send(409,"application/json","{\"error\":\"CSV Warranty perlu hasil PASS atau FAIL; tunggu tes/capture selesai\"}");return;}
    for(JsonObject output:q["outputs"].as<JsonArray>())if(output["commandedOn"]==true){server.send(409,"application/json","{\"error\":\"Hentikan output sebelum CSV Warranty\"}");return;}
    String serial=snapshot["hardware"]["serial"].as<String>();bool safe=!serial.isEmpty()&&serial.length()<=64;for(char c:serial)safe=safe&&(isalnum((unsigned char)c)||c=='_'||c=='-');
    if(!safe){server.send(409,"application/json","{\"error\":\"Serial tidak valid untuk nama file Warranty\"}");return;}
    server.sendHeader("Content-Disposition","attachment; filename=\""+serial+".csv\"");server.send(200,"text/csv; charset=utf-8",qcWarrantyCsv(snapshot,date,pic,production));
  });
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
    server.sendHeader("Content-Disposition", "attachment; filename=Compact_QC_" +
                                                 String(qcVariantName()) + "_" + unitSerial() +
                                                 ".csv");
    server.send(200, "text/csv; charset=utf-8",
                qcSnapshotCsv(snapshot, date, pic, production));
  });
}
