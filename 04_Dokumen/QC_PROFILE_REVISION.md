## Final revision checkpoint - 2026-10-14

| ID | Final status | Evidence/boundary |
| --- | --- | --- |
| REV-01 | DONE - LIVE DATA | AHT restored on1.6.0/1.6.1, fresh six-byte values; no reference accuracy certification. |
| REV-02 | READ FIXED; WRITE FAILED; M1 IGNORED | Read200 succeeds for EMPTY FF. Write Wire4 at0x3F80 twice; WC hardware question outstanding. User's exclusion fallback applied, not PASS. |
| REV-03 | DONE - LIVE | M1 excludes radio config/RS232/PD/EEPROM; radio communication/RS485 retained; direct excluded API requests reject. |
| REV-04 | DONE - LIVE | Persistent36-switch/16-profile editor, M2 example, browser DO1 on/off independent of DO2, profile revision/export/session guards and OTA-reboot persistence verified. |
| REV-05 | DONE - SOFTWARE/BROWSER | Added user click-responsiveness revision; retained controls, queued requests, immediate progress/priority Stop.1.6.1 fixes sticky-disabled profile transitions. Physical load/endurance not claimed. |

Final1.6.1 installed at192.168.1.31, build11:45:51Z. Software compile/tests + actual OTA identity/MD5/NVS/AHT/UI/CSV verified; see VALIDATION for hash/details. Firmware/source publication uses feature/qc-operator-ota only. Next hardware followup: issue1 WC schematic/measurement and successful EEPROM write before re-enabling M1 EEPROM. User's general serial-web repository untouched.

## Implementation checkpoint - 2026-10-14 18:30 UTC+7

- REV-01 source implemented: AHT field/timing/probe/full-frame/bounded recovery diagnostics. Live baseline E1 NACK; hardware result pending installation.
- REV-02 source/API corrected: readable blank EEPROM is a successful Read, not a false error; exact transport/write/readback errors added. Both slots are EMPTY FF on192.168.1.31; Write acceptance pending.
- REV-03/04 implemented locally:36-switch persistent profile module, M1 defaults, M2..M999 creation (16 total), direct API/record/input/PD/background gates, profile snapshot export. Compile candidate running; no publish/upload yet.
- Added responsiveness revision: retained output DOM, immediate progress, bounded serialized click queue, Stop priority/cancellation; UI regression PASS including profiles, unchanged controls, queue and Stop. Python10/portal2 PASS.
- Mandatory rules obtained/read via issue1 and private vault aa0bb9e. Previous missing-rule blocker resolved. User explicitly authorizes OTA to192.168.1.31 and direct browser testing.
- Source1.5.1 ->1.6.0. Candidate build11:21:29Z predates later UI edits and cannot be used as final acceptance; fresh final Build-Firmware.ps1 required after source freezes. No physical PASS/upload claimed yet.

# Revisi AHT10, EEPROM dan profil QC - 2026-10-14

Permintaan pengguna terbaru menggantikan default M1 lama. Baseline source: feature/qc-operator-ota, add83d971df4e2de5eb994beeae4a735058e1145, firmware1.5.1. Dokumen ini mencatat pekerjaan yang diminta dan rancangan implementasi; bukan klaim implementasi sudah selesai.

## Scope dan status

| ID | Status | Pekerjaan |
| --- | --- | --- |
| REV-01 | DIAGNOSED SOURCE / IMPLEMENTATION PENDING | AHT10 tidak menghasilkan data; perbaiki diagnosis UI dan uji alur komunikasi sensor. |
| REV-02 | EVIDENCE PENDING | EEPROM Read/Write gagal; diagnosis tepat, perbaikan jika memungkinkan, tes jika diperbaiki. |
| REV-03 | SPEC READY / IMPLEMENTATION PENDING | M1 mengecualikan Radio configuration, RS232, STUSB4500 dan EEPROM yang belum berhasil diperbaiki. |
| REV-04 | SPEC READY / IMPLEMENTATION PENDING | Menu konfigurasi QC untuk M0/M1/M2/M3/dst dengan pilihan tes setiap profil. |

## Fakta dan informasi yang belum diketahui

