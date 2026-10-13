# Backlog revisi Compact QC — 13 Oktober 2026

## Status saat handoff

- **Tahap: pencatatan saja. Implementasi BELUM DIMULAI.**
- Pengguna meminta menampung tugas, lalu mencatat dan push sebelum mulai. **Tunggu instruksi eksplisit `kerjakan` sebelum mengubah program, build atau menguji hardware.** Izin mencatat/push dokumen bukan izin memulai implementasi.
- Baseline source: firmware **1.2.0**, commit `218f2081f0e11f7753586a5c6f6c46b46e157855`, branch `feature/qc-operator-ota`.
- Build baseline tercatat di VALIDATION.md: `2026-10-12T17:00:08Z` / 13 Oktober 2026 00:00:08 UTC+7. Tidak ada build baru untuk backlog ini.
- Source baseline bukan bukti versi yang kini terpasang pada ESP. Pengguna melaporkan temuan pengujian; versi perangkat dan log error belum diperiksa pada tahap pencatatan.
- Upload perangkat tetap oleh tim pengguna. Tidak ada USB/OTA upload oleh Codex pada tahap ini.
- Progres: **0 dari 15 tugas aktif selesai**; satu laporan ADS1115 ditutup oleh pengguna sebagai masalah hardware yang sudah diperbaiki.
- Posisi terakhir: semua permintaan dirangkum; langkah berikutnya menunggu `kerjakan`, lalu audit source/device evidence untuk QC-01 sampai QC-16.

## Checklist dengan ID tetap

Status `TODO` = belum diimplementasikan atau diverifikasi. Jangan mengubah status hanya karena fungsi serupa ada pada v1.2.0.

