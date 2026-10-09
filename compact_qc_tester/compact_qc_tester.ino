#include <Arduino.h>
#if !defined(CONFIG_IDF_TARGET_ESP32)
#error "Provisional V2_R1 profile supports classic ESP32 only. Do not build for ESP32-S3."
#endif
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "config.h"
#include "web_ui.h"

WebServer server(80);
String ssid, token;
uint32_t lastHeartbeat=0, outputStarted=0;
bool outputActive=false, mcpReady=false, fault=false;
uint16_t savedLatch=0, savedDirection=0;
const uint8_t addresses[]={0x68,0x57,0x38,0x20,0x21,0x3c,0x77,0x28,0x48};
const char* deviceNames[]={"RTC","EEPROM","AHT","MCP1","MCP2","OLED","ATtiny404","PD converter","ADS"};

void reply(JsonDocument &doc,int code=200){String body;serializeJson(doc,body);server.sendHeader("Cache-Control","no-store");server.send(code,"application/json",body);}
void failure(const char* why,int code=409){JsonDocument d;d["error"]=why;reply(d,code);}
bool authorized(){if(!server.hasArg("token")||server.arg("token")!=token){failure("Invalid session token",403);return false;}return true;}
bool readMcp(uint8_t reg,uint16_t &value){Wire.beginTransmission(MCP1_ADDR);Wire.write(reg);if(Wire.endTransmission(false)!=0)return false;if(Wire.requestFrom(MCP1_ADDR,(uint8_t)2)!=(uint8_t)2)return false;value=Wire.read();value|=(uint16_t)Wire.read()<<8;return true;}
bool writeMcp(uint8_t reg,uint16_t value){Wire.beginTransmission(MCP1_ADDR);Wire.write(reg);Wire.write((uint8_t)value);Wire.write((uint8_t)(value>>8));return Wire.endTransmission()==0;}
void stopOutputs(){if(OUTPUT_PROFILE_VERIFIED){for(int p:RELAY_PINS)digitalWrite(p,RELAY_ACTIVE_HIGH?LOW:HIGH);if(mcpReady){const uint16_t off=LED_ACTIVE_HIGH?0:LED_MASK;if(!writeMcp(0x14,(savedLatch&~LED_MASK)|off))fault=true;}}outputActive=false;}
void initOutputs(){if(!OUTPUT_PROFILE_VERIFIED)return;for(int p:RELAY_PINS){digitalWrite(p,RELAY_ACTIVE_HIGH?LOW:HIGH);pinMode(p,OUTPUT);}if(!MCP23017_PROFILE_VERIFIED)return;
  // Validate expected BANK=0/SEQOP=0 before changing only LED bits.
  Wire.beginTransmission(MCP1_ADDR);Wire.write(0x0a);if(Wire.endTransmission(false)!=0)return;if(Wire.requestFrom(MCP1_ADDR,(uint8_t)1)!=1)return;if((Wire.read()&0xa0)!=0)return;
  if(!readMcp(0x00,savedDirection)||!readMcp(0x14,savedLatch))return;
  const uint16_t off=LED_ACTIVE_HIGH?0:LED_MASK;
  if(!writeMcp(0x14,(savedLatch&~LED_MASK)|off))return;
  if(!writeMcp(0x00,savedDirection&~LED_MASK))return;
  mcpReady=true;
}
void stateHandler(){JsonDocument d;d["board"]="Compact V2_R1 (provisional)";d["version"]=FW_VERSION;d["chip"]=ESP.getChipModel();d["ssid"]=ssid;d["token"]=token;d["uptime"]=millis();d["relayEnabled"]=OUTPUT_PROFILE_VERIFIED&&!fault;d["ledEnabled"]=OUTPUT_PROFILE_VERIFIED&&mcpReady&&!fault;d["outputActive"]=outputActive;auto a=d["inputs"].to<JsonArray>();for(size_t i=0;i<sizeof(INPUT_PINS)/sizeof(INPUT_PINS[0]);i++){auto v=a.add<JsonObject>();v["name"]=INPUT_NAMES[i];v["pin"]=INPUT_PINS[i];v["value"]=digitalRead(INPUT_PINS[i]);}auto l=d["leds"].to<JsonArray>();for(auto n:LED_NAMES)l.add(n);reply(d);}
void scanHandler(){if(!authorized())return;stopOutputs();JsonDocument d;auto a=d["devices"].to<JsonArray>();for(size_t i=0;i<sizeof(addresses);i++){Wire.beginTransmission(addresses[i]);bool found=Wire.endTransmission()==0;auto v=a.add<JsonObject>();v["name"]=deviceNames[i];char hex[5];snprintf(hex,sizeof(hex),"0x%02X",addresses[i]);v["address"]=hex;v["detected"]=found;}reply(d);}
void sdHandler(){if(!authorized())return;stopOutputs();JsonDocument d;if(!SD.begin(SD_CS,SPI,4000000)){SD.end();d["status"]="FAIL";d["detail"]="SD mount failed";reply(d);return;}char path[48];bool unique=false;for(int attempt=0;attempt<8;attempt++){snprintf(path,sizeof(path),"/qc_%08lx.tmp",(unsigned long)esp_random());if(!SD.exists(path)){unique=true;break;}}if(!unique){SD.end();failure("Cannot allocate unique SD test file");return;}String payload="COMPACT-QC:"+String(esp_random(),HEX);File f=SD.open(path,FILE_WRITE);bool ok=false;if(f){ok=f.print(payload)==payload.length();f.flush();f.close();}String readback;f=SD.open(path,FILE_READ);if(f){while(f.available()&&readback.length()<=payload.length())readback+=(char)f.read();f.close();}ok=ok&&readback==payload;bool clean=SD.remove(path);SD.end();d["status"]=(ok&&clean)?"PASS":"FAIL";d["detail"]=!clean?String("Cleanup failed; test file: ")+path:ok?"Write/read matched; temporary file removed":"Write/read mismatch; temporary file removed";reply(d);}
void outputHandler(){if(!authorized())return;if(!OUTPUT_PROFILE_VERIFIED||fault){failure("Outputs locked: verify config.h and wiring");return;}if((uint32_t)(millis()-lastHeartbeat)>SESSION_TIMEOUT_MS){failure("Heartbeat expired");return;}String raw=server.arg("index");if(raw.length()==0){failure("Missing index",400);return;}for(unsigned i=0;i<raw.length();i++)if(!isDigit(raw[i])){failure("Invalid index",400);return;}long index=raw.toInt();String kind=server.arg("kind");if(kind!="relay"&&kind!="led"){failure("Unknown output kind",400);return;}if(index<0||index>=(kind=="relay"?2:11)){failure("Index outside range",400);return;}if(kind=="led"&&!mcpReady){failure("MCP profile unavailable");return;}stopOutputs();if(fault){failure("Cannot deactivate previous output");return;}if(kind=="relay")digitalWrite(RELAY_PINS[index],RELAY_ACTIVE_HIGH?HIGH:LOW);else{uint16_t bits=LED_ACTIVE_HIGH?(1u<<LED_BITS[index]):(LED_MASK&~(1u<<LED_BITS[index]));if(!writeMcp(0x14,(savedLatch&~LED_MASK)|bits)){fault=true;stopOutputs();failure("MCP write failed");return;}}outputStarted=millis();outputActive=true;JsonDocument d;d["status"]="PERLU KONFIRMASI";reply(d);}
void setup(){Serial.begin(115200);for(int p:INPUT_PINS)pinMode(p,INPUT);Wire.begin(SDA_PIN,SCL_PIN,100000);Wire.setTimeOut(40);
  // Keep Ethernet deselected for SD tests. Boot-time strap levels need hardware verification.
  digitalWrite(ETH_CS,HIGH);pinMode(ETH_CS,OUTPUT);digitalWrite(SD_CS,HIGH);pinMode(SD_CS,OUTPUT);SPI.begin(SPI_SCK,SPI_MISO,SPI_MOSI);
  initOutputs();uint64_t mac=ESP.getEfuseMac();char name[32];snprintf(name,sizeof(name),"COMPACT-QC-%04X",(unsigned)(mac&0xffff));ssid=name;token=String(esp_random(),HEX)+String(esp_random(),HEX);WiFi.mode(WIFI_AP);WiFi.softAP(ssid.c_str(),AP_PASSWORD,1,false,1);
  server.on("/",HTTP_GET,[]{server.send_P(200,"text/html; charset=utf-8",WEB_UI);});server.on("/api/state",HTTP_GET,stateHandler);server.on("/api/i2c",HTTP_POST,scanHandler);server.on("/api/sd",HTTP_POST,sdHandler);server.on("/api/output",HTTP_POST,outputHandler);server.on("/api/heartbeat",HTTP_POST,[]{if(!authorized())return;lastHeartbeat=millis();JsonDocument d;d["ok"]=true;reply(d);});server.on("/api/stop",HTTP_POST,[]{if(!authorized())return;stopOutputs();JsonDocument d;d["ok"]=!fault;reply(d,fault?500:200);});server.onNotFound([]{failure("Not found",404);});server.begin();Serial.printf("\nCompact QC %s\nMCU: %s\nAP: %s\nURL: http://192.168.4.1\nOutputs: %s\n",FW_VERSION,ESP.getChipModel(),ssid.c_str(),OUTPUT_PROFILE_VERIFIED?"verified configuration":"LOCKED");}
void loop(){if(outputActive&&((uint32_t)(millis()-outputStarted)>=OUTPUT_PULSE_MS||(uint32_t)(millis()-lastHeartbeat)>=SESSION_TIMEOUT_MS))stopOutputs();server.handleClient();delay(2);}
