# Compact QC Tester v0.5.0

Firmware QC lab **ESP32 klasik / 16 MB** untuk TOP `TMC_T_V2_R1` + BOTTOM `TMC_B_V3_R0`. Pengguna mengonfirmasi 2026-10-12 bahwa kedua PCB tetap memakai [pinlist V2_R1](https://docs.google.com/spreadsheets/d/1dNlma5VXHjytXgDrzcwWcurq-axKmhhd/edit?gid=1745042722#gid=1745042722). Jangan mengambil pinmap ESP32-S3 dari tab V3_R0 berdasarkan label bottom saja.

Source aktif `compact_qc_tester/`; versi tunggal `config.h::FW_VERSION`. Sketch tipis memanggil `app.cpp`; hardware QC, sesi/operator, Wi-Fi, OTA dan watchdog dipisahkan. Ini firmware bench QC, bukan sertifikasi kelulusan hardware.

## Fitur dan batas bukti

- OLED SSD1306 128x64 0x3C menampilkan firmware, serial, identitas PCB dan status QC setiap item bergilir. OLED sendiri memiliki tes observasi dan feedback operator. Jika OLED rusak, status tetap tersedia di web.
- DI1-4, DMM mode GPIO2, webserver mode GPIO39, display MCP1B3, boot GPIO0, LoRa ACK/AUX/LINK: pilih level ON, beri stimulus, konfirmasi setelah sinyal stabil 300ms, lalu beri OFF dan konfirmasi. Hasil disimpan **di firmware**, bukan hanya browser. Reset/EN tidak dapat dibaca sebagai GPIO: tes manual sesudah reboot dengan catatan hasil.
- LED/relay: perintah ON 1.2 detik, konfirmasi ON dan OFF fisik oleh operator. Kontrol tetap terkunci sampai polaritas/fixture diverifikasi di `config.h`. Software tidak menentukan apakah kontak/output benar hanya dari perintah.
- Scan seluruh I2C non-reserved 0x08-0x77. ACK = DETECTED, bukan PASS. RTC baca register jam tanpa mengubah jam; MCP baca register; EEPROM baca CRC; AHT20 init/measurement/CRC setelah tipe dikonfirmasi; ADS memerlukan profil terverifikasi; PD baca ID; OLED observasi. ATtiny dan alamat asing tidak menerima command yang ditebak. Protokol ATtiny dan tipe RTC belum diketahui, jadi tes fungsi mereka memerlukan observasi/fixture tim.
- 4-20mA ADS1115 A0 single ended: model + shunt/gain/offset dikonfirmasi operator. Profil session RAM, tidak diasumsikan dari alamat 0x48. Config readback, sampel segar <1.5s dan bukan saturated, target dalam toleransi, serta feedback diperlukan. Titik 4/12/20mA tersimpan terpisah; perubahan profil menghapus kelulusan titik lama. Tegangan hasil transfer function 4-20mA dibatasi 0-3.3V secara konservatif; tetap verifikasi VDD/conditioning fisik. Jangan menghubungkan current loop langsung ke GPIO.
- Ethernet library W5100/W5200/W5500: tes controller SPI + PHY link dengan kabel. Menggunakan IP dokumentasi 192.0.2.2 tanpa DHCP untuk tes; tidak menjalankan web melalui Ethernet. PASS operator perlu catatan, dan hasil tidak mengklaim transfer paket/HTTP. Reset MCP2A1 tidak dipulsakan karena polaritas/reset circuit belum terkonfirmasi.
- SD: tulis/baca file temporary unik dan hapus. Tidak menimpa file produksi. Kedua CS SPI dinonaktifkan sesuai penggunaan.
- STUSB4500 0x28: container web fixed PDO 5-20V, langkah 50mV, arus 10-5000mA langkah 10mA. PDO1 tetap 5V; PDO2 target; readback dan soft reset renegotiation. Konfigurasi volatil RAM, **tidak menulis NVM IC**. Ini bukan PPS dan tidak menjamin adaptor menyediakan setiap tegangan; ukur output aktual sebelum feedback. RDO/state dibaca sebagai bukti protokol, bukan voltmeter.
- EEPROM C7559 = ST M24128-BWMN6TP, 16KB, 64-byte pages, alamat board 0x57. Satu EEPROM menyimpan TOP/BOTTOM dalam satu record QC. Region khusus QC **0x3F80-0x3FFF**, dua slot 64 bytes, generasi + CRC32, readback setelah write. Tulis hanya jika region kosong 0xFF atau memiliki signature QC01; menolak data asing. Region ini perlu dicadangkan tim pada firmware produksi. Record valid lama dipertahankan selama write ke slot lain; record parsial tanpa signature dikenali sebagai data asing dan tidak ditimpa otomatis.
- Serial `TMC%02X%02X%02X%02X%02X%02X` dari `esp_read_mac(...ESP_MAC_WIFI_STA)`; fallback `TMCUNKN`.
- **RS485 dan RS232 ditunda oleh pengguna 2026-10-12.** Web menyimpan baud/parity/slave/function03/04/register/count ke namespace NVS `qc-settings`, dan memilih MCP2A2: RS232 HIGH, RS485 LOW. Belum ada transaksi UART/Modbus atau PASS komunikasi. Fixture eksternal + target register diperlukan sebelum mengaktifkan tes. Pilihan jalur harus diterapkan ulang setelah reboot, tidak diasumsikan dari NVS.
- Laporan JSON mencakup hasil server, operator, serial, hardware, config, firmware/MD5, sampel dan status ditunda. Token kontrol tidak diekspor. Hasil sesi di RAM hilang saat reboot; unduh dahulu. Waktu berasal browser. Overall INCOMPLETE selama item belum PASS/ditunda; FAIL jika satu tes gagal. Observasi operator tetap dapat salah dan bukan bukti alat ukur otomatis.

## Koneksi, kredensial dan OTA

STA lab **HuaweiJIN**, AP `COMPACT-QC-xxxx` password **12345678**, URL AP http://192.168.4.1. STA IP lihat status/web. Shared STA lab disetujui oleh aturan workspace, bukan kredensial customer. `wifi_manager.cpp` membandingkan config STA yang tersimpan lalu memperbarui profil HuaweiJIN hanya jika berbeda; tidak menghapus namespace/key NVS lain. Retry STA memakai storage RAM.

OTA web `/update` memakai Digest login terpisah + token form acak sekali pakai (2 menit), abort pada error/timeout, output dihentikan sebelum update, slot aplikasi tidak aktif 2MB, restart setelah sukses. Hanya **compact_qc_tester.ino.bin**, bukan merged/bootloader/partitions. NVS dan storage dipertahankan. DONE TPL diberi layanan dalam loop dan callback flash/chunk, tidak dari task yang menyembunyikan hang main loop. Upload lambat/terputus dan timing watchdog masih memerlukan tes hardware. OTA tidak mengubah bootloader/partition table.

Keputusan pemilik 2026-10-12: build LAB publik memakai **admin / CompactLab050!** dari `lab_ota_defaults.h`. Nilai ini sengaja publik, bukan secret; jangan pakai di produksi/customer atau membuka OTA ke Internet. Tanpa header privat, build memakai default itu. `ota_secrets.h` tetap opsional untuk build privat yang BIN-nya tidak boleh didistribusikan. `Build-Firmware.ps1 -PublicLabRelease` menolak override privat. BIN hanya di Release assets, bukan Git history.

## Build dan handoff

Dependency: Arduino ESP32 **3.3.10**, ArduinoJson **7.4.3**, Adafruit GFX **1.12.6**, Adafruit SSD1306 **2.5.17**, Adafruit BusIO **1.17.4**, Ethernet **2.0.2**. ArduinoJson/GFX/SSD1306/Ethernet dipasang via Library Manager/arduino-cli. Library WiFi/WebServer/Wire/SPI/SD/Update dari core.

```powershell
./Build-Firmware.ps1
# atau setelah membuat ota_secrets.h:
arduino-cli compile --jobs 4 --fqbn esp32:esp32:esp32:FlashSize=16M --build-property upload.maximum_size=2097152 --output-dir build compact_qc_tester
node tests/web-ui.test.cjs
node tests/preview.cjs
```

Preview http://127.0.0.1:8769 mengunci output, menolak aksi hardware; bukan simulasi PASS. Tidak ada upload otomatis dalam script. Ikuti [handoff USB/OTA](04_Dokumen/TEAM_HANDOFF.md) untuk offset legacy; uploader default Arduino tidak aman untuk layout yang ada.

## Pin / output safety

I2C SDA21/SCL22. SPI18/19/23; SD CS12; Ethernet CS15. UART RX4/TX5; switch MCP2A2. DI13/14/25/26; relay32/33; LED MCP1A0-A7/B0-B2; displaybuttonB3; LoRa controlB4-B6; TPL DONE **MCP1B7**. MCP23017-E/ML dikonfirmasi pengguna. BANK0/SEQOP0 diperlukan; perubahan bit mempertahankan bit lain.

`OUTPUT_PROFILE_VERIFIED=false` dan `MCP23017_PROFILE_VERIFIED=false` tetap default. Konfirmasi polaritas lalu ubah flags untuk tes output. TPL independen tetap aktif. I2C failure mengunci output berikutnya; kondisi OFF fisik tidak dapat dijamin saat bus/power/reset gagal. GPIO12/15/2/5 adalah strap; GPIO34-39 tanpa pull internal. Gunakan fixture sesuai board.

## Aturan workspace yang dibaca

`2026 Vault Firmware Rules/00_Start_Here/Firmware_AI_Vault_Home.md`, `01_Rules/Rules_Nama_dan_Folder_Workspace.md`, `01_Rules/Rules_Firmware_Standar.md`, `01_Rules/Rules_GitHub_Gitflow.md`, `01_Rules/Rules_Kredensial_WiFi.md`, `04_Profiles/WiFi/WiFi_HuaweiJIN.md`. Arsitektur Arduino, firmware versioning dan Gitflow skills digunakan. Source tetap pada layout repo yang ada untuk menjaga workflow build; dokumen handoff di `04_Dokumen`. Publish branch feature, tanpa push/merge main/develop.

Referensi primary: [M24128 datasheet](https://www.st.com/resource/en/datasheet/m24128-bw.pdf), [STUSB4500 datasheet](https://www.st.com/resource/en/datasheet/stusb4500.pdf), [ST register/soft-reset implementation](https://github.com/usb-c/STUSB4500/tree/master/Firmware/Project), [ST programming guide](https://www.st.com/resource/en/user_manual/um2650-the-stusb4500-software-programing-guide-stmicroelectronics.pdf). Lihat VALIDATION.md untuk batas pengujian versi ini.