| ID | Status | Tugas | Kriteria selesai |
| --- | --- | --- | --- |
| QC-01 | TODO | Hasil QC jelas pada setiap menu, termasuk OLED dan semua modul komunikasi | Badge/teks PASS atau FAIL besar, warna dan alasan hasil terlihat di container modul, konsisten dengan Ringkasan QC dan report; jangan hanya menampilkan raw JSON. Belum diuji/berjalan/tidak berlaku dibedakan dari hasil final. |
| QC-02 | TODO | Perbaiki ON/OFF LED yang dilaporkan terbalik | Perintah ON menyalakan LED, OFF mematikan LED; verifikasi mapping/polaritas aktual. Pengguna menyatakan output module sudah benar: jangan membalik DO. |
| QC-03 | TODO | DO1 dan DO2 independen | ON/OFF DO1 tidak mengubah DO2, dan sebaliknya; keduanya dapat ON bersamaan. Kontrol/konfirmasi/QC per modul tetap terpisah. Stop semua, reset sesi dan OTA tetap eksplisit mematikan semua. |
| QC-04 | TODO | Highlight ON/OFF mengikuti status, default OFF | Biru berpindah ke OFF ketika OFF diterima, kembali ke ON ketika ON diterima; startup semua output OFF. Status UI berasal dari state firmware/perintah yang berhasil, bukan warna ON permanen atau optimisme setelah request gagal. |
| QC-05 | TODO | Perbaiki kegagalan komunikasi kontrol output | Cari penyebab request output sering gagal; tampilkan error sebenarnya dan state terakhir tanpa mengklaim berhasil. Verifikasi cepat berganti ON/OFF, polling, heartbeat/session, reconnect dan akses tiap baris. Penyebab saat ini belum terbukti. |
| QC-06 | TODO | Sederhanakan konfirmasi fisik output dan latch hasil | Hanya pilihan **Sesuai** / **Tidak sesuai**. Setelah hasil QC PASS/FAIL final, tidak perlu meminta konfirmasi lagi; pengguna tetap bisa ON/OFF tanpa menghapus hasil. Reset hanya melalui mekanisme tes/sesi ulang yang jelas. |
| QC-07 | TODO | AHT10 terdeteksi tetapi gagal membaca data | Diagnosis init/status busy/timing/read length/I2C; tampilkan temperature dan humidity valid atau alasan gagal yang jelas. ACK alamat saja tidak menghasilkan data/PASS. Tetap AHT10 sesuai konfirmasi pengguna. |
| QC-08 | TODO | Tambah konfirmasi QC I2C oleh pengguna | Setelah scan, daftar alamat tetap ada; sediakan tombol PASS/FAIL user dan hasil jelas dalam menu/report. Label hasil sebagai konfirmasi pengguna atas scan, bukan bukti identitas IC atau seluruh protokol. |
| QC-09 | CLOSED — USER | ADS1115 tidak terdeteksi | **Dibatalkan sebagai perbaikan firmware:** pengguna memastikan masalah hardware dan sudah OK. Jangan membuka kembali tanpa bukti baru. |
| QC-10 | TODO | QC akurasi 4–20 mA dengan lima titik | Container baru, lima target uji, nilai hasil pembacaan/capture dan konfirmasi Sesuai/Tidak sesuai per titik. Kelima titik sesuai -> PASS; titik tidak sesuai -> FAIL; titik belum selesai tidak PASS. Kalibrasi empat titik yang sudah berhasil tetap dipertahankan dan terpisah dari QC. |
| QC-11 | TODO | Modbus RS485 master dengan dua tag independen | Dua config terpisah, Request masing-masing, transaksi ke slave/register tertentu, data hasil tampil, konfirmasi Sesuai/Tidak sesuai menjadi QC. Config mengikuti rincian di bawah. Request sungguhan diperlukan; menyimpan config bukan PASS. |
| QC-12 | TODO | Skenario dan QC RS232 jelas saat mode dipilih | Jelaskan wiring laptop USB-RS232, default serial, langkah pengujian, pola paket, timeout/CRC dan kriteria PASS/FAIL. Saat memilih RS232 tampilkan UI QC yang relevan; jangan hanya form Modbus. Pertahankan integrasi penguji laptop lokal yang telah dibuat. |
| QC-13 | TODO | Pisahkan menu RS485/RS232, komunikasi dan peripheral SPI | Menu khusus RS485/RS232. Peripheral SPI memuat SD card dan pemeriksaan ETH melalui SPI. Komunikasi memuat jaringan ETH/Wi-Fi, MQTT serta tes/config radio. Bedakan pemeriksaan chip ETH dari konektivitas jaringan ETH. |
| QC-14 | TODO | QC jaringan Wi-Fi/ETH dan config MQTT | Setting jaringan/broker MQTT, IP aktual, status koneksi dan hasil QC yang jelas. Koneksi jaringan/IP dan keberhasilan koneksi MQTT harus terbukti, bukan sekadar config tersimpan atau PHY link ON. Secret tidak ikut report/log/push. |
| QC-15 | TODO | Hasil radio jelas, baca data dan konfigurasi E70 | Pengguna menyebut LoRa; hardware terkonfirmasi E70-915NW14S. Tampilkan hasil komunikasi dan konfigurasi dengan jelas, data terbaca dan hasil config/readback; jangan hanya raw JSON. Rincian write/volatile/NVM perlu ditentukan sebelum implementasi, tidak otomatis menulis NVM. |
| QC-16 | TODO | Diagnosis fitur identitas EEPROM / PD Sync masih error | Screenshot terakhir memuat keduanya; perlakukan keduanya sebagai perlu diagnosis sampai diketahui fitur mana yang gagal. Perbaiki Write+verify/Read EEPROM dan/atau Request PDO sesuai bukti. Tidak ada pesan error lengkap yang diberikan; jangan menganggap record kosong atau sourcePDO kosong sebagai akar masalah. |

## Rincian dan batas yang harus dibawa ke implementasi

### Output dan konfirmasi

- Fakta source baseline: README v1.2.0 mendokumentasikan hanya satu output ON sekaligus. Ini bertentangan dengan permintaan baru DO1/DO2 independen; revisi perlu mengubah model state, bukan sekadar tombol frontend.
- Fakta keputusan sebelumnya: DO1/DO2 aktif LOW; LED MCP aktif HIGH. Laporan baru mengatakan ON/OFF LED terbalik. **Polaritas LED aktual sekarang belum dipastikan**; periksa pemetaan/perintah dan pengamatan sebelum mengganti konstanta.
- Jangan menonaktifkan heartbeat, timeout, I2C fault lock atau stop OTA untuk menyembunyikan error komunikasi. Terapkan pengamanan pada state setiap output.
- Hasil QC final tidak boleh hilang karena perintah ON/OFF lanjutan. Tentukan reset tes secara eksplisit dan tampilkan kapan hasil lama berlaku.
- OLED perlu membedakan init berhasil dari hasil observasi user; tulisan `OLED init PASS` bukan pengganti hasil QC visual.