- Pengguna melaporkan AHT belum menghasilkan data dan EEPROM belum berhasil Read/Write. Screenshot AHT menampilkan BELUM DIUJI, nilai kosong dan pesan umum; screenshot tidak memuat versi/buildUTC, URL atau error transaksi.
- Fakta source: aht10State menulis `hardware.detail`, UI membaca `hardware.ahtDetail`. Error sensor tidak tampil melalui field yang diharapkan. Perbaikan field diperlukan, tetapi belum membuktikan penyebab sensor gagal.
- Source memulai AHT di beginQcHardware. Inisialisasi E1 dan pengukuran AC dipakai untuk AHT10; jangan menggantinya dengan perintah AHT20 tanpa bukti model fisik.
- EEPROM source memakai alamat0x57, alamat memori16-bit, read32-byte, write16-byte dan dua slot64-byte QC01 di0x3F80-0x3FFF. Foreign bytes dilindungi. Belum ada bukti baru yang memastikan NACK, WC, buffer, wiring atau isi region sebagai akar masalah.
- Tidak ada device contact, Write, output command atau upload pada revisi ini. IP/versi/repair status aktual belum diverifikasi.
- Aturan versioning/change-notification yang diwajibkan AGENTS belum tersedia di laptop ini. [Issue bantuan #1](https://github.com/TaufikAS0/compact-qc-tester-v2-r1/issues/1) meminta isi aturan dan bukti read-only terbaru dari AI laptop utama. Firmware/build/push menunggu isi aturan wajib; diagnosis read-only dan spesifikasi dapat dilanjutkan.

## Konfigurasi QC yang akan diimplementasikan

- Menu **Konfigurasi QC**: tambah profil M2/M3/dst, pilih profil untuk diedit, ON/OFF setiap tes, Simpan, dan gunakan profil untuk sesi baru. M0 dan M1 juga editable. Digital output dapat OFF sebagai default M2 lalu ON kembali melalui menu.
- Rancangan batas MCU: maksimum16 profil tersimpan, ID M0 sampai M999, ID unik. Batas ditampilkan di UI; profile penuh/ID invalid/duplikat ditolak tanpa kehilangan data. Ini pilihan implementasi, bukan batas yang diminta pengguna.
- Persistensi di namespace NVS khusus profil QC, terpisah dari qc-adc, qc-settings dan EEPROM; schema/catalog version, validasi dan CRC. Save/readback sebelum menerapkan. Blob tidak valid tidak boleh mempersempit cakupan diam-diam; tampilkan error dan gunakan default yang didokumentasikan.
- Gunakan ID tes stabil, bukan label UI, sebagai kunci konfigurasi. Pilihan meliputi tujuh input, OLED, LED, masing-masing DO, I2C, RTC, AHT, analog, EEPROM, PD, SD, Ethernet SPI, serial/Modbus, WiFi/ETH/MQTT, radio communication dan radio configuration. Parent/child seperti RS485 dan dua tag harus punya aturan dependensi konsisten agar subtest tidak menghasilkan kewajiban tersembunyi.
- Profil M1 default: Radio configuration OFF, RS232 OFF, STUSB4500 OFF; Radio communication tetap ON. EEPROM OFF sementara kegagalan belum terselesaikan. Jika perbaikan EEPROM terbukti berhasil di perangkat, aktifkan EEPROM pada M1 dan ulangi tes. Tidak otomatis mengubah FAIL menjadi pengecualian saat tes gagal.
- OFF berarti **IGNORED BY PROFILE**, bukan PASS. Counter/overall hanya menghitung tes yang ON; ringkasan, OLED, JSON/CSV tetap mencatat tes yang OFF serta snapshot/revisi profil. Semua tes OFF menghasilkan INCOMPLETE, bukan PASS tanpa bukti.
- Save/edit atau pergantian profil aktif menghentikan output dan worker, menunggu PD selesai, lalu memulai sesi baru. Bukti sesi lama tidak boleh dipindahkan ke cakupan baru. Kalibrasi/EEPROM produksi tetap dipertahankan.
- Enforcement server-side: tes OFF ditolak pada endpoint langsung, bukan hanya tombol disabled. Background polling/auto-PASS tidak dapat menulis verdict untuk tes OFF. Stop/heartbeat/OTA dan layanan TPL tetap bekerja. Radio configuration OFF melarang Apply config, sementara read communication tetap dapat diuji. RS232 OFF tidak menonaktifkan RS485.
- Profil menambahkan konfigurasi cakupan, bukan pin remapping. DO32/33 tetap aktif HIGH; LED MCP aktif LOW; MCP/TPL bits dan partition layout dipertahankan.

## Kriteria pengujian

1. AHT: tampilkan stage/status/retry/detail asli, bukan placeholder; uji NACK, busy, frame pendek, timeout, recovery bounded dan decoding6-byte dengan simulator bus. Alur yang selalu busy harus berhenti dengan error, bukan menunggu tanpa batas. Valid data membutuhkan frame baru; software tests tidak membuktikan sensor fisik.
2. EEPROM: uji blank/valid/torn/foreign, address16-bit, page bounds, WC/readback mismatch, NACK/ACK timeout, cache invalidation dan old-valid-slot retention. Tidak menghapus foreign region atau menguji write di alamat lain. Physical Read/Write/power-cycle tetap perlu bukti nyata.
3. Profil: M1 exclusions, Radio communication tetap applicable, RS485 tetap applicable, M2 DO OFF/ON, duplicate/capacity/malformed/storage failure, reboot persistensi, semuaOFF INCOMPLETE, perubahan profil reset evidence dan direct endpoint rejection.
4. Export: first ten Longhi CSV columns tetap, nama M2/M3 tersimpan pada productType, IGNORED/status/applicable konsisten, snapshot/revisi konfigurasi memengaruhi QC evidence code. Tidak mengekspor credential/token.
5. Integrasi: Node actual-DOM regression, Python fixtures, production C++ assertions, build hanya Build-Firmware.ps1 dengan metadata/hash/legacy partition gates setelah aturan wajib dibaca. Versi baru dipilih sesuai aturan yang diminta melalui issue.

## Checkpoint kelanjutan

- Source dan versi firmware belum diubah; revisi ini baru dicatat. Enam perubahan dokumen sebelumnya tetap lokal/uncommitted; dokumen spesifikasi ini menambah perubahan lokal. Tidak ada commit/push/merge/upload.
- Next: baca balasan GitHub #1, baca aturan wajib secara penuh, lalu mulai REV-01 field diagnosis dan bounded sensor workflow, REV-02 sesuai error EEPROM, REV-03/04 catalog/profil dan enforcement. Update CURRENT/backlog setelah langkah bermakna.
