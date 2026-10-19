# Rencana QC radio dengan Compact responder 24 jam

Status: perencanaan saja, sesuai keputusan owner 19 Oktober 2026. Tidak ada
implementasi responder, pengiriman RF, perubahan parameter radio, atau klaim
hardware PASS dalam rilis 1.18.0.

Fakta: kedua perangkat direncanakan Compact; pasangan akan menyala 24 jam.
Firmware repo memakai E70-915NW14S / CC1310, bukan radio LoRa. QC GET/SEND/GET
yang sudah tersedia menguji UART dan parameter modul, belum pertukaran RF.
Firmware/serial/akses IP atau USB pasangan belum ditentukan.

Asumsi yang perlu diuji: menyalakan Compact identik tidak otomatis membuat
pasangan siap membalas. Peran jaringan E70 (coordinator/node), mode pin dan
arah komunikasi harus kompatibel. Kandidatnya responder tetap sebagai
coordinator dan DUT sebagai node, tetapi pemetaan ini harus diverifikasi dari
konfigurasi serta dokumentasi E70 yang dipakai sebelum mengubah parameter.

Penilaian desain: 8/10 untuk pemeriksaan komunikasi dua arah di station.
Menerima serial saja lemah: paket lama, echo, atau unit lain dapat terlihat
sebagai sukses. Diperlukan challenge baru dan identitas tujuan yang eksplisit.
Ini tetap bukan pengukuran sensitivitas, daya pancar, jangkauan atau sertifikasi.

## Alur yang diusulkan

1. Provision satu Compact sebagai **QC responder**, dengan output selalu OFF.
   Responder aktif hanya pada peran yang disimpan eksplisit; firmware unit QC
   biasa tidak otomatis membalas semua pesan jaringan produksi.
2. Teknisi memasangkan serial responder yang diharapkan, channel/PAN, air mode,
   UART dan mode jaringan yang kompatibel. Baca konfigurasi kedua modul dahulu;
   jangan mengubah parameter radio diam-diam saat QC dimulai.
3. Pada satu halaman Auto QC, unit DUT mengirim request berversi dengan serial
   DUT, serial tujuan, nonce acak dan nomor urut. Payload dibatasi panjangnya dan
   dilindungi CRC. Tidak mengirim password, token web atau konfigurasi rahasia.
4. Responder menerima request valid, menolak serial sumber sendiri, lalu
   membalas nonce/nomor urut yang sama, serial DUT dan serial responder aktual.
   Balasan echo tanpa identitas pasangan yang tepat ditolak.
5. DUT menampilkan serial yang benar-benar diterima, umur respons, jumlah
   percobaan dan latensi. Kandidat awal: timeout 2 detik, maksimal 3 percobaan,
   satu request aktif; nilai ini perlu dikalibrasi dengan air mode aktual.
6. PASS hanya setelah respons baru dengan versi, CRC, nonce, nomor urut dan dua
   serial cocok. Respons terlambat dari percobaan lama tidak diterima. Timeout
   pada unit yang sudah dipasangkan menjadi FAIL komunikasi dengan detail,
   bukan kesimpulan otomatis bahwa pemancar rusak. Responder belum diprovision
   atau konfigurasi belum terverifikasi menjadi UNVERIFIED.

## Batas dan perlindungan

- Satu responder mendukung antrean terbatas, rate limit dan backoff acak agar
  beberapa DUT tidak bertabrakan terus. Serial tujuan menghindari PASS dari
  Compact lain yang kebetulan menyala. Broadcast discovery hanya jika diperlukan
  dan ditinjau tersendiri; bukan scanning IP/LAN.
- Nonce/CRC melawan paket lama dan kerusakan data, bukan autentikasi penyerang.
  Jika sistem perlu bukti anti-pemalsuan, gunakan MAC dengan kunci provisioning
  terpisah; jangan sebut serial sebagai bukti autentikasi.
- UART/mode pin harus punya satu pemilik; Stop, OTA, disconnect dan sesi berubah
  membatalkan request serta memulihkan mode. Pertahankan MCP1B7/TPL watchdog.
  Jangan menjalankan QC parameter AT dan pertukaran RF secara bersamaan.
- Responder 24 jam perlu watchdog, counter reboot/error, antrean bounded,
  pembatasan log/NVS (tidak menulis setiap paket), pemulihan setelah listrik
  kembali, dan indikasi status/IP di OLED. Tidak pernah mengaktifkan DO/LED test
  atau menulis RTC/EEPROM sebagai akibat request RF.
- Firmware pasangan juga harus diperbarui untuk mendukung protokol ini. Compact
  yang hanya dinyalakan dengan firmware lama belum tentu membalas serial.

## Tahap implementasi dan acceptance berikutnya

Bekukan spesifikasi frame setelah firmware/serial pasangan dan konfigurasi
E70 diketahui. Tambahkan uji parser (CRC, panjang, duplikat, echo, nonce lama,
serial salah, overflow), unit test timeout/Stop dan mock transport yang jelas
berlabel simulasi. Lalu uji dua Compact asli: normal, pasangan OFF, salah
channel/PAN, salah serial, reboot responder, beberapa DUT dan soak 24 jam.
Arsipkan build/hash kedua unit dan bukti latensi/sukses/gagal. Hardware PASS
hanya untuk pertukaran yang benar-benar dilakukan; jangkauan/daya memerlukan
fixture dan pengukuran tersendiri.