### Analog

- Pengguna menyatakan ADC dan kalibrasi empat titik **sudah berhasil**. Gain/offset aktif dari NVS tetap dipakai.
- Lima titik QC adalah uji setelah kalibrasi, bukan mengganti fit empat titik menjadi lima titik.
- Nilai default lima target, toleransi, cara capture dan reset hasil belum ditentukan pengguna. Pilih dan dokumentasikan saat `kerjakan`; jangan mengklaim telah disetujui sekarang.
- Simpan evidence tiap titik (target, nilai terbaca, konfirmasi, waktu/sesi dan kalibrasi yang digunakan) dalam report. Perubahan kalibrasi harus membatalkan evidence QC lama yang tidak lagi sesuai.

### RS485 dua tag

- Pengguna kini meminta transaksi Modbus, menggantikan penundaan Modbus sebelumnya; implementasi **tetap belum boleh dimulai** sebelum `kerjakan`.
- Config link: mode, baud, parity dan slave ID; contoh baseline 9600/N/slave1, bukan universal.
- Dua tag dikonfigurasi independen: nama tag (contoh UI max10 karakter), function **Read Coils (01), Read Discrete Inputs (02), Read Holding Registers (03), Read Input Registers (04)**, datatype (contoh Unsigned; opsi lain ditentukan dengan representasi yang didukung), alamat dan length (contoh max2).
- Sebutkan alamat register sebagai offset protokol atau notasi vendor, agar tidak ambigu 0 vs 40001. Bedakan bit coils/discrete dari register 16-bit. Endianness/word order untuk nilai multi-register belum diketahui.
- Device berperan master; arah TX/RX RS485 otomatis tanpa DE/RE sudah dikonfirmasi. Route MCP2A2 LOW untuk RS485 / HIGH untuk RS232.
- Request tiap tag menampilkan nilai mentah dan hasil interpretasi beserta error timeout/CRC/exception. Response invalid tidak boleh dikonfirmasi PASS sebagai data valid.
- Hasil per tag wajib jelas. Aturan agregasi QC RS485 (misalnya kedua tag selesai/sesuai) perlu dinyatakan saat implementasi.

### RS232 dan radio

- RS232 adalah interface listrik; tidak memiliki satu protokol/baud QC universal. Baseline custom binary laptop 9600/8N1/100 paket adalah skenario proyek, bukan sertifikasi standar RS232. Jangan menyamakan adapter USB-TTL atau echo lokal dengan pengujian transceiver.
- Pertahankan backend laptop lokal terpisah dan jelaskan proses Start/hasil device vs hasil laptop, wiring TX/RX/GND dan kecocokan adapter.
- E70 baseline terkonfirmasi CC1310/15.4 star, **bukan LoRa**. SF/BW/coding rate/CRC LoRa tetap tidak didukung; jangan menandainya PASS atau mengarang perintah.
- Pengguna kini meminta config radio, berbeda dari v1.2.0 read-only. Jenis perubahan yang dapat dilakukan, prosedur aman, target produksi dan persistensi belum dipastikan. Readback/konfigurasi saja tidak membuktikan RF link antarmodul.
- QC otomatis: skenario selesai dengan evidence valid -> PASS; kegagalan skenario yang terkonfirmasi -> FAIL. Belum diuji, dibatalkan, unsupported atau tanpa evidence jangan diubah menjadi PASS. Tampilkan alasan kegagalan/ketidaklengkapan di container masing-masing.

### Jaringan, I2C, EEPROM dan PD

- ETH memiliki dua lapisan: peripheral SPI/controller/link dan komunikasi jaringan/IP/MQTT. Hindari dua hasil PASS yang mengklaim hal sama tanpa tes berbeda.
- Wi-Fi lab default HuaweiJIN / AP12345678 tetap baseline. Setting jaringan baru/MQTT jangan membocorkan credential ke public repo; pertahankan AP pemulihan dan NVS lain.
- Granularitas tombol PASS/FAIL I2C (bus/scan keseluruhan vs tiap baris) belum dinyatakan. Permintaan saat ini adalah QC I2C oleh pengguna; scan tetap daftar alamat, tidak mengirim command model yang ditebak.
- EEPROM TOP `TMC_T_V2_R1` / BOTTOM `TMC_B_V3_R0`, C7559/M24128, QC region0x3F80–0x3FFF. Periksa write/readback, CRC dan WC; jangan menghapus foreign/production data untuk memaksa write berhasil.
- PD hanya M0. M1 STUSB4500 unpopulated tetap N-A. Periksa transaksi I2C, sink PDO/source capabilities/negosiasi dan adapter; target12V perlu compatible fixed PDO. Actual VBUS perlu pengukuran, bukan inferensi dari angka request. Tidak otomatis program NVM STUSB.

