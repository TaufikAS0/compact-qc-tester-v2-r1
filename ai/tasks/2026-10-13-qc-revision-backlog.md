## Final continuation - v1.6.1 / 2026-10-14

Revisions REV-01..05 are tracked in04_Dokumen/QC_PROFILE_REVISION.md. Software implemented/build/tests PASS; authorized OTA192.168.1.31 installed1.6.1/build11:45:51Z, exact MD5/serial match, calibration/profile persistence verified. AHT now live valid. EEPROM Read fixed but Write remains rejected Wire4; M1 ignores EEPROM per user fallback. M1 also excludes Radio configuration/RS232/PD. Editable M2 example saved with both DO OFF; browser toggles/session/CSV verified. UI click queue/Stop/stable DOM and profile-transition fix included. No general serial-web work or automatic main/develop merge. See CURRENT for final publication state; physical output/accuracy/EEPROM-WC followup remains explicit.

## Resume audit - 2026-10-14 17:49 UTC+7

- Open project: Compact QC only. Clean local feature/qc-operator-ota was 12 commits behind; fetched and fast-forwarded to the exact remote checkpoint add83d971df4e2de5eb994beeae4a735058e1145. Remote main remains35733ba; no remote develop exists. No main/develop merge.
- Source truth: firmware_version.h1.5.1; relay_profile.h GPIO32/33 active HIGH (ON=HIGH/OFF=LOW), LED MCP active LOW. User reports failed relay as hardware; no fresh physical evidence collected.
- QC-02/04/05: v1.5.1 SOFTWARE PASS per checkpoint and current regression run; BENCH PENDING. Earlier table/status and device readings below are historical. The obsolete instruction not to invert DO is superseded by the user's both-HIGH correction. QC-16 remains BENCH FOLLOWUP; do not reopen QC-09 without new evidence.
- This continuation corrects contradictory operator/install polarity, version and serial-deferral text. Firmware source/version stays1.5.1. Node UI PASS, Python10 PASS, portal2 PASS. No new firmware compile, device contact or upload.
- Local build/compact_qc_tester.ino.bin is an older1120144-byte app, SHA256231291610a98957b526c86115ccc9534da0ec1e35febba30538ba2e87ddb55c5, with no firmware-manifest.json or build_metadata.h. It is not the gated v1.5.1 app and must not be relabelled or installed as such.
- Vault paths referenced by AGENTS are absent here; targeted searches under D:/Home Work, C:/Users/taufi/Documents and C:/Users/taufi/.codex found no Rules_Versioning.md or Rules_Change_Notifications.md. Resolve/read these before a fresh build. The checkpoint's v1.5.1 build evidence is retained as historical evidence, not reproduced locally.
- Next: locate the mandatory vault rules; if a fresh binary is needed, use Build-Firmware.ps1 and retain exact manifest/hash. Team verifies installed version/buildUTC, repaired relay, GPIO vs physical DO behavior and Stop/timeout; EEPROM/PD followup requires fresh errors/observations. Publication state is recorded in CURRENT/worklog and Git history.

## Reopened output report � 2026-10-14

QC-02/QC-04/QC-05 reopened for DO1 always physically OFF (user confirmed) and DO2 inverted on installed1.5.0. v1.5.1 fixes proven startup sequencing bug and corrects BOTH DO1/DO2 toHIGH; user identifies the failed relay as hardware. Final1.5.1 build/tests PASS; see latest branch worklog. Physical acceptance pending team install. Earlier DONE SOFTWARE does not certify physical output behavior.

# Backlog revisi Compact QC — 13 Oktober 2026

## Status saat handoff

