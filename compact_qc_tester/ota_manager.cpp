#include "ota_manager.h"
#if __has_include("ota_secrets.h")
#include "ota_secrets.h" // PRIVATE override: that build must not be distributed.
#else
#include "lab_ota_defaults.h" // Intentional public default for shared LAB builds.
#endif
#include "tpl_watchdog.h"
#include <Arduino.h>
#include <Update.h>
#include <esp_ota_ops.h>

static_assert(sizeof(OTA_PASSWORD) >= 13,
              "Choose an OTA password of at least 12 characters in ota_secrets.h");

namespace {
WebServer *web = nullptr;
void (*stopSafe)() = nullptr;
String nonce;
uint32_t issuedAt = 0, lastChunk = 0, rebootAt = 0;
bool busy = false, accepted = false, complete = false, rebootPending = false;
String result;
constexpr uint32_t FORM_TIMEOUT_MS = 120000, UPLOAD_TIMEOUT_MS = 15000;

bool authenticated() { return web->authenticate(OTA_USERNAME, OTA_PASSWORD); }
void challenge() { web->requestAuthentication(DIGEST_AUTH, "Compact QC OTA"); }
void abortUpload(const char *why) {
  if (Update.isRunning())
    Update.abort();
  busy = false;
  complete = false;
  result = why;
}
void page() {
  if (!authenticated()) {
    challenge();
    return;
  }
  if (busy || rebootPending) {
    web->send(409, "text/plain", "OTA sedang berjalan");
    return;
  }
  char random[33];
  snprintf(random, sizeof(random), "%08lx%08lx%08lx%08lx", (unsigned long)esp_random(),
           (unsigned long)esp_random(), (unsigned long)esp_random(), (unsigned long)esp_random());
  nonce = random;
  issuedAt = millis();
  const String html =
      String("<!doctype html><html lang='id'><meta name='viewport' "
             "content='width=device-width,initial-scale=1'>") +
      "<title>Update Firmware QC</title><main style='max-width:650px;margin:40px auto;font:16px "
      "system-ui;padding:20px'>" +
      "<h1>Update firmware lewat OTA</h1><p>Pilih file aplikasi "
      "<b>compact_qc_tester.ino.bin</b>. " +
      "Gunakan build untuk Compact V2_R1 / ESP32 klasik, maksimum 2 MB. " +
      "Jangan pilih merged.bin, bootloader atau partitions.bin.</p>" +
      "<p>Perangkat akan restart setelah upload berhasil. Pertahankan catu daya selama "
      "proses.</p>" +
      "<form method='POST' enctype='multipart/form-data' action='/update?nonce=" + nonce +
      "'><input type='file' name='firmware' accept='.bin' required><button>Upload "
      "firmware</button></form>" +
      "<p>Form berlaku 2 menit. Jika kedaluwarsa, muat ulang halaman.</p><a href='/'>Kembali ke "
      "QC</a></main></html>";
  web->sendHeader("Cache-Control", "no-store");
  web->send(200, "text/html; charset=utf-8", html);
}
void upload() {
  HTTPUpload &file = web->upload();
  if (file.status == UPLOAD_FILE_START) {
    accepted = complete = false;
    result = "Upload tidak diizinkan";
    if (busy || rebootPending || !authenticated())
      return;
    if (nonce.isEmpty() || web->arg("nonce") != nonce ||
        static_cast<uint32_t>(millis() - issuedAt) > FORM_TIMEOUT_MS)
      return;
    nonce = ""; // One-use form token, independent of the QC control session.
    accepted = true;
    stopSafe();
    if (!tplWatchdogReady()) {
      abortUpload("DONE TPL tidak siap; OTA dibatalkan");
      return;
    }
    updateTplWatchdog();
    const esp_partition_t *next = esp_ota_get_next_update_partition(nullptr);
    if (!next || next->size < 1024 || !Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
      abortUpload("Tidak dapat membuka slot OTA");
      return;
    }
    busy = true;
    lastChunk = millis();
    result = "";
  } else if (file.status == UPLOAD_FILE_WRITE && accepted && busy) {
    updateTplWatchdog();
    if (!tplWatchdogReady()) {
      abortUpload("I2C DONE TPL gagal");
      return;
    }
    if (Update.write(file.buf, file.currentSize) != file.currentSize) {
      abortUpload("Firmware tidak valid, terlalu besar, atau flash gagal");
      return;
    }
    updateTplWatchdog();
    lastChunk = millis();
  } else if (file.status == UPLOAD_FILE_END && accepted && busy) {
    updateTplWatchdog();
    if (!tplWatchdogReady() || file.totalSize == 0 || !Update.end(true)) {
      abortUpload("Verifikasi firmware gagal; firmware aktif dipertahankan");
      return;
    }
    busy = false;
    complete = true;
  } else if (file.status == UPLOAD_FILE_ABORTED && accepted) {
    abortUpload("Upload terputus; firmware aktif dipertahankan");
  }
}
void finish() {
  if (!authenticated()) {
    challenge();
    return;
  }
  web->sendHeader("Cache-Control", "no-store");
  if (!accepted) {
    web->send(403, "text/plain", "Token form salah/kedaluwarsa atau upload belum dimulai");
    return;
  }
  if (!complete) {
    if (busy)
      abortUpload("Upload tidak lengkap");
    web->send(400, "text/plain; charset=utf-8", result);
    accepted = false;
    return;
  }
  web->send(200, "text/html; charset=utf-8",
            "<meta name='viewport' content='width=device-width,initial-scale=1'><h1>Upload "
            "berhasil</h1><p>Perangkat sedang restart. Tunggu sekitar 10 detik, lalu <a "
            "href='/'>buka QC</a>.</p>");
  rebootAt = millis();
  rebootPending = true;
  accepted = complete = false;
}
} // namespace

void beginWebOta(WebServer &server, void (*stopOutputs)()) {
  web = &server;
  stopSafe = stopOutputs;
  Update.onProgress([](size_t, size_t) { updateTplWatchdog(); });
  server.on("/update", HTTP_GET, page);
  server.on("/update", HTTP_POST, finish, upload);
}
void updateWebOta() {
  if (busy && static_cast<uint32_t>(millis() - lastChunk) > UPLOAD_TIMEOUT_MS) {
    abortUpload("Upload timeout");
  }
  if (rebootPending && static_cast<uint32_t>(millis() - rebootAt) >= 750)
    ESP.restart();
}
bool webOtaBusy() { return busy || rebootPending; }
