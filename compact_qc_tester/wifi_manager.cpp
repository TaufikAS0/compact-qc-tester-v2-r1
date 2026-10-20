#include "wifi_manager.h"
#include "config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <cstring>
#include <esp_wifi.h>
#include <atomic>
#include <Preferences.h>
#include "network_qc.h"
#include "wifi_retry.h"

namespace {
uint32_t lastAttempt = 0;
bool wasConnected = false;
std::atomic<uint32_t> disconnects{0}, gotIPs{0}, lastDisconnect{0}, lastGotIP{0}, reason{0};
uint32_t retries = 0;
int lastRetryResult = ESP_OK;
bool sleepDisabled = false;
struct SavedWiFi { uint32_t magic; char ssid[33]; char password[65]; uint32_t checksum; };
SavedWiFi configured = {};
bool customProfile = false, settingsPending = false;
bool (*settingsIdle)() = nullptr;
uint32_t settingsAt = 0;
String settingsStatus = "Menghubungkan profil lab";
uint32_t profileChecksum(const SavedWiFi &p) {
  uint32_t h=2166136261u;
  for (char c:p.ssid) h=(h^static_cast<uint8_t>(c))*16777619u;
  for (char c:p.password) h=(h^static_cast<uint8_t>(c))*16777619u;
  return h;
}
bool validProfile(const SavedWiFi &p) {
  return p.magic==0x57494631 && p.ssid[0] && p.ssid[32]==0 && p.password[64]==0 &&
         p.checksum==profileChecksum(p);
}
struct WiFiEventRecord { uint32_t at; uint16_t reason; int8_t rssi; };
WiFiEventRecord events[8] = {};
uint32_t eventCount=0;
portMUX_TYPE eventsMux=portMUX_INITIALIZER_UNLOCKED;
void wifiEvent(arduino_event_id_t event, arduino_event_info_t info) {
  // Called from the network task: counters only, no JSON/String/NVS work here.
  if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
    reason = info.wifi_sta_disconnected.reason;
    lastDisconnect = millis(); ++disconnects;
    portENTER_CRITICAL(&eventsMux);
    events[eventCount++ % 8] = {millis(),info.wifi_sta_disconnected.reason,info.wifi_sta_disconnected.rssi};
    portEXIT_CRITICAL(&eventsMux);
  } else if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
    lastGotIP = millis(); ++gotIPs;
  }
}
} // namespace

void beginLabWiFi(const char *apSsid) {
  Preferences prefs;
  if (prefs.begin("qc-wifi",true)) {
    if (prefs.getBytesLength("profile-v1")==sizeof(configured)) {
      prefs.getBytes("profile-v1",&configured,sizeof(configured));
      customProfile=validProfile(configured);
    }
    prefs.end();
  }
  if (!customProfile) {
    configured={}; configured.magic=0x57494631;
    strlcpy(configured.ssid,STA_SSID,sizeof(configured.ssid));
    strlcpy(configured.password,STA_PASSWORD,sizeof(configured.password));
    configured.checksum=profileChecksum(configured);
  }
  // Explicit web settings take precedence across reboot. Never erase other NVS.
  WiFi.persistent(false);
  WiFi.onEvent(wifiEvent);
  WiFi.mode(WIFI_AP_STA);
  // One scheduler owns reconnects; avoid core retries between five-minute slots.
  WiFi.setAutoReconnect(false);
  if (!WiFi.softAP(apSsid, AP_PASSWORD, 1, false, 1)) {
    Serial.println("ERROR: recovery AP failed to start");
  }
  WiFi.begin(configured.ssid, configured.password);
  sleepDisabled = WiFi.setSleep(false);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  lastAttempt = millis();
}

void updateLabWiFi() {
  if (settingsPending && settingsIdle && settingsIdle() && uint32_t(millis()-settingsAt)>=700) {
    settingsPending=false;
    WiFi.disconnect(false,false);
    WiFi.begin(configured.ssid,configured.password);
    WiFi.setSleep(false);
    lastAttempt=millis();
    settingsStatus="Menghubungkan; tunggu alamat IP (AP pemulihan tetap aktif)";
  }
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (connected) settingsStatus="Terhubung dan mendapat IP";
  else if (!settingsPending && millis()-lastAttempt>=20000)
    settingsStatus="Belum mendapat IP; periksa SSID/password/jangkauan/router";
  if (connected != wasConnected) {
    if (!connected) lastAttempt=millis(); // five minutes from observed link loss
    Serial.printf("STA: %s; SSID: %s; IP: %s\n", connected ? "CONNECTED" : "DISCONNECTED", WiFi.SSID().c_str(),
                  WiFi.localIP().toString().c_str());
    wasConnected = connected;
  }
  if (wifiRetryDue(millis(),lastAttempt,connected,networkQcBusy(),settingsPending)) {
    lastAttempt = millis();
    ++retries;
    // Retry only the configured remembered network; keep recovery AP running.
    // Never force disconnect while an association may still be in flight.
    lastRetryResult = esp_wifi_connect();
    settingsStatus=lastRetryResult==ESP_OK
      ? "Mencoba Wi-Fi tersimpan; percobaan berikutnya paling cepat5menit"
      : "Permintaan koneksi gagal; akan dicoba lagi dalam5menit";
  }
}