- **Implementasi source1.5.0 selesai/diverifikasi software; physical acceptance belum dilakukan.** User melanjutkan setelah checkpoint13:52, tanpa deadline baru.
-14 dari15 tugas aktif DONE SOFTWARE. QC-16 DIAGNOSED/BENCH FOLLOWUP; ADS QC-09 CLOSED USER. Jangan menyamakan software DONE dengan hardware PASS.
- Branch feature/qc-operator-ota. Source checkpoints1.3.0 (468e781),1.4.0 (b36f3d6); final1.5.0 sudah dipush sebagai fa0624ea3d979f2a87ff3db118a46c4ad6ca0387 dan SHA remote diverifikasi pada2026-10-14. Tidak merge main/develop.
- Final build **2026-10-13T09:34:41Z /2026-10-13 16:34:41 UTC+7**, app1260416 bytes, metadata/partition gates PASS; rincian lengkap VALIDATION.md.
- Actual device192.168.1.55 masih1.2.0/build2026-10-12T21:08:39Z. No upload; tim pengguna menginstal firmware lalu menjalankan QC_OPERATOR_GUIDE.md.
- QC-16 fakta: EEPROM57 merespons tetapi belum ada identitas QC valid; slot kosong/foreign/corrupt belum diketahui pada firmware lama. PD28 tidak merespons scan pada M0; periksa populasi/power/bus atau pilihM1 jika memang unpopulated. Source menambahkan diagnosis/read raw/protocol retry; tidak mengarang keberhasilan tegangan.
- Next: team install, physical tests pada guide dan feedback untuk hardware followup. Tidak ada tugas source lain yang sengaja ditinggalkan tanpa implementasi.

## Checklist dengan ID tetap

Status `TODO` = belum diimplementasikan atau diverifikasi. Jangan mengubah status hanya karena fungsi serupa ada pada v1.2.0.

