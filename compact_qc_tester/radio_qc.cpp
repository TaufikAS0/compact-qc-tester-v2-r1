#include "radio_qc.h"
#include "radio_protocol.h"
#include "config.h"
#include "qc_hardware.h"
#include "qc_session.h"
#include "qc_profiles.h"
#include "ota_manager.h"
#include "current_qc.h"
#include "current_calibration.h"
#include <HardwareSerial.h>
namespace {
HardwareSerial radio(2);
bool active=false,saved=false,entered=false,matchedEntry=false,applyConfig=false,preflight=false,ack=false,commandError=false,writeAttempted=false,writeVerified=false;
uint8_t oldDir=0,oldLatch=0,selectedMask=0,writtenMask=0,ackedMask=0;
uint32_t at=0,readAt=0,readSequence=0;int stage=0,command=0,retries=0;
int expected[7]={0,4,1,65535,7,0,0},actual[7],baseline[7],written[7];
String line,version,commStatus="BELUM DIUJI",configStatus="BELUM DIUJI",radioDetail,trace[7],readId,readSession,readProfileRevision;
using namespace RadioProtocol;
void traceLine(const String &s){if(command<7&&trace[command].length()<450)trace[command]+=s.substring(0,120)+"\n";}
bool restore(){return !saved||mcpRestorePortBits(MCP1_ADDR,1,0x30,oldLatch,oldDir);}
void publish(){qcRecord("Radio communication",commStatus,radioDetail);qcRecord("Radio configuration",configStatus,radioDetail);}
void finish(){radio.end();active=false;if(!restore()){commStatus=configStatus="FAIL";writeVerified=false;radioDetail+="; pemulihan pin mode gagal";}saved=false;publish();if(qcProfileSelected()){String why;if(!qcSaveProgress(why))radioDetail+="; checkpoint belum tersimpan: "+why;}}
void exitMode(){radio.print("AT+EXIT\r\n");line="";stage=4;at=millis();matchedEntry=false;}
void sendQuery(){line="";ack=commandError=false;String request=command<7?String("AT+")+keys[command]+"=?":"AT+VER";traceLine("TX "+request);radio.print(request+"\r\n");at=millis();stage=3;}
void sendWrite(){line="";ack=commandError=false;writeAttempted=true;String request=String("AT+")+keys[command]+"="+expected[command];traceLine("TX "+request);radio.print(request+"\r\n");at=millis();stage=5;}
bool nextWrite(int from){for(command=from;command<7;++command)if(selectedMask&(1<<command)){sendWrite();return true;}return false;}
void beginQueries(){for(auto &n:actual)n=-1;command=0;retries=0;sendQuery();}
void evaluate(){
 bool missing=false;String values;for(int i=0;i<7;++i){missing|=actual[i]<0;values+=String(" ")+keys[i]+"="+(actual[i]<0?String("BELUM TERBACA"):String(actual[i]));}
 commStatus=missing?"UNVERIFIED":"PASS";radioDetail="Baca parameter:"+values;
 if(applyConfig&&preflight){
  for(int i=0;i<7;++i)if(selectedMask&(1<<i)){
   if(actual[i]<0||actual[i]!=baseline[i]){configStatus="UNVERIFIED";radioDetail+="; setelan awal berubah/tidak terbaca: "+String(keys[i])+". Tidak ada parameter ditulis; baca dan tinjau ulang";exitMode();return;}
   if(actual[i]==expected[i])selectedMask&=~(1<<i);
  }
  if(!selectedMask){configStatus="BELUM DIUJI";radioDetail+="; tidak ada perubahan, bukan bukti uji tulis";exitMode();return;}
  preflight=false;nextWrite(0);return;
 }
 if(applyConfig){
  bool absent=false,different=false;for(int i=0;i<7;++i)if(selectedMask&(1<<i)){absent|=actual[i]<0;different|=actual[i]>=0&&actual[i]!=expected[i];}
  writeVerified=!absent&&!different&&(ackedMask&selectedMask)==selectedMask;writtenMask=selectedMask;for(int i=0;i<7;++i)written[i]=expected[i];
  configStatus=different?"FAIL":absent?"UNVERIFIED":writeVerified?"PASS":"UNVERIFIED";
  radioDetail+="; tulis+verifikasi hanya parameter yang diubah: ";for(int i=0;i<7;++i)if(selectedMask&(1<<i))radioDetail+=String(keys[i])+" ";
  radioDetail+="; ACK dan baca ulang diperlukan. Belum uji simpan setelah power-cycle; beberapa setelan perlu restart";
 }else if(writeVerified){
  bool absent=false,different=false;for(int i=0;i<7;++i)if(writtenMask&(1<<i)){absent|=actual[i]<0;different|=actual[i]>=0&&actual[i]!=written[i];}
  configStatus=different?"FAIL":absent?"UNVERIFIED":"PASS";if(absent||different)writeVerified=false;
 }else{configStatus="BELUM DIUJI";radioDetail+="; belum uji tulis. Perbedaan draft bukan kegagalan hardware";}
 ++readSequence;readId=String(readSequence)+"-"+qcSessionId();readSession=qcSessionId();readProfileRevision=String(activeQcProfileRevision());readAt=millis();
 radioDetail+="; bukan uji RF/jangkauan";exitMode();
}
void consumeLine(){
 line.trim();if(stage==2&&line=="Enter AT Mode")matchedEntry=true;if(stage==4&&line=="Exit AT Mode")matchedEntry=true;
 if(stage==3||stage==5){traceLine("RX "+line);ack|=writeAck(std::string_view(line.c_str(),line.length()));commandError|=line.startsWith("ERROR")||line.startsWith("+ERROR");}
 if(stage==3&&command<7){int n=parse(std::string_view(line.c_str(),line.length()),command);if(n>=0)actual[command]=n;}
 else if(stage==3&&command==7&&line.length()&&!line.startsWith("AT+")&&line!="OK"&&line!="+OK"&&!line.startsWith("ERROR"))version=line;
 line="";
}
}
bool radioQcActive(){return active;}
void resetRadioQc(){if(active)return;commStatus=configStatus="BELUM DIUJI";radioDetail=version=readId="";writeAttempted=writeVerified=false;writtenMask=ackedMask=0;for(int i=0;i<7;++i){actual[i]=-1;trace[i]="";}}
void cancelRadioQc(const char *reason){if(!active)return;commStatus=configStatus="INCOMPLETE";writeVerified=false;radioDetail=reason;radio.print("AT+EXIT\r\n");finish();}
void radioQcState(JsonObject out){
 out["schema"]="QC-RADIO-2";out["active"]=active;out["model"]="E70-915NW14S";out["technology"]="CC1310 / 15.4 star (not LoRa)";
 out["communication"]=commStatus;out["configuration"]=configStatus;out["detail"]=radioDetail;out["versionResponse"]=version;
 out["readId"]=readId;out["readSessionId"]=readSession;out["readProfileRevision"]=readProfileRevision;out["readAgeMs"]=readId.length()?millis()-readAt:0;out["operation"]=applyConfig?"WRITE_VERIFY":"READ";
 out["writeAttempted"]=writeAttempted;out["writeVerified"]=writeVerified;out["writeMask"]=writtenMask;out["ackedMask"]=ackedMask;out["stage"]=stage;
 out["sf"]=out["bandwidth"]=out["codingRate"]=out["loraCRC"]="NOT SUPPORTED";
 auto params=out["parameters"].to<JsonArray>();for(int i=0;i<7;++i){auto p=params.add<JsonObject>();p["name"]=keys[i];p["target"]=expected[i];if(actual[i]>=0)p["read"]=actual[i];p["status"]=actual[i]<0?"UNVERIFIED":"READ OK";p["diagnostic"]=trace[i];p["written"]=bool(writtenMask&(1<<i));}
 if(actual[0]>=0&&actual[2]>=0)out["frequencyMHz"]=908.2+.2*actual[0];out["ack"]=digitalRead(34);out["aux"]=digitalRead(35);out["link"]=digitalRead(36);
}
void beginRadioQc(WebServer &web,bool (*authorize)(),void (*stop)()){
 for(auto &n:actual)n=-1;
 web.on("/api/radio/start",HTTP_POST,[&web,authorize,stop]{
  if(!authorize())return;JsonDocument d;String apply=web.arg("apply");bool writing=apply=="1",valid=apply.isEmpty()||apply=="0"||writing;int next[7];uint8_t mask=0;
  for(int i=0;i<7;++i){next[i]=actual[i];if(!writing)continue;String fields=","+web.arg("fields")+",";if(fields.indexOf(","+String(keys[i])+",")<0)continue;
   mask|=1<<i;String value=web.arg(keys[i]);if(value.isEmpty()||value.length()>5)valid=false;for(char c:value)if(c<'0'||c>'9')valid=false;next[i]=value.toInt();if(next[i]>maxima[i]||actual[i]<0)valid=false;}
  JsonDocument calibration;calibrationState(calibration.to<JsonObject>());
  if(active||webOtaBusy()||pdNegotiating()||currentQcBusy()||calibration["pending"].as<int>()>=0)d["error"]="Tunggu radio/OTA/PD/capture selesai";
  else if(writing&&(!qcTestEnabled("Radio configuration")||web.arg("confirm")!="1"||web.arg("sessionId")!=qcSessionId()||web.arg("profileRevision")!=String(activeQcProfileRevision())||readSession!=qcSessionId()||readProfileRevision!=String(activeQcProfileRevision())||readId.isEmpty()||web.arg("readId")!=readId||millis()-readAt>300000))d["error"]="Baca parameter terbaru dan tinjau/konfirmasi tulis; sesi/profil/readId berubah atau kedaluwarsa";
  else if(!valid||(writing&&!mask))d["error"]="Parameter tulis tidak valid/belum terbaca atau tidak ada perubahan";
  else{
   stop();qcRetest("Radio communication");qcRetest("Radio configuration");readId="";
   for(int i=0;i<7;++i){baseline[i]=actual[i];expected[i]=next[i];trace[i]="";actual[i]=-1;}
   applyConfig=writing;preflight=writing;selectedMask=mask;ackedMask=0;writeAttempted=false;
   commStatus="TESTING";if(writing){configStatus="TESTING";writeVerified=false;}else if(!writeVerified)configStatus="BELUM DIUJI";
   radioDetail=writing?"Baca ulang setelan awal sebelum menulis perubahan":"Baca parameter tersimpan; tidak menulis setelan";publish();
   uint8_t bank,gpio=0,dir=0;
   if(!qcReadReg(MCP1_ADDR,0x0a,&bank,1)||(bank&0xa0)||!qcReadReg(MCP1_ADDR,0x01,&oldDir,1)||!qcReadReg(MCP1_ADDR,0x15,&oldLatch,1))d["error"]="Pemetaan mode MCP1 tidak tersedia";
   else{
    saved=true;if(!mcpBit(MCP1_ADDR,RADIO_M0_BIT,true)||!mcpBit(MCP1_ADDR,RADIO_M1_BIT,true)||!qcReadReg(MCP1_ADDR,0x13,&gpio,1)||!qcReadReg(MCP1_ADDR,0x01,&dir,1)||(gpio&0x30)!=0x30||(dir&0x30)){bool ok=restore();saved=false;d["error"]=ok?"Gagal masuk pin mode konfigurasi":"Gagal masuk/pulihkan pin mode";}
    else{radio.setRxBufferSize(512);radio.begin(115200,SERIAL_8N1,RADIO_RX,RADIO_TX);if(!radio){radio.end();bool restored=restore();saved=false;d["error"]=restored?"Inisialisasi UART2 gagal":"Inisialisasi UART2 dan pemulihan pin mode gagal";}else{entered=matchedEntry=false;command=stage=retries=0;line=version="";active=true;at=millis();d["ok"]=true;}}
   }
  }
  if(!d["error"].isNull()&&commStatus=="TESTING"){commStatus="FAIL";configStatus="UNVERIFIED";radioDetail=d["error"].as<String>();publish();}
  String body;serializeJson(d,body);web.send(d["error"].isNull()?200:409,"application/json",body);
 });
}
void updateRadioQc(){
 if(!active)return;uint32_t now=millis();for(int n=0;n<128&&radio.available();++n){char c=radio.read();if(c=='\r'||c=='\n')consumeLine();else if((static_cast<unsigned char>(c)>=32||c=='\t')&&line.length()<128)line+=c;}
 if(stage==0&&now-at>=200){radio.write(uint8_t(0));stage=1;at=now;}
 else if(stage==1&&now-at>=120){radio.print("+++");stage=2;at=now;}
 else if(stage==2){if(matchedEntry){entered=true;beginQueries();}else if(now-at>=1500){commStatus="FAIL";configStatus="UNVERIFIED";radioDetail="Tidak ada respons Enter AT Mode; periksa daya/UART/pin mode. Tidak ada setelan ditulis";exitMode();}}
 else if(stage==5){if(ack){ackedMask|=1<<command;if(!nextWrite(command+1))beginQueries();}else if(commandError||now-at>=1500){configStatus=commandError?"FAIL":"UNVERIFIED";radioDetail="Tulis ditolak/timeout pada "+String(keys[command])+"; sebagian perubahan sebelumnya mungkin tersimpan. Baca ulang sebelum melanjutkan";exitMode();}}
 else if(stage==3&&((command<7&&actual[command]>=0)||now-at>=1500)){
  if(line.length())consumeLine();if(command<7&&actual[command]<0&&retries==0){++retries;sendQuery();return;}
  retries=0;if(++command<=7)sendQuery();else evaluate();
 }else if(stage==4&&(matchedEntry||now-at>=500)){if(!matchedEntry&&entered){commStatus=configStatus="UNVERIFIED";writeVerified=false;radioDetail+="; keluar AT belum terkonfirmasi; periksa modul";}finish();}
}
