Latest deployment1.18.1/CURRENT V2.72 includes these1.18.0 features plus AP send repair.
See AP_RESPONSE_REPAIR_1_18_1.md for actual build/hash/OTA/runtime evidence and limits.

# Auto QC terpadu dan Wi-Fi tersimpan — 1.18.0

## Previous

1.17.3 / build 2026-10-17T04:23:32Z. Pengamatan 12 jam sebelumnya telah selesai,
lihat NETWORK_OBSERVATION_2026-10-17.md; bukan hasil uji versi 1.18.0.

## Changes

- Sidebar memiliki kelompok QC Auto/Manual, Persiapan/Diagnostik, Hasil/Riwayat,
  Pengaturan/Pemeliharaan. Auto QC memuat panel input, OLED, output, analog, I2C,
  identitas/PD, serial, SPI dan jaringan di halaman yang sama. Elemen asli
  dipindah kembali ketika berpindah ke Manual; ID kontrol tidak diduplikasi.
- Mulai tetap ditinjau eksplisit dengan fixture aman. LED/DO dinyalakan dan
  dimatikan satu per satu otomatis, menunggu observasi ON dan OFF pengguna.
  Stop, heartbeat hilang, halaman tersembunyi atau konteks berubah membatalkan.
- EEPROM memakai challenge baru pada slot QC cadangan 0x3F80–0x3FFF, membaca
  balik, memulihkan byte awal dan memverifikasi kedua slot. Slot identitas valid
  terbaru tidak ditulis. Data asing ditolak. Gagal pemulihan adalah FAIL dan
  perlu pemeriksaan sebelum retry. Jangan memutus daya selama write/restore.
- I2C scan menentukan PASS/FAIL dari semua alamat wajib yang ACK. Mode Teknisi,
  Konfigurasi QC: Muat kriteria, isi hex dipisah koma, tinjau dan simpan. Kosong
  atau belum dikonfigurasi menghasilkan UNVERIFIED; API konfirmasi manual tidak
  dapat mengubahnya menjadi PASS. Kriteria disimpan di qc-i2c/criteria-v1 dengan
  CRC/readback dan revisi, terpisah profil/kalibrasi. Perubahan hanya retest I2C.
- RS485 Auto membaca dua tag bergantian dengan jeda 1 detik sesudah respons atau
  timeout; batas request mengikuti konfigurasi tag. Lease browser 3,5 detik,
  Stop/new session/OTA membatalkan. CRC/timeout gagal tidak menghasilkan PASS.
  Nilai memerlukan konfirmasi pengguna terhadap sumber; FAIL observasi
  menghentikan polling. Default baru Tag2 Signed, offset 1, length 1; preset
  pengguna yang sudah tersimpan dipertahankan. Hasil final dilewati pada Mulai.
- Wi-Fi memiliki **Simpan & hubungkan Wi-Fi**, tersedia tanpa profil QC. Endpoint
  /api/wifi/config memerlukan token, review, output OFF dan worker idle.
  Satu blob qc-wifi/profile-v1 disimpan dan dibaca balik sebelum diterapkan.
  Boot memakai profil tersimpan; default HuaweiJIN berlaku bila belum ada.
  Password tidak diekspor dalam health/state/report atau pesan review.
  Password kosong mempertahankan nilai SSID yang sama; SSID baru memerlukan
  password, atau pilihan jaringan terbuka eksplisit. AP pemulihan tetap aktif.
- OLED bergantian menampilkan status STA, IP saat terhubung, dan IP AP. Saat
  terputus, IP STA ditampilkan --, bukan alamat lama sebagai koneksi aktif.

## Unchanged

M0/M1, pinmap classic ESP32, partisi legacy 16 MB/slot aplikasi 2 MB, polaritas
DO HIGH/LED LOW, MCP1B7/TPL, namespace kalibrasi dan checkpoint lama dipertahankan.
OLED dan hasil analog tetap memerlukan observasi/stimulus nyata; input tetap
dua siklus HIGH–LOW–HIGH debounced. PD tetap persetujuan terpisah; M1 PD OFF.
RS232 ditunda oleh owner. Radio pasangan hanya rencana: RADIO_PAIR_PLAN.md.
Alur radio parameter/RTC terdahulu tidak diaktifkan otomatis ketika boot.

## Tests

Node UI regression, firmware compile termasuk fault injection compile-time
EEPROM write/read/restore, Python protocol/archive/metadata, portal packaging.
Hasil aktual build dan OTA dicatat di CURRENT/VALIDATION setelah verifikasi.
Tidak membuat physical PASS dari mock/browser/build.

## Limitations

Kriteria alamat I2C membuktikan kehadiran, bukan fungsi sensor. EEPROM write
terputus listrik bisa merusak slot cadangan yang sedang ditulis; identitas valid
terbaru sengaja tidak disentuh. Auto QC tetap memerlukan browser terbuka dan
fixture/operator. Mode Teknisi adalah presentasi, bukan akun otorisasi terpisah.
Setting Wi-Fi AP memakai HTTP lokal; tidak mengklaim transport terenkripsi.
Password baru harus dimasukkan oleh owner; tidak diasumsikan sama dengan router.

Fakta diagnosis 19 Oktober: unit CA478 / 1.17.3 dapat diakses lewat AP4.1,
STA belum mendapat IP, tidak ada event disconnect/gotIP sejak boot; alamat
router lama79 timeout. Kode lama hanya menerima kredensial untuk worker QC
sementara dan boot selalu kembali ke profil lab. Ini menjelaskan pengaturan
yang tidak bertahan, belum membuktikan sebab kegagalan asosiasi router saat ini.
Tidak ada scanning LAN atau tebakan perubahan router.

## Upgrade

Build hanya Build-Firmware.ps1, source dipush sebelum app-only OTA. Verifikasi
serial CA478, chip ESP32-D0WD-V3, baseline 1.17.3/build04:23:32Z/hash, output OFF
dan tidak ada worker/sampling aktif. Backup full state/checkpoint lokal di folder
ignored; tidak menulis token/fullstate ke Git. App-only OTA tidak erase NVS.
Checkpoint bukti versi lama dipertahankan sebagai historis, tidak di-resume
otomatis ke alur/kriteria baru. Pilih sesi baru hanya atas tindakan operator.
Sesudah restart cek versi/build/hash, profil/kalibrasi/identitas/checkpoint lama.