| ID | Status | Tugas | Kriteria selesai |
| --- | --- | --- | --- |
| QC-01 | DONE — SOFTWARE | Hasil QC jelas pada setiap menu, termasuk OLED dan semua modul komunikasi | Badge/teks PASS atau FAIL besar, warna dan alasan hasil terlihat di container modul, konsisten dengan Ringkasan QC dan report; jangan hanya menampilkan raw JSON. Belum diuji/berjalan/tidak berlaku dibedakan dari hasil final. |
| QC-02 | DONE — SOFTWARE | Perbaiki ON/OFF LED yang dilaporkan terbalik | Perintah ON menyalakan LED, OFF mematikan LED; verifikasi mapping/polaritas aktual. Pengguna menyatakan output module sudah benar: jangan membalik DO. |
| QC-03 | DONE — SOFTWARE | DO1 dan DO2 independen | ON/OFF DO1 tidak mengubah DO2, dan sebaliknya; keduanya dapat ON bersamaan. Kontrol/konfirmasi/QC per modul tetap terpisah. Stop semua, reset sesi dan OTA tetap eksplisit mematikan semua. |
| QC-04 | DONE — SOFTWARE | Highlight ON/OFF mengikuti status, default OFF | Biru berpindah ke OFF ketika OFF diterima, kembali ke ON ketika ON diterima; startup semua output OFF. Status UI berasal dari state firmware/perintah yang berhasil, bukan warna ON permanen atau optimisme setelah request gagal. |
| QC-05 | DONE — SOFTWARE | Perbaiki kegagalan komunikasi kontrol output | Cari penyebab request output sering gagal; tampilkan error sebenarnya dan state terakhir tanpa mengklaim berhasil. Verifikasi cepat berganti ON/OFF, polling, heartbeat/session, reconnect dan akses tiap baris. Penyebab saat ini belum terbukti. |
| QC-06 | DONE — SOFTWARE | Sederhanakan konfirmasi fisik output dan latch hasil | Hanya pilihan **Sesuai** / **Tidak sesuai**. Setelah hasil QC PASS/FAIL final, tidak perlu meminta konfirmasi lagi; pengguna tetap bisa ON/OFF tanpa menghapus hasil. Reset hanya melalui mekanisme tes/sesi ulang yang jelas. |
| QC-07 | DONE — SOFTWARE | AHT10 terdeteksi tetapi gagal membaca data | Diagnosis init/status busy/timing/read length/I2C; tampilkan temperature dan humidity valid atau alasan gagal yang jelas. ACK alamat saja tidak menghasilkan data/PASS. Tetap AHT10 sesuai konfirmasi pengguna. |
| QC-08 | DONE — SOFTWARE | Tambah konfirmasi QC I2C oleh pengguna | Setelah scan, daftar alamat tetap ada; sediakan tombol PASS/FAIL user dan hasil jelas dalam menu/report. Label hasil sebagai konfirmasi pengguna atas scan, bukan bukti identitas IC atau seluruh protokol. |
| QC-09 | CLOSED — USER | ADS1115 tidak terdeteksi | **Dibatalkan sebagai perbaikan firmware:** pengguna memastikan masalah hardware dan sudah OK. Jangan membuka kembali tanpa bukti baru. |
| QC-10 | DONE — SOFTWARE | QC akurasi 4–20 mA dengan lima titik | Container baru, lima target uji, nilai hasil pembacaan/capture dan konfirmasi Sesuai/Tidak sesuai per titik. Kelima titik sesuai -> PASS; titik tidak sesuai -> FAIL; titik belum selesai tidak PASS. Kalibrasi empat titik yang sudah berhasil tetap dipertahankan dan terpisah dari QC. |
| QC-11 | DONE — SOFTWARE | Modbus RS485 master dengan dua tag independen | Dua config terpisah, Request masing-masing, transaksi ke slave/register tertentu, data hasil tampil, konfirmasi Sesuai/Tidak sesuai menjadi QC. Config mengikuti rincian di bawah. Request sungguhan diperlukan; menyimpan config bukan PASS. |
| QC-12 | DONE — SOFTWARE | Skenario dan QC RS232 jelas saat mode dipilih | Jelaskan wiring laptop USB-RS232, default serial, langkah pengujian, pola paket, timeout/CRC dan kriteria PASS/FAIL. Saat memilih RS232 tampilkan UI QC yang relevan; jangan hanya form Modbus. Pertahankan integrasi penguji laptop lokal yang telah dibuat. |
| QC-13 | DONE — SOFTWARE | Pisahkan menu RS485/RS232, komunikasi dan peripheral SPI | Menu khusus RS485/RS232. Peripheral SPI memuat SD card dan pemeriksaan ETH melalui SPI. Komunikasi memuat jaringan ETH/Wi-Fi, MQTT serta tes/config radio. Bedakan pemeriksaan chip ETH dari konektivitas jaringan ETH. |
| QC-14 | DONE — SOFTWARE | QC jaringan Wi-Fi/ETH dan config MQTT | Setting jaringan/broker MQTT, IP aktual, status koneksi dan hasil QC yang jelas. Koneksi jaringan/IP dan keberhasilan koneksi MQTT harus terbukti, bukan sekadar config tersimpan atau PHY link ON. Secret tidak ikut report/log/push. |
| QC-15 | DONE — SOFTWARE | Hasil radio jelas, baca data dan konfigurasi E70 | Pengguna menyebut LoRa; hardware terkonfirmasi E70-915NW14S. Tampilkan hasil komunikasi dan konfigurasi dengan jelas, data terbaca dan hasil config/readback; jangan hanya raw JSON. Rincian write/volatile/NVM perlu ditentukan sebelum implementasi, tidak otomatis menulis NVM. |
| QC-16 | DIAGNOSED — BENCH FOLLOWUP | Diagnosis fitur identitas EEPROM / PD Sync masih error | Screenshot terakhir memuat keduanya; perlakukan keduanya sebagai perlu diagnosis sampai diketahui fitur mana yang gagal. Perbaiki Write+verify/Read EEPROM dan/atau Request PDO sesuai bukti. Tidak ada pesan error lengkap yang diberikan; jangan menganggap record kosong atau sourcePDO kosong sebagai akar masalah. |

## Rincian dan batas yang harus dibawa ke implementasi

### Output dan konfirmasi

- Fakta source baseline: README v1.2.0 mendokumentasikan hanya satu output ON sekaligus. Ini bertentangan dengan permintaan baru DO1/DO2 independen; revisi perlu mengubah model state, bukan sekadar tombol frontend.
- Keputusan terbaru pengguna2026-10-13: DO1/DO2 tetap aktif LOW; **LED MCP aktif LOW**, menggantikan jawaban HIGH sebelumnya. Source telah diperbaiki; bench acceptance belum dilakukan.
- Jangan menonaktifkan heartbeat, timeout, I2C fault lock atau stop OTA untuk menyembunyikan error komunikasi. Terapkan pengamanan pada state setiap output.
- Hasil QC final tidak boleh hilang karena perintah ON/OFF lanjutan. Tentukan reset tes secara eksplisit dan tampilkan kapan hasil lama berlaku.
- OLED perlu membedakan init berhasil dari hasil observasi user; tulisan `OLED init PASS` bukan pengganti hasil QC visual.

