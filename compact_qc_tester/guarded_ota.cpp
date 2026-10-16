#include "guarded_ota.h"
#include "ota_guard_policy.h"
#include "config.h"
#include "ota_manager.h"
#include "qc_hardware.h"
#include "tpl_watchdog.h"
#if __has_include("ota_secrets.h")
#include "ota_secrets.h"
#else
#include "lab_ota_defaults.h"
#endif
#include <ArduinoJson.h>
#include <Update.h>
#include <esp_ota_ops.h>
#include <esp_image_format.h>
#include <mbedtls/sha256.h>
#include <algorithm>
#include <string>

namespace {
constexpr char target[]="TMC_TARGET=TMC|TMC_T_V2_R1+TMC_B_V3_R0|ESP32|tmc-legacy-16mb";
constexpr char revision[]="TMC_T_V2_R1+TMC_B_V3_R0";
constexpr char partitionScheme[]="tmc-legacy-16mb";
WebServer *server; bool (*readyCheck)(String &);
bool active=false,done=false,reboot=false,started=false; uint32_t issued=0,lastChunk=0,rebootAt=0;
String nonce,error,expectedHash,expectedVersion,expectedBuild,runningHash;
size_t expectedSize=0,received=0,prefixSize=0; uint8_t prefix[256];
mbedtls_sha256_context hash;
std::string markerTail; bool targetFound=false,buildFound=false;
void send(int code,JsonDocument &d){String body;serializeJson(d,body);server->sendHeader("Cache-Control","no-store");server->send(code,"application/json",body);}
void reject(const String &why){if(started)Update.abort();started=active=done=false;error=why;}
bool auth(){if(server->authenticate(OTA_USERNAME,OTA_PASSWORD))return true;server->requestAuthentication(DIGEST_AUTH,"Compact QC OTA");return false;}
bool available(String &why){if(webOtaBusy()){why="OTA already running";return false;}return readyCheck(why);}
bool layoutMatches(){
 const char *names[]={"nvs","otadata","factory","ota_0","ota_1","storage"};
 const uint32_t offsets[]={0x9000,0x17000,0x20000,0x220000,0x420000,0x620000};
 const uint32_t sizes[]={0x8000,0x2000,0x200000,0x200000,0x200000,0xf0000};
 for(unsigned i=0;i<6;i++){const auto *p=esp_partition_find_first(ESP_PARTITION_TYPE_ANY,ESP_PARTITION_SUBTYPE_ANY,names[i]);if(!p||p->address!=offsets[i]||p->size!=sizes[i])return false;}return true;
}
bool hex64(const String &v){return ota_guard::sha256Valid(v.c_str());}
String digest(const uint8_t *bytes){char out[65];for(unsigned i=0;i<32;i++)snprintf(out+2*i,3,"%02x",bytes[i]);return out;}
void identify(){
 JsonDocument d;String why;const esp_partition_t *slot=esp_ota_get_next_update_partition(nullptr);
 d["deviceId"]=unitSerial();d["productCode"]="TMC";d["hardwareRevision"]=revision;d["chipFamily"]="ESP32";
 d["firmwareVersion"]=FW_VERSION;d["buildUTC"]=FW_BUILD_UTC;d["firmwareSha256"]=runningHash;
 d["flashSizeBytes"]=ESP.getFlashChipSize();d["partitionScheme"]=partitionScheme;d["otaSlotSizeBytes"]=slot?slot->size:0;
 d["otaSupported"]=layoutMatches()&&slot&&slot->size>=0x200000&&!runningHash.isEmpty();d["otaPath"]="/api/ota";d["otaAuthMode"]="digest";
 d["identityContract"]="TELEMETRIC-OTA-1";d["deviceGuard"]="TELEMETRIC-OTA-1";
 if(!available(why))d["updateBlockedReason"]=why;send(200,d);
}
void prepare(){
 if(!auth())return;String why;if(!available(why)){JsonDocument d;d["error"]=why;send(409,d);return;}
 char value[33];snprintf(value,sizeof(value),"%08lx%08lx%08lx%08lx",(unsigned long)esp_random(),(unsigned long)esp_random(),(unsigned long)esp_random(),(unsigned long)esp_random());
 nonce=value;issued=millis();JsonDocument d;d["nonce"]=nonce;d["expiresInSeconds"]=120;send(200,d);
}
void chunks(){
 auto &file=server->upload();
 if(file.status==UPLOAD_FILE_START){
  done=false;String why;
  if(!auth())return;
  if(!available(why)){error=why;return;}
  if(!layoutMatches()){error="Runtime partition layout mismatch";return;}
  if(nonce.isEmpty()||server->arg("nonce")!=nonce||millis()-issued>120000){error="Expired or invalid one-use nonce";return;}nonce="";
  if(!ota_guard::targetMatches(server->header("x-telemetric-product-code").c_str(),server->header("x-telemetric-device-id").c_str(),unitSerial(),server->header("x-telemetric-hardware-revision").c_str(),server->header("x-telemetric-partition-scheme").c_str(),server->header("x-telemetric-archive-confirmed").c_str())){error="Product/device/PCB/partition or archive confirmation mismatch";return;}
  expectedHash=server->header("x-telemetric-firmware-sha256");expectedHash.toLowerCase();expectedVersion=server->header("x-telemetric-firmware-version");expectedBuild=server->header("x-telemetric-build-utc");
  String size=server->header("x-telemetric-app-size");for(char c:size)if(!isdigit((unsigned char)c)){error="Invalid size";return;}
  expectedSize=strtoul(size.c_str(),nullptr,10);const auto *slot=esp_ota_get_next_update_partition(nullptr);
  if(!hex64(expectedHash)||expectedVersion.isEmpty()||expectedVersion.length()>31||expectedBuild.length()!=20||!slot||!ota_guard::capacityFits(expectedSize,slot->size)){error="Invalid application metadata or slot capacity";return;}
  received=prefixSize=0;started=false;targetFound=buildFound=false;markerTail.clear();error="";
  mbedtls_sha256_starts(&hash,0);active=true;lastChunk=millis();
 }else if(file.status==UPLOAD_FILE_WRITE&&active){
  if(received+file.currentSize>expectedSize||!tplWatchdogReady()){reject("Oversized image or watchdog unavailable");return;}
  // Verify markers across arbitrary transport chunk boundaries, not strings truncated at NUL.
  std::string scan=markerTail;scan.append((const char*)file.buf,file.currentSize);
  targetFound=targetFound||scan.find(target)!=std::string::npos;
  std::string build=std::string("QC_BUILD=")+expectedVersion.c_str()+"|"+expectedBuild.c_str();
  buildFound=buildFound||scan.find(build)!=std::string::npos;
  markerTail=scan.substr(scan.size()>160?scan.size()-160:0);
  mbedtls_sha256_update(&hash,file.buf,file.currentSize);
  size_t consumed=0;
  if(prefixSize<sizeof(prefix)){consumed=std::min(sizeof(prefix)-prefixSize,file.currentSize);memcpy(prefix+prefixSize,file.buf,consumed);prefixSize+=consumed;}
  if(!started&&prefixSize==sizeof(prefix)){
   // Arduino core ships a generic app descriptor; it is NOT product identity.
   // Reject wrong-chip/full images here; require compiled target + exact build + hash BEFORE activation.
   if(!ota_guard::appHeaderValid(prefix,prefixSize)){reject("Not a matching ESP32 16MB application image");return;}
   if(!Update.begin(expectedSize,U_FLASH)){reject("Cannot open inactive OTA slot");return;}started=true;
   if(Update.write(prefix,sizeof(prefix))!=sizeof(prefix)){reject("Flash prefix failed");return;}
  }
  if(started&&consumed<file.currentSize&&Update.write(file.buf+consumed,file.currentSize-consumed)!=file.currentSize-consumed){reject("Flash write failed");return;}
  received+=file.currentSize;lastChunk=millis();updateTplWatchdog();
 }else if(file.status==UPLOAD_FILE_END&&active){
  uint8_t bytes[32];mbedtls_sha256_finish(&hash,bytes);
  if(file.totalSize!=expectedSize||!ota_guard::activateAllowed(started,received,expectedSize,digest(bytes)==expectedHash,targetFound,buildFound)){reject("Size/SHA-256/compiled target/build mismatch; active firmware retained");return;}
  if(!Update.end(false)){reject("ESP image validation failed");return;}active=started=false;done=true;
 }else if(file.status==UPLOAD_FILE_ABORTED&&active)reject("Upload interrupted; active firmware retained");
}
void finish(){if(!auth())return;JsonDocument d;if(!done){d["error"]=error.isEmpty()?"Upload not accepted":error;send(409,d);return;}d["accepted"]=true;send(200,d);done=false;reboot=true;rebootAt=millis();}
}
void beginGuardedOta(WebServer &web,bool (*ready)(String &)){
 server=&web;readyCheck=ready;mbedtls_sha256_init(&hash);
 const char *headers[]={"x-telemetric-product-code","x-telemetric-device-id","x-telemetric-hardware-revision","x-telemetric-partition-scheme","x-telemetric-firmware-sha256","x-telemetric-firmware-version","x-telemetric-build-utc","x-telemetric-app-size","x-telemetric-archive-confirmed"};web.collectHeaders(headers,9);
 // Hash the actual running flash image, including its appended digest, once at boot.
 const auto *part=esp_ota_get_running_partition();esp_image_metadata_t meta={};esp_partition_pos_t pos={part->address,part->size};
 if(esp_image_get_metadata(&pos,&meta)==ESP_OK&&meta.image_len>0&&meta.image_len<=part->size){
  mbedtls_sha256_context ctx;mbedtls_sha256_init(&ctx);mbedtls_sha256_starts(&ctx,0);uint8_t bytes[2048];bool ok=true;
  for(size_t at=0;at<meta.image_len;at+=sizeof(bytes)){size_t n=std::min(sizeof(bytes),size_t(meta.image_len-at));if(esp_partition_read(part,at,bytes,n)!=ESP_OK){ok=false;break;}mbedtls_sha256_update(&ctx,bytes,n);updateTplWatchdog();yield();}
  uint8_t result[32];mbedtls_sha256_finish(&ctx,result);mbedtls_sha256_free(&ctx);if(ok)runningHash=digest(result);
 }
 // Keep the compatibility marker in the executable, not merely in a manifest.
 Serial.println(target);
 web.on("/api/device-info",HTTP_GET,identify);web.on("/api/ota/prepare",HTTP_GET,prepare);web.on("/api/ota",HTTP_POST,finish,chunks);
}
bool guardedOtaBusy(){return active||reboot;}
void updateGuardedOta(){if(active&&millis()-lastChunk>15000)reject("Upload timed out");if(reboot&&millis()-rebootAt>750)ESP.restart();}
