#include "i2c_criteria.h"
#include "qc_blob.h"
#include "qc_session.h"
namespace {
JsonDocument criteria;
bool available=false;
bool valid(JsonArrayConst a) {
  if(a.size()<1 || a.size()>16)return false;
  bool seen[128]={};
  for(JsonVariantConst v:a) {
    if(!v.is<int>())return false; int n=v.as<int>();
    if(n<8||n>119||seen[n])return false; seen[n]=true;
  }
  return true;
}
}
uint32_t i2cCriteriaRevision(){return available?(criteria["revision"]|0u):0;}
void evaluateI2cScan(JsonArrayConst devices,JsonObject result) {
  auto expected=criteria["required"].as<JsonArrayConst>();
  if(!available||!valid(expected)) {
    result["status"]="UNVERIFIED";
    result["detail"]="Teknisi harus menyimpan daftar alamat I2C wajib; ACK saja tidak membuktikan fungsi sensor";
  } else {
    auto missing=result["missing"].to<JsonArray>();
    for(int n:expected) {
      bool found=false;
      for(JsonObjectConst p:devices)if(p["address"].as<int>()==n)found=true;
      if(!found)missing.add(n);
    }
    result["required"].set(expected);
    result["criteriaRevision"]=criteria["revision"];
    result["status"]=missing.size()?"FAIL":"PASS";
    String detail="Required I2C addresses ACK; revision="+String(criteria["revision"].as<uint32_t>());
    if(missing.size()){detail="Missing I2C addresses:";for(int n:missing)detail+=" 0x"+String(n,HEX);}
    result["detail"]=detail+"; criteria revision="+String(i2cCriteriaRevision())+"; address-presence test only";
  }
  qcRecord("I2C scan",result["status"].as<String>(),result["detail"].as<String>());
}
void beginI2cCriteria(WebServer &web,bool (*authorize)(),bool (*idle)()) {
  String why; available=qcLoadBlob("qc-i2c","criteria-v1",criteria,why)&&valid(criteria["required"]);
  web.on("/api/i2c/criteria",HTTP_GET,[&web] {
    JsonDocument d;d["configured"]=available;d["revision"]=criteria["revision"]|0;
    d["required"].set(criteria["required"]);
    String body;serializeJson(d,body);web.send(200,"application/json",body);
  });
  web.on("/api/i2c/criteria",HTTP_POST,[&web,authorize,idle] {
    if(!authorize())return;JsonDocument d,next;String why;
    if(!idle()||web.arg("confirm")!="1")d["error"]="Hentikan output/tes lalu tinjau kriteria";
    else if(web.arg("required").length()>100 || deserializeJson(next,web.arg("required")) || !valid(next.as<JsonArrayConst>()))d["error"]="Daftar harus1..16 alamat unik desimal8..119";
    else if(web.arg("revision")!=String(criteria["revision"]|0u))d["error"]="Kriteria berubah; muat ulang";
    else {
      JsonDocument candidate;candidate["required"].set(next.as<JsonArray>());
      candidate["revision"]=(criteria["revision"]|0u)+1u;
      if(!qcSaveBlob("qc-i2c","criteria-v1",candidate,why))d["error"]="Gagal menyimpan kriteria; muat ulang";
      else {criteria.set(candidate);available=true;qcRetest("I2C scan");d["ok"]=true;d["detail"]="Kriteria tersimpan; hanya hasil I2C perlu diuji ulang";}
    }
    String body;serializeJson(d,body);web.send(d["error"].isNull()?200:409,"application/json",body);
  });
}