### Analog

- Pengguna menyatakan ADC dan kalibrasi empat titik **sudah berhasil**. Gain/offset aktif dari NVS tetap dipakai.
- Lima titik QC adalah uji setelah kalibrasi, bukan mengganti fit empat titik menjadi lima titik.
- Nilai default lima target, toleransi, cara capture dan reset hasil belum ditentukan pengguna. Pilih dan dokumentasikan saat `kerjakan`; jangan mengklaim telah disetujui sekarang.
- Simpan evidence tiap titik (target, nilai terbaca, konfirmasi, waktu/sesi dan kalibrasi yang digunakan) dalam report. Perubahan kalibrasi harus membatalkan evidence QC lama yang tidak lagi sesuai.

### RS485 dua tag

- Pengguna kini meminta transaksi Modbus, menggantikan penundaan Modbus sebelumnya; implementasi telah diizinkan hanya sampai deadline sesi; belum dikerjakan pada batch output ini.
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

## Checkpoint implementasi sesi13:36–13:52 UTC+7

QC-02/03/04/06/08: source selesai; tes UI lulus, compile dihentikan sebelum deadline; gate belum lulus. QC-01 badge lokal OLED/AHT/RTC/EEPROM/PD/ETH/SD/serial/radio ditambah, belum semua menu. QC-05 penyebab source terkonfirmasi: ON memanggil stop semua, heartbeat lama menolak perintah baru, poll tumpang tindih; diperbaiki tetapi kegagalan fisik/endurance belum terbukti selesai. QC-07/10/11/12/13/14/15/16 belum diimplementasikan dalam sesi ini. ADS tetap CLOSED.

Files source: app.cpp, config.h, firmware_version.h, qc_hardware.cpp, qc_math_tests.cpp, qc_session.cpp/.h, web_ui.h, output_state.h; test web-ui.test.cjs. No upload. Next: diagnosis AHT10 actual busy/init/read evidence, lalu QC lima titik dan master Modbus dua tag. Jangan menganggap data mock sebagai PASS hardware.

Final checkpoint: STOPPED before13:52 UTC+7 per user. Final1.3.0 build interrupted during library compilation; no new valid manifest/binary. Only documentation checkpoint committed/pushed;10 firmware/test files remain local (9 modified plus new output_state.h). Remote source remains1.2.0/218f208. Resume requires new user authorization and completing Build-Firmware.ps1 before source publication.

Resumed checkpoint:1.3.0 final compile/metadata/partition gates PASS (2026-10-13T08:16:19Z). QC-02/03/04/06/08 DONE SOFTWARE; physical acceptance still pending.5/15 software completed,2 partial,8 TODO. User continuation reauthorizes remaining work.

1.4.0 checkpoint: QC-07 and QC-10 software implementation/build PASS; physical acceptance pending.7/15 software DONE,2 partial,6 TODO. Read-only actual device192.168.1.55 still1.2.0; EEPROM no valid identity, STUSB28 no ACK. Next Modbus2 tags/menu/network/radio and diagnostics.

Active1.5.0 checkpoint: QC-01/05/11/12/13/14/15 implemented locally, final compile/browser gate pending. QC-16 diagnosis implemented: actual olddevice1.2.0 at192.168.1.55 has EEPROM ACK but no valid identity; PD0x28 absent. EEPROM raw-slot diagnostic and precise PD errors added; no physical write/PDO or upload. Node/Python tests PASS. Latest build PID38844; see worklog.

Final1.5.0 gate PASS. Operator guide/checkpoint documents all revised flows and remaining physical tests. No source TODO:14 software DONE,1 diagnosed hardware/bench followup,ADSclosed. Source push follows final verification; see branch history for commit.
