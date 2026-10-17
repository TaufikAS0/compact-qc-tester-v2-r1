#include "wifi_manager.h"
#include "config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <cstring>
#include <esp_wifi.h>
#include <atomic>
#include "network_qc.h"

namespace {
uint32_t lastAttempt = 0;
bool wasConnected = false;
std::atomic<uint32_t> disconnects{0}, gotIPs{0}, lastDisconnect{0}, lastGotIP{0}, reason{0};
uint32_t retries = 0;
int lastRetryResult = ESP_OK;
bool sleepDisabled = false;
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
  // Replace any previously saved driver STA profile with the approved lab
  // profile. This changes WiFi driver configuration only, not application NVS.
  WiFi.persistent(true);
  WiFi.onEvent(wifiEvent);
  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoReconnect(true);
  if (!WiFi.softAP(apSsid, AP_PASSWORD, 1, false, 1)) {
    Serial.println("ERROR: recovery AP failed to start");
  }
  wifi_config_t saved = {};
  const bool sameProfile = esp_wifi_get_config(WIFI_IF_STA, &saved) == ESP_OK &&
                           strncmp(reinterpret_cast<const char *>(saved.sta.ssid), STA_SSID,
                                   sizeof(saved.sta.ssid)) == 0 &&
                           strncmp(reinterpret_cast<const char *>(saved.sta.password), STA_PASSWORD,
                                   sizeof(saved.sta.password)) == 0;
  // The core already compares the AP profile before saving. Avoid rewriting
  // the STA profile too, and keep retries in RAM after the initial migration.
  if (sameProfile)
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
  WiFi.begin(STA_SSID, STA_PASSWORD);
  sleepDisabled = WiFi.setSleep(false);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  lastAttempt = millis();
}

void updateLabWiFi() {
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (connected != wasConnected) {
    Serial.printf("STA: %s; SSID: %s; IP: %s\n", connected ? "CONNECTED" : "DISCONNECTED", WiFi.SSID().c_str(),
                  WiFi.localIP().toString().c_str());
    wasConnected = connected;
  }
  if (!connected && !networkQcBusy() && static_cast<uint32_t>(millis() - lastAttempt) >= 30000 &&
      static_cast<uint32_t>(millis() - lastDisconnect.load()) >= 30000) {
    lastAttempt = millis();
    ++retries;
    // Core AutoReconnect handles ordinary loss. Fallback must not disconnect
    // first: WiFi.reconnect() can interrupt an association already in flight.
    lastRetryResult = esp_wifi_connect();
  }
}

void labWiFiState(JsonObject d) {
  const bool connected = WiFi.status() == WL_CONNECTED;
  d["connected"] = connected;
  d["ssid"] = WiFi.SSID();
  d["ip"] = WiFi.localIP().toString();
  d["sleepDisabled"] = sleepDisabled;
  d["disconnects"] = disconnects.load();
  d["gotIPs"] = gotIPs.load();
  d["retryRequests"] = retries;
  d["lastRetryResult"] = lastRetryResult;
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
