#include "rtc_qc.h"
#include "config.h"
#include "rtc_math.h"
#include "qc_blob.h"
#include "qc_hardware.h"
#include "qc_session.h"
#include "qc_profiles.h"
#include "current_qc.h"
#include "current_calibration.h"
#include "ota_manager.h"
#include <esp_timer.h>
namespace {
JsonDocument meta,external,quick;
WebServer *web;bool (*auth)();void (*stopSafe)();
bool metadataReady=false,ticking=false,tickPassed=false,tickFailed=false;int64_t tickAt=0,tickEpoch=0;
String rtcDetail="Belum diuji; komunikasi/waktu bukan akurasi atau retensi baterai",tickSession;
uint32_t tickRevision=0;String bootId;
struct Reading {bool transport=false,valid=false,osf=true;int64_t local=-1,at=0,completed=0;uint8_t bytes[19]={};};
Reading read(){Reading r;r.at=esp_timer_get_time();r.transport=qcReadReg(0x68,0,r.bytes,19);r.completed=esp_timer_get_time();if(r.transport){r.valid=RtcMath::decode(r.bytes,r.local);r.osf=(r.bytes[15]&128)!=0;}return r;}
void send(JsonDocument &d,int code=200){if(d["rtc"].is<JsonObject>()){int64_t now=esp_timer_get_time();d["rtc"]["snapshotResponseUptimeUs"]=now;d["rtc"]["sampleAgeUpperBoundMs"]=(now-d["rtc"]["sampleUptimeUs"].as<int64_t>()+999)/1000;}String s;serializeJson(d,s);web->sendHeader("Cache-Control","no-store");web->send(code,"application/json",s);}
void error(const String &s,int code=409){JsonDocument d;d["error"]=s;send(d,code);}
bool integer(const char *key,int64_t lo,int64_t hi,int64_t &n){String s=web->arg(key);if(s.isEmpty()||s.length()>16)return false;for(unsigned i=0;i<s.length();++i)if(!isDigit(s[i])&&!(i==0&&s[i]=='-'))return false;char *end;n=strtoll(s.c_str(),&end,10);return !*end&&n>=lo&&n<=hi;}
bool id(const String &s){if(s.isEmpty()||s.length()>64)return false;for(char c:s)if(!isalnum((unsigned char)c)&&c!='-'&&c!='_'&&c!='.')return false;return true;}
bool context(){
 if(!auth())return false;
 if(webOtaBusy()||!qcProfileSelected()||!qcTestEnabled("RTC")||web->arg("sessionId")!=qcSessionId()||web->arg("profileRevision")!=String(activeQcProfileRevision())||web->arg("clockRevision")!=String(rtcClockRevision())){error("RTC/session/profile/revision changed; reload before action");return false;}
 if(web->arg("confirm")!="1"){error("Explicit confirm=1 required",400);return false;}
 if(!metadataReady){error("RTC metadata unavailable/corrupt; no clock writes allowed");return false;}return true;
}
bool saveMeta(){String why;if(!qcSaveBlob("qc-rtc","meta-v1",meta,why)){metadataReady=false;rtcDetail=why;return false;}return true;}
void record(const Reading&r){
 bool extPass=external["accuracy"]["result"]=="PASS"&&external["retention"]["result"]=="PASS";
 bool extFail=external["accuracy"]["result"]=="FAIL"||external["retention"]["result"]=="FAIL";
 bool sameQuick=quick["sessionId"].as<String>()==qcSessionId()&&quick["clockRevision"].as<uint32_t>()==rtcClockRevision();
 bool quickPass=sameQuick&&quick["result"]=="PASS"&&meta["writeVerified"]==true;
 const char *status=!r.transport||!r.valid||tickFailed||extFail||(sameQuick&&quick["result"]=="FAIL")?"FAIL":r.osf?"UNVERIFIED":ticking?"RUNNING":quickPass||(tickPassed&&extPass)?"PASS":"UNVERIFIED";
 qcRecord("RTC",status,!r.transport?"DS3231 read failed":!r.valid?"Invalid RTC calendar":r.osf?"OSF active: set correct time/readback required; battery fault not proven":quickPass&&String(status)=="PASS"?"QC CEPAT Write/Readback PASS; sampling60s/harian/baterai terpisah, bukan sertifikasi akurasi":rtcDetail);
}
void sample(JsonObject d,const Reading&r){
 d["schema"]="COMPACT-RTC-1";d["bootId"]=bootId;d["sampleUptimeUs"]=r.at;d["sampleCompletedUptimeUs"]=r.completed;d["resolutionMs"]=1000;
 d["communicationValid"]=r.transport;d["valid"]=r.valid;d["oscillatorStopped"]=r.osf;d["timeValid"]=r.valid&&!r.osf;d["clockRevision"]=rtcClockRevision();d["timezoneConfigured"]=meta["configured"].as<bool>()&&meta["writeVerified"].as<bool>();
 d["utcOffsetMinutes"]=meta["offsetMinutes"];d["lastSetUtcSeconds"]=meta["lastSetUtcSeconds"];d["writeVerified"]=meta["writeVerified"];d["metadataReady"]=metadataReady;
 d["productionCriterion"]="WRITE_READBACK";d["quick"].set(quick.as<JsonObject>());d["samplingComplete"]=tickPassed&&external["accuracy"]["result"]=="PASS"&&external["retention"]["result"]=="PASS";
 if(r.transport)d["temperatureC"]=(int8_t)r.bytes[17]+(r.bytes[18]>>6)*.25;
 if(r.valid){uint8_t b[7];RtcMath::encode(r.local,b);char t[24];snprintf(t,sizeof(t),"%04d-%02d-%02d %02d:%02d:%02d",2000+RtcMath::dec(b[6]),RtcMath::dec(b[5]),RtcMath::dec(b[4]),RtcMath::dec(b[2]),RtcMath::dec(b[1]),RtcMath::dec(b[0]));d["time"]=t;d["rtcLocalEpochSeconds"]=r.local;
  if(metadataReady&&meta["configured"].as<bool>()&&meta["writeVerified"].as<bool>())d["utcSeconds"]=r.local-meta["offsetMinutes"].as<int>()*60LL;}
 d["runningTest"]=ticking;d["runningTestPassed"]=tickPassed;d["runningElapsedSeconds"]=ticking?(esp_timer_get_time()-tickAt)/1000000:0;d["detail"]=rtcDetail;d["externalRunId"]=meta["runId"];d["externalRunSessionId"]=meta["runSessionId"];d["externalEvidence"].set(external.as<JsonObject>());
}
}
uint32_t rtcClockRevision(){return meta["revision"]|0u;}
bool rtcRunActive(){const char *run=meta["runId"]|"";return run[0]!='\0';}
String rtcEvidenceIdentity(){String s,q;serializeJson(external,s);serializeJson(quick,q);return String(rtcClockRevision())+"|"+s+"|"+q;}
void rtcExportEvidence(JsonObject d){d["clockRevision"]=rtcClockRevision();d["tickPassed"]=tickPassed;d["tickFailed"]=tickFailed;d["external"].set(external.as<JsonObject>());d["quick"].set(quick.as<JsonObject>());d["productionCriterion"]="WRITE_READBACK";}
bool rtcRestoreEvidence(JsonObjectConst d){if(d["clockRevision"].as<uint32_t>()!=rtcClockRevision())return false;external.set(d["external"]);quick.set(d["quick"]);tickPassed=d["tickPassed"]|false;tickFailed=d["tickFailed"]|false;ticking=false;rtcDetail="Bukti RTC dipulihkan dari checkpoint; status hardware dibaca ulang";return true;}
void resetRtcQc(){ticking=tickPassed=tickFailed=false;external.clear();quick.clear();rtcDetail="Belum diuji; QC cepat Write/Readback dan sampling terpisah";}
void rtcQcState(JsonObject d){Reading r=read();record(r);sample(d,r);}
void updateRtcQc(){
 if(!ticking)return;
 int64_t elapsed=esp_timer_get_time()-tickAt;
 if(!qcProfileSelected()||qcSessionId()!=tickSession||rtcClockRevision()!=tickRevision){ticking=false;rtcDetail="Tes berjalan dibatalkan karena konteks berubah";return;}
 if(elapsed<60000000)return;
 Reading r=read();ticking=false;tickPassed=r.transport&&r.valid&&!r.osf&&RtcMath::tick(tickEpoch,r.local,elapsed/1000);
 tickFailed=!tickPassed;
 rtcDetail=tickPassed?"Jam maju sesuai60 detik (toleransi2s); menunggu bukti portal akurasi dan retensi baterai":"Tes jam berjalan gagal/terputus; ulangi setelah periksa waktu/OSF";record(r);
}
void beginRtcQc(WebServer &server,bool (*authorize)(),void (*stop)()){
 web=&server;auth=authorize;stopSafe=stop;bootId=String(esp_random(),HEX)+String(esp_random(),HEX);String why;
 Preferences p;bool existed=false;if(p.begin("qc-rtc",true)){existed=p.isKey("meta-v1");p.end();}
 if(existed){metadataReady=qcLoadBlob("qc-rtc","meta-v1",meta,why)&&meta["schema"]==1&&meta["revision"].is<uint32_t>();}
 else{meta["schema"]=1;meta["revision"]=0;meta["configured"]=false;meta["writeVerified"]=false;metadataReady=true;}
 if(!metadataReady)rtcDetail="RTC metadata invalid; explicit maintenance needed, no automatic erase";
 server.on("/api/rtc/sample",HTTP_GET,[]{if(!qcTestEnabled("RTC")){error("RTC disabled by profile");return;}JsonDocument d;d["serial"]=unitSerial();d["sessionId"]=qcSessionId();d["profile"]=qcVariantName();d["profileRevision"]=activeQcProfileRevision();d["version"]=FW_VERSION;d["buildUTC"]=FW_BUILD_UTC;sample(d["rtc"].to<JsonObject>(),read());send(d);});
 server.on("/api/rtc/reset",HTTP_POST,[]{
  if(!context())return;if(rtcRunActive()||ticking||rtcClockRevision()==UINT32_MAX){error("Finish/abort active RTC run before retest");return;}
  meta["revision"]=rtcClockRevision()+1;if(!saveMeta()){error(rtcDetail);return;}
  resetRtcQc();qcProgressDirty();String why;bool ok=qcSaveProgress(why);JsonDocument d;d["ok"]=ok;d["detail"]="Hanya bukti RTC direset; tes lain tetap. "+why;send(d,ok?200:507);
 });
 server.on("/api/rtc/time",HTTP_POST,[]{
  if(!context())return;if(ticking||rtcRunActive()||pdNegotiating()||currentQcBusy()){error("RTC run/capture active; finish or abort before setting clock");return;}
  if(web->hasArg("qcQuick")&&web->arg("qcQuick")!="1"){error("qcQuick must be1 when requesting quick QC",400);return;}bool quickTest=web->arg("qcQuick")=="1";
  JsonDocument cal;calibrationState(cal.to<JsonObject>());if(cal["pending"].as<int>()>=0){error("Wait for calibration capture");return;}
  int64_t utc,offset;if(!integer("utcSeconds",946684800LL,4102444799LL,utc)||!integer("utcOffsetMinutes",-720,840,offset)||web->arg("source").isEmpty()||web->arg("source").length()>64){error("UTC seconds2000..2099, offset -720..840 and source max64 required",400);return;}
  Reading before=read();uint8_t b[7];if(!before.transport||!RtcMath::encode(utc+offset*60,b)){error("RTC unreadable / local calendar outside2000..2099");return;}
  if(rtcClockRevision()==UINT32_MAX){error("Clock revision exhausted");return;}
  stopSafe();resetRtcQc();meta["revision"]=rtcClockRevision()+1;meta["configured"]=true;meta["offsetMinutes"]=offset;meta["lastSetUtcSeconds"]=utc;meta["source"]=web->arg("source");meta["writeVerified"]=false;if(!saveMeta()){error(rtcDetail);return;}
  bool challengeOk=true;int64_t challengeLocal=-1,challengeReadback=-1;
  if(quickTest){uint8_t probe[7];challengeLocal=RtcMath::writeChallenge(utc+offset*60,before.local);challengeOk=RtcMath::encode(challengeLocal,probe)&&qcWriteReg(0x68,0,probe,7);Reading check=read();challengeReadback=check.local;challengeOk=challengeOk&&check.transport&&check.valid&&RtcMath::readback(challengeLocal,check.local);}
  // Persist intent first. Any interruption leaves writeVerified=false; never infer time trust.
  // Always attempt restoration to the reviewed real time, even if the deliberate challenge failed.
  bool ok=qcWriteReg(0x68,0,b,7);uint8_t control=before.bytes[14]&0x7f;if(ok)ok=qcWriteReg(0x68,14,&control,1);
  Reading after=read();ok=ok&&after.transport&&after.valid&&RtcMath::readback(utc+offset*60,after.local);
  // Alarm flags are write-zero-to-clear: write ones to preserve flags that may have arrived meanwhile.
  if(ok){uint8_t status=(after.bytes[15]&0x7f)|3;ok=qcWriteReg(0x68,15,&status,1);after=read();ok=ok&&after.transport&&after.valid&&!after.osf&&RtcMath::readback(utc+offset*60,after.local);}
  bool restored=ok;ok=ok&&challengeOk;meta["writeVerified"]=restored;if(!saveMeta()){error(rtcDetail);return;}
  rtcDetail=ok?"Waktu ditulis dan dibaca balik; OSF dibersihkan. Belum bukti akurasi/baterai":"RTC write/readback failed; jangan percaya waktu, ulangi eksplisit";
  if(quickTest){quick["schema"]="QC-RTC-QUICK-1";quick["scope"]="WRITE_READBACK";quick["sessionId"]=qcSessionId();quick["clockRevision"]=rtcClockRevision();quick["result"]=ok?"PASS":"FAIL";quick["challengeUtcSeconds"]=challengeLocal-offset*60;quick["challengeReadbackUtcSeconds"]=challengeReadback<0?-1:challengeReadback-offset*60;quick["challengeVerified"]=challengeOk;quick["restorationVerified"]=restored;quick["requestedUtcSeconds"]=utc;quick["readbackUtcSeconds"]=after.valid?after.local-offset*60:-1;quick["utcOffsetMinutes"]=offset;quick["source"]=web->arg("source");quick["completedUptimeMs"]=millis();}
  record(after);qcProgressDirty();bool saved=true;String why;cal.clear();if(quickTest)saved=qcSaveProgress(why);
  JsonDocument d;d["ok"]=ok&&saved;d["writeReadbackVerified"]=ok;if(quickTest){d["checkpointSaved"]=saved;d["detail"]=why;}sample(d["rtc"].to<JsonObject>(),after);send(d,!saved?507:ok?200:409);
 });
 server.on("/api/rtc/tick",HTTP_POST,[]{if(!context())return;if(ticking||rtcRunActive()){error("RTC test already active");return;}Reading r=read();if(!r.transport||!r.valid||r.osf){error("RTC calendar/OSF invalid; set correct clock first");return;}ticking=true;tickPassed=tickFailed=false;tickAt=esp_timer_get_time();tickEpoch=r.local;tickRevision=rtcClockRevision();tickSession=qcSessionId();rtcDetail="Tes jam berjalan60 detik; bukan akurasi24jam atau retensi";JsonDocument d;d["ok"]=true;send(d);});
 server.on("/api/rtc/run",HTTP_POST,[]{
  if(!context())return;String run=web->arg("runId");if(!id(run)){error("runId ASCII identifier1..64 required",400);return;}
  String action=web->arg("action");
  if(action=="bind"){
   Reading r=read();if(rtcRunActive()||ticking||!r.valid||r.osf||!meta["writeVerified"].as<bool>()){error("RTC run active or UTC not established");return;}
   String why;if(!qcSaveProgress(why)){error("Cannot bind RTC run without durable QC checkpoint: "+why,507);return;}
   meta["runId"]=run;meta["runSessionId"]=qcSessionId();if(!saveMeta()){error(rtcDetail);return;}
  }else if(action=="abort"){
   if(meta["runId"].as<String>()!=run||meta["runSessionId"].as<String>()!=qcSessionId()){error("Run/session mismatch");return;}
   meta["runId"]="";meta["runSessionId"]="";if(!saveMeta()){error(rtcDetail);return;}
  }else{error("action bind/abort only; report contract separate",400);return;}
  JsonDocument d;d["ok"]=true;d["detail"]=action=="bind"?"RTC clock locked for portal run":"RTC run aborted; no PASS";send(d);
 });
 server.on("/api/rtc/evidence",HTTP_POST,[]{
  if(!context())return;JsonDocument in;
  if(web->arg("report").length()>2200||deserializeJson(in,web->arg("report"))){error("Bounded JSON report required",400);return;}
  String kind=in["kind"].as<String>(),run=in["runId"].as<String>();
  if(in["schema"]!="QC-RTC-EVIDENCE-1"||(kind!="accuracy"&&kind!="retention")||!id(run)||in["operator"].as<String>().isEmpty()||in["operator"].as<String>().length()>64||in["source"].as<String>().isEmpty()||in["source"].as<String>().length()>64){error("Evidence schema/kind/run/operator/source invalid",400);return;}
  if(!external[kind].isNull()){bool same=true;for(const char *k:{"schema","kind","runId","operator","source","sessionId","clockRevision","elapsedSeconds","driftSeconds","uncertaintySeconds","toleranceSeconds","referenceVerified","physicalPowerRemoved"})same=same&&(external[kind][k]==in[k]);if(same){String why;bool saved=qcSaveProgress(why);JsonDocument d;d["ok"]=saved;d["duplicate"]=true;d["detail"]=why;send(d,saved?200:507);}else error("Immutable RTC evidence exists; new QC session required for retest");return;}
  if(meta["runId"].as<String>()!=run||meta["runSessionId"].as<String>()!=qcSessionId()||in["sessionId"].as<String>()!=qcSessionId()||in["clockRevision"].as<uint32_t>()!=rtcClockRevision()){error("Evidence run/session/clock revision mismatch");return;}
  for(const char *k:{"elapsedSeconds","driftSeconds","uncertaintySeconds","toleranceSeconds"})if(!in[k].is<double>()||!isfinite(in[k].as<double>())){error("Finite elapsed/drift/uncertainty/tolerance required",400);return;}
  double elapsed=in["elapsedSeconds"],drift=in["driftSeconds"],uncertainty=in["uncertaintySeconds"],tolerance=in["toleranceSeconds"];
  if(elapsed<60||elapsed>31536000||uncertainty<1||uncertainty>3600||tolerance<=0||tolerance>3600||fabs(drift)>31536000){error("Evidence bounds/resolution invalid",400);return;}
  Reading current=read();bool quality=in["referenceVerified"]==true&&current.valid&&!current.osf;
  int verdict=RtcMath::evidence(elapsed,drift,uncertainty,tolerance,quality,kind=="retention",in["physicalPowerRemoved"]==true);
  String result=verdict==1?"PASS":verdict==-1?"FAIL":"INCOMPLETE";
  // Keep only normalized, bounded evidence. No caller-selected verdict can force PASS.
  JsonDocument normalized;for(const char *k:{"schema","kind","runId","operator","source","sessionId","clockRevision","elapsedSeconds","driftSeconds","uncertaintySeconds","toleranceSeconds","referenceVerified","physicalPowerRemoved"})normalized[k].set(in[k]);
  normalized["result"]=result;external[kind].set(normalized.as<JsonObject>());
  meta["runId"]="";meta["runSessionId"]="";if(!saveMeta()){error(rtcDetail);return;}
  rtcDetail="Bukti eksternal portal: akurasi="+external["accuracy"]["result"].as<String>()+", retensi="+external["retention"]["result"].as<String>()+"; retensi adalah observasi operator, bukan sensor suplai";
  record(current);String why;bool saved=qcSaveProgress(why);JsonDocument d;d["ok"]=saved;d["result"]=result;d["detail"]=why;send(d,saved?200:507);
 });
 server.on("/api/rtc/run/release",HTTP_POST,[]{
  if(!auth())return;
  if(webOtaBusy()||!metadataReady||web->arg("confirm")!="1"||web->arg("runId")!=meta["runId"].as<String>()||!rtcRunActive()||web->arg("sessionId")!=meta["runSessionId"].as<String>()||web->arg("clockRevision")!=String(rtcClockRevision())){error("Explicit original run/session/revision required to release orphan run");return;}
  meta["runId"]="";meta["runSessionId"]="";if(!saveMeta()){error(rtcDetail);return;}JsonDocument d;d["ok"]=true;d["detail"]="RTC lock released as aborted; no PASS/evidence erased";send(d);
 });
}
