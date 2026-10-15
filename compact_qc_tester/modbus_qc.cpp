#include "modbus_qc.h"
#include "modbus_math.h"
#include "qc_hardware.h"
#include "qc_session.h"
#include "config.h"
#include "ota_manager.h"
#include <HardwareSerial.h>
namespace {
struct Tag {
  String name = "Tag", parity = "N", type = "Unsigned", order = "ABCD";
  uint32_t baud = 9600, timeout = 2000;
  uint16_t address = 0, count = 1, raw[2] = {};
  uint8_t slave = 1, function = 3;
  bool valid = false; int verdict = 0;
  double value = 0; uint32_t at = 0, requestId = 0;
  String status = "BELUM DIUJI", detail;
};
Tag tags[2];
HardwareSerial uart(1);
int pending = -1; uint32_t started = 0;
uint8_t rx[16]; size_t received = 0;
void record(int n) {
  auto &p = tags[n]; qcRecord("Modbus tag:"+String(n+1), p.status, p.name+"; "+p.detail);
  if (tags[0].verdict == 1 && tags[1].verdict == 1)
    qcRecord("RS485", "PASS", "Two Modbus tags CRC-valid; both user-confirmed sesuai");
  else if (tags[0].status == "FAIL" || tags[1].status == "FAIL")
    qcRecord("RS485", "FAIL", "Modbus tag failure; see per-tag evidence");
  else qcRecord("RS485", "MENUNGGU USER", "Two Modbus tag requests + user confirmations required");
}
void end(const char *status, const String &detail) {
  int n=pending; uart.end(); pending=-1;
  if(n<0) return;
  auto &p=tags[n]; p.status=status; p.detail=detail; record(n);
}
void reply(WebServer &web, bool ok, const String &detail) {
  JsonDocument d; d["ok"]=ok; d[ok?"detail":"error"]=detail;
  String body;serializeJson(d,body);web.send(ok?200:409,"application/json",body);
}
bool num(WebServer &web,const char *name,uint32_t lo,uint32_t hi,uint32_t &out) {
  String s=web.arg(name); if(!s.length()||s.length()>9) return false;
  for(char c:s)if(c<'0'||c>'9')return false;
  out=strtoul(s.c_str(),nullptr,10);return out>=lo&&out<=hi;
}
bool selected(WebServer &web,int &n) {uint32_t v;if(!num(web,"index",0,1,v))return false;n=v;return true;}
void parse() {
  auto &p=tags[pending];
  auto result=QcModbus::validate(rx,received,p.slave,p.function,p.count);
  if(result!=QcModbus::Response::Valid) {
    String why=result==QcModbus::Response::Exception?"Modbus exception "+String(rx[2]):result==QcModbus::Response::Crc?"Modbus CRC mismatch":result==QcModbus::Response::Slave?"Unexpected slave address":result==QcModbus::Response::Function?"Unexpected Modbus function":"Invalid response length/byte count";
    end("FAIL",why);return;
  }
  for(int i=0;i<p.count;++i)p.raw[i]=p.function<=2 ? ((rx[3]>>i)&1) : uint16_t(uint16_t(rx[3+2*i])<<8|rx[4+2*i]);
  uint32_t value=p.count==1?p.raw[0]:p.order=="CDAB"?(uint32_t(p.raw[1])<<16|p.raw[0]):(uint32_t(p.raw[0])<<16|p.raw[1]);
  if(p.function<=2)p.value=p.raw[0];
  else if(p.type=="Float"){float f;memcpy(&f,&value,4);if(!isfinite(f)){end("FAIL","Float decoded NaN/Infinity; check datatype/order");return;}p.value=f;}
  else if(p.type=="Signed")p.value=p.count==1?int16_t(value):int32_t(value);
  else p.value=value;
  p.valid=true;p.at=millis();
  end("MENUNGGU USER","CRC-valid response; slave="+String(p.slave)+" FC="+String(p.function)+" offset="+String(p.address)+" raw="+String(p.raw[0])+","+String(p.raw[1])+" value="+String(p.value,6)+" request="+String(p.requestId,HEX)+" uptimeMs="+String(p.at));
}
}
bool modbusQcActive(){return pending>=0;}
void cancelModbusQc(){if(pending>=0)end("INCOMPLETE","Cancelled by Stop/new session/OTA");}
void resetModbusQc(){cancelModbusQc();for(int i=0;i<2;++i){tags[i].valid=false;tags[i].verdict=0;tags[i].status="BELUM DIUJI";tags[i].detail="";}}
void beginModbusQc(WebServer &web,bool (*authorize)(),void (*stop)()) {
  tags[0].name="Tag1";tags[1].name="Tag2";
  web.on("/api/modbus/tag/config",HTTP_POST,[&web,authorize] {
    if(!authorize())return;
    int n;uint32_t b,t,s,f,a,c;String name=web.arg("name"),parity=web.arg("parity"),type=web.arg("type"),order=web.arg("order");
    if(webOtaBusy()||pending>=0||!selected(web,n)||!num(web,"baud",1200,115200,b)||!num(web,"timeout",100,10000,t)||!num(web,"slave",1,247,s)||!num(web,"function",1,4,f)||!num(web,"address",0,65535,a)||!num(web,"count",1,2,c)||a+c>65536||!name.length()||name.length()>10||(parity!="N"&&parity!="E"&&parity!="O")||(type!="Unsigned"&&type!="Signed"&&type!="Float")||(order!="ABCD"&&order!="CDAB")||(type=="Float"&&(f<3||c!=2))) {reply(web,false,"Invalid tag/link/datatype; Float requires2 registers");return;}
    qcRetest("Modbus tag:"+String(n+1));
    auto &p=tags[n];p.name=name;p.parity=parity;p.type=type;p.order=order;p.baud=b;p.timeout=t;p.slave=s;p.function=f;p.address=a;p.count=c;p.valid=false;p.verdict=0;p.status="BELUM DIUJI";p.detail="Config saved in RAM; not communication evidence";record(n);reply(web,true,p.detail);
  });
  web.on("/api/modbus/tag/request",HTTP_POST,[&web,authorize,stop] {
    if(!authorize())return;
    int n;if(webOtaBusy()||pdNegotiating()||pending>=0||!selected(web,n)){reply(web,false,"Wait for active test/OTA/PD or choose valid tag");return;}
    qcRetest("Modbus tag:"+String(n+1));
    stop();auto &p=tags[n];p.valid=false;p.raw[0]=p.raw[1]=0;p.verdict=0;p.status="TESTING";p.detail="Request queued";p.requestId=esp_random();
    if(!mcpBit(MCP2_ADDR,2,false)){p.status="FAIL";p.detail="MCP2A2 LOW route write/readback failed";record(n);reply(web,false,p.detail);return;}
    uart.setRxBufferSize(256);uart.begin(p.baud,p.parity=="E"?SERIAL_8E1:p.parity=="O"?SERIAL_8O1:SERIAL_8N1,SERIAL_RX,SERIAL_TX);
    if(!uart){p.status="FAIL";p.detail="UART1 initialization failed";record(n);reply(web,false,p.detail);return;}
    uint8_t tx[]={p.slave,p.function,uint8_t(p.address>>8),uint8_t(p.address),0,uint8_t(p.count),0,0};uint16_t crc=QcModbus::crc(tx,6);tx[6]=crc;tx[7]=crc>>8;
    while(uart.available())uart.read();pending=n;received=0;started=millis();
    if(uart.write(tx,8)!=8){end("FAIL","UART request queue failed");reply(web,false,p.detail);return;}
    record(n);reply(web,true,"RS485 master request sent; direction automatic, no DE/RE");
  });
  web.on("/api/modbus/tag/confirm",HTTP_POST,[&web,authorize] {
    if(!authorize())return;
    int n;String v=web.arg("verdict");if(webOtaBusy()||pending>=0||!selected(web,n)||!tags[n].valid||(v!="PASS"&&v!="FAIL")){reply(web,false,"Valid response required before user confirmation");return;}
    auto &p=tags[n];p.verdict=v=="PASS"?1:-1;p.status=v;p.detail+="; user "+v;record(n);reply(web,true,p.detail);
  });
  web.on("/api/modbus/stop",HTTP_POST,[&web,authorize]{if(!authorize())return;cancelModbusQc();reply(web,true,"Modbus stopped");});
}
void updateModbusQc() {
  if(pending<0)return;auto &p=tags[pending];
  if(millis()-started>=p.timeout){end("FAIL","Modbus timeout; check power/wiring/slave/baud/parity/register");return;}
  for(int budget=0;budget<16&&uart.available();++budget){
    if(received>=sizeof(rx)){end("FAIL","Modbus RX overflow");return;}
    rx[received++]=uart.read();
    if(received>=3){size_t expected=(rx[1]&0x80)?5:5+rx[2];if(expected>sizeof(rx)){end("FAIL","Modbus length outside tag bound");return;}if(received==expected){parse();return;}}
  }
}
void modbusQcState(JsonObject out) {
  out["active"]=pending>=0;out["pending"]=pending;
  auto list=out["tags"].to<JsonArray>();for(auto &p:tags){auto d=list.add<JsonObject>();d["name"]=p.name;d["baud"]=p.baud;d["parity"]=p.parity;d["slave"]=p.slave;d["function"]=p.function;d["address"]=p.address;d["count"]=p.count;d["type"]=p.type;d["order"]=p.order;d["timeout"]=p.timeout;d["valid"]=p.valid;d["status"]=p.status;d["detail"]=p.detail;d["requestId"]=p.requestId;if(p.valid){d["value"]=p.value;d["uptimeMs"]=p.at;auto r=d["raw"].to<JsonArray>();for(int i=0;i<p.count;++i)r.add(p.raw[i]);}}
}