void labWiFiState(JsonObject d) {
  const bool connected = WiFi.status() == WL_CONNECTED;
  d["connected"] = connected;
  d["configuredSSID"] = configured.ssid;
  d["savedProfile"] = customProfile;
  d["settingsPending"] = settingsPending;
  d["settingsStatus"] = settingsStatus;
  d["apIP"] = WiFi.softAPIP().toString();
  d["stationStatus"] = static_cast<int>(WiFi.status());
  d["ssid"] = WiFi.SSID();
  d["ip"] = WiFi.localIP().toString();
  d["sleepDisabled"] = sleepDisabled;
  d["disconnects"] = disconnects.load();
  d["gotIPs"] = gotIPs.load();
  d["retryRequests"] = retries;
  d["lastRetryResult"] = lastRetryResult;
  d["retryIntervalMs"] = WIFI_RETRY_INTERVAL_MS;
  const uint32_t elapsed=uint32_t(millis()-lastAttempt);
  d["retryInMs"] = connected ? 0 : (elapsed>=WIFI_RETRY_INTERVAL_MS ? 0 : WIFI_RETRY_INTERVAL_MS-elapsed);
  d["lastDisconnectMs"] = lastDisconnect.load();
  d["lastGotIPMs"] = lastGotIP.load();
  d["lastDisconnectReason"] = reason.load();
  WiFiEventRecord copy[8];uint32_t count;
  portENTER_CRITICAL(&eventsMux);count=eventCount;memcpy(copy,events,sizeof(copy));portEXIT_CRITICAL(&eventsMux);
  auto history=d["disconnectHistory"].to<JsonArray>();
  for(uint32_t i=count>8?count-8:0;i<count;++i){const auto &e=copy[i%8];auto r=history.add<JsonObject>();r["uptimeMs"]=e.at;r["reason"]=e.reason;r["rssiDBm"]=e.rssi;}
  if (connected) {
    const int rssi = WiFi.RSSI();
    d["rssiDBm"] = rssi;
    d["bars"] = rssi >= -55 ? 4 : rssi >= -65 ? 3 : rssi >= -75 ? 2 : 1;
    d["channel"] = WiFi.channel();
    d["bssid"] = WiFi.BSSIDstr();
  } else { d["rssiDBm"] = nullptr; d["bars"] = 0; }
}

void beginWiFiSettings(WebServer &web, bool (*authorize)(), bool (*idle)()) {
  settingsIdle=idle;
  web.on("/api/wifi/config",HTTP_POST,[&web,authorize,idle] {
    if (!authorize()) return;
    auto respond=[&](int code,const char *message) {
      JsonDocument d; d["ok"]=code==200; d[code==200?"detail":"error"]=message;
      String body; serializeJson(d,body); web.send(code,"application/json",body);
    };
    if (!idle() || settingsPending) { respond(409,"Hentikan output/QC dan tunggu pekerjaan selesai sebelum mengganti Wi-Fi"); return; }
    if (web.arg("confirm")!="1") { respond(400,"Tinjau dan konfirmasi perubahan Wi-Fi"); return; }
    String ssid=web.arg("ssid"), password=web.arg("wifiPassword");
    const bool open=web.arg("openNetwork")=="1";
    if (ssid.isEmpty() || ssid.length()>32 || password.length()>64 ||
        (password.length()>0 && password.length()<8) || (open && password.length())) {
      respond(400,"SSID harus1..32 byte; password WPA8..63 atau64digit hex; jaringan terbuka harus dipilih eksplisit"); return;
    }
    if (password.length()==64) for(char c:password) if(!isxdigit(c)) { respond(400,"PSK64karakter harus hex"); return; }
    if (!open && password.isEmpty()) {
      if (ssid!=String(configured.ssid)) { respond(400,"Isi password untuk SSID baru atau pilih jaringan terbuka"); return; }
      password=configured.password;
    }
    SavedWiFi next={}; next.magic=0x57494631;
    ssid.toCharArray(next.ssid,sizeof(next.ssid));
    password.toCharArray(next.password,sizeof(next.password)); next.checksum=profileChecksum(next);
    Preferences prefs;
    if (!prefs.begin("qc-wifi",false)) { respond(500,"Penyimpanan Wi-Fi tidak tersedia; profil lama dipertahankan"); return; }
    SavedWiFi verify={};
    const bool saved=prefs.putBytes("profile-v1",&next,sizeof(next))==sizeof(next) &&
      prefs.getBytes("profile-v1",&verify,sizeof(verify))==sizeof(verify) && validProfile(verify) &&
      memcmp(&next,&verify,sizeof(next))==0;
    prefs.end();
    if (!saved) { respond(500,"Gagal menyimpan Wi-Fi; profil aktif dipertahankan"); return; }
    configured=next; customProfile=true; settingsPending=true; settingsAt=millis();
    settingsStatus="Tersimpan; koneksi akan diterapkan";
    respond(200,"Wi-Fi tersimpan untuk reboot berikutnya; koneksi diterapkan sesaat lagi. Tetap gunakan AP jika IP router berubah.");
  });
}
