#include "network_qc.h"
#include "qc_session.h"
#include "qc_hardware.h"
#include "config.h"
#include "ota_manager.h"
#include <WiFi.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <esp_mac.h>
#include <atomic>
namespace {
struct Job {
  int kind = 0; bool ethernet = false, dhcp = true;
  char ssid[33] = {}, wifiPassword[65] = {}, host[97] = {}, user[65] = {}, password[97] = {}, clientId[65] = {};
  uint16_t port = 1883;
  uint8_t ip[4] = {}, gateway[4] = {}, subnet[4] = {}, dns[4] = {};
};
struct Result { int kind = 0, status = 1, mqttCode = 0; bool ethernet = false; char ip[20] = {}, message[192] = {}; };
QueueHandle_t jobs = nullptr, results = nullptr;
bool busy = false; std::atomic<bool> cancelled{false};
Result latest; String networkDetail = "No network test performed";
String wifiStatus = "BELUM DIUJI", ethStatus = "BELUM DIUJI", mqttStatus = "BELUM DIUJI";
const char *item(int kind) { return kind==0?"WiFi network":kind==1?"Ethernet network":"MQTT"; }
void worker(void *) {
  Job job;
  while (xQueueReceive(jobs,&job,portMAX_DELAY)==pdTRUE) {
    Result r; r.kind=job.kind; r.ethernet=job.ethernet;
    WiFiClient wifi; EthernetClient eth;
    // Allow the web command response to leave before a requested SSID transition.
    vTaskDelay(pdMS_TO_TICKS(500));
    bool ready = false;
    if (!cancelled && job.ethernet) {
      uint8_t mac[6]; esp_read_mac(mac,ESP_MAC_WIFI_STA); mac[0]=(mac[0]|2)&~1;
      digitalWrite(SD_CS,HIGH); Ethernet.init(ETH_CS);
      if(job.kind==1) {
        if(job.dhcp) ready=Ethernet.begin(mac,5000,1000)!=0;
        else { Ethernet.begin(mac,IPAddress(job.ip),IPAddress(job.dns),IPAddress(job.gateway),IPAddress(job.subnet)); ready=true; }
      } else ready=Ethernet.hardwareStatus()!=EthernetNoHardware;
      ready &= Ethernet.hardwareStatus()!=EthernetNoHardware && Ethernet.linkStatus()==LinkON && uint32_t(Ethernet.localIP())!=0;
      Ethernet.localIP().toString().toCharArray(r.ip,sizeof(r.ip));
      eth.setConnectionTimeout(3000);
      if(!ready) snprintf(r.message,sizeof(r.message),"Ethernet controller/link/IP or DHCP failed; check cable and network config");
    } else if (!cancelled) {
      if(job.kind==0) { // Explicit test applies supplied credentials even if already on the same SSID.
        WiFi.disconnect(false,false); WiFi.begin(job.ssid,job.wifiPassword);
      }
      uint32_t since=millis();
      while(!cancelled && WiFi.status()!=WL_CONNECTED && millis()-since<20000) vTaskDelay(pdMS_TO_TICKS(50));
      ready=WiFi.status()==WL_CONNECTED && uint32_t(WiFi.localIP())!=0;
      WiFi.localIP().toString().toCharArray(r.ip,sizeof(r.ip));
      wifi.setConnectionTimeout(3000);
      if(!ready) snprintf(r.message,sizeof(r.message),"WiFi association/DHCP timeout; check SSID/password/router");
    }
    if(ready&&!cancelled) {
      if(job.kind==2) {
        PubSubClient mqtt(job.ethernet?static_cast<Client &>(eth):static_cast<Client &>(wifi));
        mqtt.setServer(job.host,job.port); mqtt.setSocketTimeout(3); mqtt.setKeepAlive(15); mqtt.setBufferSize(512);
        bool ok=job.user[0]?mqtt.connect(job.clientId,job.user,job.password):mqtt.connect(job.clientId);
        r.mqttCode=mqtt.state(); r.status=ok?0:1;
        if(ok) snprintf(r.message,sizeof(r.message),"MQTT CONNACK accepted via %s; IP=%s (connect test only; disconnected afterward)",job.ethernet?"Ethernet":"WiFi",r.ip);
        // Keep formatting explicit for failure so code is reliably shown.
        if(!ok) snprintf(r.message,sizeof(r.message),"MQTT connect failed via %s; state=%d",job.ethernet?"Ethernet":"WiFi",r.mqttCode);
        mqtt.disconnect();
      } else {
        r.status=job.ethernet&&!job.dhcp?3:0; snprintf(r.message,sizeof(r.message),"%s link/association + IP acquisition OK; IP=%s",job.ethernet?"Ethernet":"WiFi",r.ip);
      }
    }
    wifi.stop(); eth.stop();
    if(cancelled) { r.status=2; snprintf(r.message,sizeof(r.message),"Cancelled by Stop/new session/OTA"); }
    memset(job.wifiPassword,0,sizeof(job.wifiPassword)); memset(job.password,0,sizeof(job.password));
    xQueueSend(results,&r,portMAX_DELAY);
  }
}
bool copyArg(WebServer &web,const char *key,char *out,size_t length,bool required=false) {
  String s=web.arg(key); if(s.length()>=length || (required&&s.isEmpty())) return false;
  s.toCharArray(out,length); return true;
}
void reply(WebServer &web,bool ok,const char *message) {
  JsonDocument d;d["ok"]=ok;d[ok?"detail":"error"]=message;String body;serializeJson(d,body);web.send(ok?200:409,"application/json",body);
}
}
bool networkQcBusy(){return busy;}
void cancelNetworkQc(){if(busy)cancelled=true;}
void resetNetworkQc(){cancelNetworkQc();wifiStatus=ethStatus=mqttStatus="BELUM DIUJI";latest={};networkDetail="New QC session";}
void beginNetworkQc(WebServer &web,bool (*authorize)(),void (*stop)()) {
  jobs=xQueueCreate(1,sizeof(Job));results=xQueueCreate(1,sizeof(Result));
  if(jobs&&results) { if(xTaskCreate(worker,"qc-network",6144,nullptr,1,nullptr)!=pdPASS){vQueueDelete(jobs);vQueueDelete(results);jobs=results=nullptr;} }
  web.on("/api/network/test",HTTP_POST,[&web,authorize,stop] {
    if(!authorize())return;
    if(!jobs||busy||webOtaBusy()||pdNegotiating()){reply(web,false,"Network worker unavailable/busy or OTA/PD active");return;}
    Job job;String kind=web.arg("kind"),link=web.arg("interface");
    if(kind!="wifi"&&kind!="ethernet"&&kind!="mqtt"){reply(web,false,"Choose WiFi/Ethernet/MQTT test");return;}
    job.kind=kind=="wifi"?0:kind=="ethernet"?1:2;job.ethernet=job.kind==1||link=="ethernet";
    if(job.kind==0 && (!copyArg(web,"ssid",job.ssid,sizeof(job.ssid),true)||!copyArg(web,"wifiPassword",job.wifiPassword,sizeof(job.wifiPassword)))){reply(web,false,"Invalid SSID/password length");return;}
    if(job.kind==1) {
      String method=web.arg("method");if(method!="dhcp"&&method!="static"){reply(web,false,"Select DHCP/static");return;}job.dhcp=method=="dhcp";
      if(!job.dhcp) {IPAddress ip,gateway,subnet,dns;if(!ip.fromString(web.arg("ip"))||!gateway.fromString(web.arg("gateway"))||!subnet.fromString(web.arg("subnet"))||!dns.fromString(web.arg("dns"))||uint32_t(ip)==0){reply(web,false,"Invalid static IPv4 configuration");return;}for(int i=0;i<4;++i){job.ip[i]=ip[i];job.gateway[i]=gateway[i];job.subnet[i]=subnet[i];job.dns[i]=dns[i];}}
    }
    if(job.kind==2) {
      String port=web.arg("port");bool valid=port.length()>0&&port.length()<6;for(char c:port)valid &= isDigit(c);long p=port.toInt();
      if((link!="wifi"&&link!="ethernet")||!valid||p<1||p>65535||!copyArg(web,"host",job.host,sizeof(job.host),true)||!copyArg(web,"user",job.user,sizeof(job.user))||!copyArg(web,"password",job.password,sizeof(job.password))||!copyArg(web,"clientId",job.clientId,sizeof(job.clientId))){reply(web,false,"Invalid MQTT host/port/client/auth settings");return;}job.port=p; if(!job.clientId[0])snprintf(job.clientId,sizeof(job.clientId),"QC-%s-%08lx",unitSerial(),static_cast<unsigned long>(esp_random()));
    }
    if(job.kind==0 && String(job.ssid)==STA_SSID && !job.wifiPassword[0]) strncpy(job.wifiPassword,STA_PASSWORD,sizeof(job.wifiPassword)-1);
    if(job.kind==2 && job.password[0] && !job.user[0]){reply(web,false,"MQTT password authentication requires a username");return;}
    stop();cancelled=false;busy=true;
    networkDetail="Network test queued; settings volatile, credentials excluded from report"; if(job.kind==0)wifiStatus="TESTING";else if(job.kind==1)ethStatus="TESTING";else mqttStatus="TESTING";
    qcRecord(item(job.kind),"TESTING",networkDetail);
    if(xQueueSend(jobs,&job,0)!=pdTRUE){busy=false;reply(web,false,"Network queue unavailable");return;}
    reply(web,true,"Network test started; changing SSID may change device IP, recovery AP remains available");
  });
}
void updateNetworkQc() {
  Result r;if(results&&xQueueReceive(results,&r,0)==pdTRUE){busy=false;latest=r;networkDetail=r.message;String status=r.status==0?"PASS":r.status==2?"INCOMPLETE":r.status==3?"UNVERIFIED":"FAIL";if(r.kind==0)wifiStatus=status;else if(r.kind==1)ethStatus=status;else mqttStatus=status;qcRecord(item(r.kind),status,networkDetail);if(r.kind==2&&r.ethernet&&r.status==0){ethStatus="PASS";qcRecord("Ethernet network","PASS","MQTT CONNACK received over Ethernet/IP; TCP network path verified");}}
}
void networkQcState(JsonObject d) {
  d["active"]=busy;d["wifiStatus"]=wifiStatus;d["ethernetStatus"]=ethStatus;d["mqttStatus"]=mqttStatus;d["detail"]=networkDetail;d["lastIP"]=latest.ip;d["mqttCode"]=latest.mqttCode;
  d["staSSID"]=WiFi.SSID();d["staIP"]=WiFi.localIP().toString();d["staConnected"]=WiFi.status()==WL_CONNECTED;
}
