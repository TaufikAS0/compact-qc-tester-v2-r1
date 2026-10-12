#include "wifi_manager.h"
#include "config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <cstring>
#include <esp_wifi.h>

namespace {
uint32_t lastAttempt = 0;
bool wasConnected = false;
} // namespace

void beginLabWiFi(const char *apSsid) {
  // Replace any previously saved driver STA profile with the approved lab
  // profile. This changes WiFi driver configuration only, not application NVS.
  WiFi.persistent(true);
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
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  lastAttempt = millis();
}

void updateLabWiFi() {
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (connected != wasConnected) {
    Serial.printf("STA: %s; SSID: %s; IP: %s\n", connected ? "CONNECTED" : "DISCONNECTED", STA_SSID,
                  WiFi.localIP().toString().c_str());
    wasConnected = connected;
  }
  if (!connected && static_cast<uint32_t>(millis() - lastAttempt) >= 30000) {
    lastAttempt = millis();
    WiFi.reconnect();
  }
}
