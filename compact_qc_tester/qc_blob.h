#pragma once
#include <Preferences.h>
#include <ArduinoJson.h>
#include "qc_report_math.h"
#include <vector>
// One NVS blob contains checksum+payload; NVS atomically commits a key update.
// Never erase namespaces/format partitions. A failed write leaves the prior checkpoint available.
inline bool qcLoadBlob(const char *ns,const char *key,JsonDocument &doc,String &why){
 Preferences p;if(!p.begin(ns,true)){why="Storage missing/unavailable";return false;}
 size_t n=p.getBytesLength(key);if(n<5||n>16004){p.end();why="No valid checkpoint / size invalid";return false;}
 std::vector<uint8_t> b(n);bool ok=p.getBytes(key,b.data(),n)==n;p.end();uint32_t crc=0;memcpy(&crc,b.data(),4);
  if(!ok||crc!=qcCrc(std::string_view((char*)b.data()+4,n-4))||deserializeMsgPack(doc,b.data()+4,n-4)){why="Storage CRC/schema parse failed";return false;}
  if(doc["_qcCodec"]==1){
    JsonDocument rows;rows.set(doc["tests"]);auto dictionary=doc["_texts"].as<JsonArray>();auto out=doc["tests"].to<JsonArray>();
    if(rows.size()>80||dictionary.size()>160){why="Invalid progress codec";return false;}
    for(JsonArray row:rows.as<JsonArray>()){
      if(row.size()!=3||!row[1].is<unsigned>()||!row[2].is<unsigned>()||row[1].as<unsigned>()>=dictionary.size()||row[2].as<unsigned>()>=dictionary.size()){why="Invalid progress dictionary";return false;}
      auto p=out.add<JsonObject>();p["name"]=row[0];p["status"]=dictionary[row[1].as<unsigned>()];p["detail"]=dictionary[row[2].as<unsigned>()];
    }
    doc.remove("_qcCodec");doc.remove("_texts");
  }
 why="NVS checkpoint verified";return true;
}
inline bool qcSaveBlob(const char *ns,const char *key,JsonDocument &doc,String &why){
 String payload;serializeJson(doc,payload);if(payload.length()>16000){why="Checkpoint exceeds16KB; export report to laptop";return false;}
  JsonDocument packed;packed.set(doc);
  if(doc["tests"].is<JsonArray>()){
    auto dictionary=packed["_texts"].to<JsonArray>();auto rows=packed["tests"].to<JsonArray>();packed["_qcCodec"]=1;
    auto index=[&](String text){unsigned i=0;for(JsonVariant s:dictionary){if(s.as<String>()==text)return i;++i;}dictionary.add(text);return i;};
    for(JsonObject t:doc["tests"].as<JsonArray>()){auto row=rows.add<JsonArray>();row.add(t["name"]);row.add(index(t["status"].as<String>()));row.add(index(t["detail"].as<String>()));}
  }
  size_t size=measureMsgPack(packed);if(size>9000){why="Compact checkpoint exceeds9KB NVS budget; export to portal";return false;}
  std::vector<uint8_t>b(size+4);serializeMsgPack(packed,b.data()+4,size);uint32_t crc=qcCrc(std::string_view((char*)b.data()+4,size));memcpy(b.data(),&crc,4);
  packed.clear(); // Release the encoding tree before allocating the readback tree on ESP32.
 Preferences p;if(!p.begin(ns,false)){why="NVS unavailable; previous checkpoint retained";return false;}
  bool ok=p.putBytes(key,b.data(),b.size())==b.size();p.end();std::vector<uint8_t>().swap(b);JsonDocument check;
  if(!ok){why="NVS write failed; do not assume progress saved";return false;}
  if(!qcLoadBlob(ns,key,check,why)){why="NVS readback failed: "+why;return false;}
 String result;serializeJson(check,result);if(result!=payload){why="NVS readback differs";return false;}why="Progress saved and read back from NVS";return true;
}
