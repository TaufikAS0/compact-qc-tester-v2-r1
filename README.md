# Compact QC Tester V2_R1

Firmware bench tester dengan web lokal, profil **sementara ESP32 klasik** sesuai header ESP32-WROOM pada pinlist. Bukan firmware produksi dan bukan bukti kelulusan board. ESP32-S3 ditolak saat compile sampai tersedia mapping yang benar.

Sumber: https://docs.google.com/spreadsheets/d/1dNlma5VXHjytXgDrzcwWcurq-axKmhhd/edit?gid=1745042722#gid=1745042722 (dibaca 2026-10-09).

## Build

Arduino ESP32 core 3.3.10, ArduinoJson 7.4.3. WiFi/WebServer/Wire/SPI/SD berasal dari core.

```powershell
arduino-cli compile --fqbn esp32:esp32:esp32 compact_qc_tester
```

Tidak ada upload otomatis. Pastikan MCU, port serial, rangkaian boot, flash/partition dan watchdog sebelum flashing. GPIO12 (CS SD), GPIO15 (CS Ethernet), GPIO2 dan GPIO5 merupakan pin strap; kondisi saat reset harus diperiksa secara fisik. GPIO34–39 hanya input dan tidak memiliki resistor pull internal. Switch kanan GPIO39 tidak bisa menggerakkan LED melalui firmware.

Tes logika UI: `node tests/web-ui.test.cjs`. Preview tanpa hardware: `node tests/preview.cjs`, buka http://127.0.0.1:8769. Preview mengunci output dan menolak semua tes hardware; bukan simulasi hasil PASS.

## Penggunaan

1. Setelah upload yang disetujui, sambungkan ke hotspot `COMPACT-QC-xxxx`, password default `compactqc123` (ubah di config.h).
2. Buka http://192.168.4.1 melalui browser. Satu perangkat WiFi pada satu waktu; gunakan satu tab QC.
3. Isi nomor unit dan operator; mulai pemeriksaan baru. Tes alamat I2C, amati input, lakukan tes SD; unduh laporan JSON.
4. Firmware tidak menulis EEPROM/RTC/PD/ATtiny/LoRa. Scan hanya memeriksa alamat yang diharapkan, tidak memberi PASS fungsi.
5. Hasil disimpan di browser untuk sesi tersebut. Reload/reboot kehilangan hasil. Unduh sebelum menutup. Laporan waktu memakai jam browser, bukan RTC board.

## Kontrol output terkunci secara default

`OUTPUT_PROFILE_VERIFIED=false` memastikan relay tidak dikonfigurasi output dan MCP tidak ditulis. Kondisi fisik relay selama reset/input tetap bergantung rangkaian PCB; software tidak menjaminnya. Sebelum mengaktifkan:

- Konfirmasi MCU klasik, polaritas `RELAY_ACTIVE_HIGH` dan `LED_ACTIVE_HIGH`, serta level aman. Lepaskan beban produksi.
- Untuk LED, pastikan MCP1 **MCP23017**, alamat 0x20, BANK=0 dan SEQOP=0; set `MCP23017_PROFILE_VERIFIED=true`. Seluruh indikator yang dapat dikontrol adalah A0–A7 dan B0–B2. B3–B7 dipertahankan agar tombol, LoRa dan watchdog tidak diubah.
- Set `OUTPUT_PROFILE_VERIFIED=true`, compile ulang. Tiap output pulsa 1,2 detik. Output sebelumnya dimatikan sebelum output berikutnya; heartbeat hilang 3,5 detik menghentikan pulsa. Tombol stop mematikan output. Ini bukan interlock listrik; gunakan fixture pembatas jika diperlukan.
- I2C gagal saat output LED aktif dapat mencegah perintah OFF mencapai MCP. Firmware mengunci tes berikutnya, tetapi kondisi fisik perlu diperiksa manual. Watchdog/EN bisa mereset MCU sebelum OFF; jangan mengandalkan firmware sebagai satu-satunya pengaman.

## Cakupan dan hasil

- I2C: RTC 0x68, EEPROM 0x57, AHT 0x38, MCP1 0x20, MCP2 0x21, OLED 0x3C, ATtiny 0x77, PD 0x28, ADS 0x48.
- Input: DI 13/14/25/26, switch 2/39, LoRa status 34/35/36. Operator harus memberikan kedua kondisi dan memeriksa nilai. Input belum dinilai otomatis.
- SD: CS12, SPI 18/19/23. File acak baru, readback tepat, hapus hanya file tes yang dibuat. Gagal hapus dilaporkan dengan nama file.
- Relay 32/33 dan 11 LED MCP: pulsa dipilih manual, konfirmasi operator sesudah tes dijalankan.
- Belum ada tes fungsi Ethernet, LoRa, RS485/232, RTC, EEPROM, AHT, ADS, OLED, ATtiny, PD converter dan watchdog. DONE TPL5010 tidak digerakkan; watchdog aktif mungkin mereset tester. Model chip, protokol supervisor dan interval watchdog harus ditentukan untuk tahap berikutnya.
- Laporan keseluruhan hanya FAIL atau INCOMPLETE: versi ini **tidak menerbitkan PASS seluruh board** karena cakupan fungsi belum lengkap. Checklist dan bukti operator tidak membuktikan hasil listrik independen.

## Validasi hardware yang diperlukan

Boot dengan SD terpasang/tidak terpasang; scan setiap alamat dan uji perangkat hilang; DI/button kedua level dengan fixture; ukur kontak relay; tiap LED sesuai label; timeout/pencabutan WiFi; SD sukses/gagal/cleanup; watchdog reset; operasi berulang. Compile hanya membuktikan software dapat dibangun, bukan hardware bekerja.