## Bukti dan unknowns

- Laporan dan screenshot berasal dari chat pengguna setelah push v1.2.0. Ambient browser menunjuk `http://127.0.0.1:8769/#comms`; ini **bukan bukti** screenshot berasal dari hardware atau localhost terhubung ke ESP.
- Sebelum diagnosis, bedakan preview mock, web laptop8770 dan live dashboard ESP; periksa URL/version/buildUTC/API/error yang sebenarnya. Jangan menyimpulkan AHT/PD/EEPROM rusak dari placeholder preview.
- Screenshot dipakai sebagai referensi tampilan/keluhan; tidak dimasukkan sebagai asset repo publik. Deskripsi dan kriteria di atas cukup untuk melanjutkan pekerjaan tanpa file Temp laptop.
- Unknowns: versi perangkat aktual, pesan error output/EEPROM/PD lengkap, model sensor fisik yang terbaca, broker MQTT/credential, target radio produksi, target lima titik QC, datatype/word order Modbus.

## Urutan kerja setelah izin memulai

1. Audit branch/status/remote dan reread aturan; baca CURRENT worklog serta dua update terbaru, kemudian backlog ini.
2. Bedakan mock vs device; kumpulkan error nyata. Mulai QC-01/02/03/04/05/06 karena fondasi state/perintah memengaruhi banyak tes.
3. QC-07/08/10/16: sensor, I2C, analog QC dan EEPROM/PD sesuai evidence. QC-09 tetap closed.
4. QC-11/12/13/14/15: Modbus dua tag, RS232, menu SPI/jaringan/MQTT/radio.
5. Integrasikan badge/menu/report/OLED, tes regresi, build dengan versi dan UTC baru; bedakan software PASS dari bench acceptance.
6. Perbarui checklist, changelog/handoff/worklog; commit/push feature branch. Tidak merge main/develop atau upload device otomatis.

Urutan ini usulan untuk kelanjutan, bukan bukti pekerjaan sudah dimulai. Keputusan baru pengguna dapat mengubah urutan/scope.

## Protokol checkpoint

- Saat mulai sebuah ID, ubah status ke `IN PROGRESS`, isi checkpoint dan CURRENT worklog.
- Saat berhenti, tulis file/baris/modul yang sudah berubah, tes yang dijalankan, error terakhir, commit yang sudah dipush, perubahan lokal yang belum dipush dan **langkah konkret berikutnya**.
- Saat selesai, checklist menjadi `[x]`/`DONE` hanya setelah kriteria diuji; tulis batas verifikasi hardware. Simpan timestamp berlabel UTC/UTC+7.
- Pertahankan ID task dan history; koreksi scope ditambahkan sebagai update, bukan menghapus permintaan yang belum ditangani.

## Checkpoint terbaru — 2026-10-13 13:31 UTC+7

- Yang dikerjakan: pencatatan backlog dan aturan resume; tidak ada source firmware/UI/backend yang diubah.
- Yang belum dikerjakan: semua15 ID aktif pada checklist.
- Versi firmware tetap1.2.0; tidak ada build, tes hardware atau upload baru.
- Baseline Git sudah diverifikasi lokal/remote `218f208`. Commit dokumen dapat dilihat dari history file ini dan branch worklog.
- Aturan dibaca: Firmware_AI_Vault_Home, Rules_Nama_dan_Folder_Workspace, Rules_Firmware_Standar, Rules_GitHub_Gitflow, Rules_Kredensial_WiFi, WiFi_HuaweiJIN, Rules_Versioning dan Rules_Change_Notifications. File lengkap berada di vault workspace, daftar path di README.
- Tahap dokumentasi tidak mengubah profil HuaweiJIN/AP12345678 atau NVS. Mekanisme lama tetap selective migration driver STA; NVS lain dipertahankan.
- Langkah berikutnya: **menunggu pengguna mengatakan `kerjakan`**.
