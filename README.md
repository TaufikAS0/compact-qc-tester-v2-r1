# RTC QC cepat1.11.0

Installed build2026-10-15T08:16:55Z /15:16:55 WIB at192.168.1.31, OTA/runtime hash verified. Operator: **I2C & Sensor ->pilih zona ->QC cepat: Write + Readback ->tinjau dan konfirmasi**. Firmware menulis pola waktu berbeda dan membacanya balik, lalu mengembalikan waktu laptop dan memverifikasi lagi. Kedua tahap wajib berhasil; checkpoint langsung disimpan/readback. Periksa waktu laptop dahulu.

PASS RTC produksi kini berarti **Write/Readback** sesuai keputusan pengguna. Jam berjalan60s, akurasi harian dan baterai tetap sampling terpisah, tidak otomatis PASS. Test sampling yang benar-benar FAIL/OSF/kalender tidak valid tetap memblokir PASS. Hasil QC lain tetap: actual34PASS termasuk RTC cepat, original33 dan82 kolom analog/Modbus CSV exact setelah reboot/pemulihan. [Kontrak API dan scope laporan](04_Dokumen/RTC_PORTAL_CONTRACT.md).

Simpan progres eksplisit sebelum listrik diputus; setelah reboot gunakan **Tinjau pemulihan sesi** sebelum sesi/profil baru. Output tetap OFF. Resume biasa harus build/profil/preset/kalibrasi sama; hanya ada upgrade eksplisit khusus build1.10.6 terverifikasi ke1.11.0 dengan asal build tercatat. Autosave dapat tertinggal60s; unduh laporan/checkpoint untuk arsip eksternal. Baca CURRENT/VALIDATION untuk bukti dan keterbatasan.

# Readable diagnosis charts1.9.1

Current values+units, numeric/time axes, min/mean/max and honest minimum scale ranges. Stale/offline current values are blank; graph gaps preserved. Installed build/OTA/self-check in VALIDATION and CURRENT.

# QC station source1.9.0

Adds perM test presets (tags, analog refs/tolerance, serial fixture, output timeout), readiness, schematic connection map, guided outputs, history/import on laptop, offline QR print and stale-aware mini charts. Loading presets only fills forms; actions remain explicit. Reports include preset revision/CRC. Run tools/laptop-qc/server.py and open http://127.0.0.1:8770/history to merge device archive bundles; import never opensCOM or contacts device. [Scope/limitations](04_Dokumen/QC_STATION_REVISION.md). Exact build/OTA/publication in CURRENT and VALIDATION.

# Current operator workflow - source1.8.0

Operator view shows active profile, next test and pending/fail/all filters. Switch to Technician for profile configuration, calibration and OTA; this is a presentation mode, not authentication. Session/profile changes require review before apply. Reports require operator/date and are saved as immutable snapshots in laptop browser storage, with exact JSON/CSV downloads; downloaded files are durable handoff. Browser archive may be cleared with browser data. RTC OSF is UNVERIFIED, never PASS. [Revision/procedure](04_Dokumen/OPERATOR_WEB_REVISION.md). Device/build/push truth is latest CURRENT/VALIDATION; older installed records below historical.

# Current PD controls - source1.7.0

See [PD controls](04_Dokumen/PD_CONTROLS.md): Fixed PDO presets and slider with explicit confirmation, no PPS support. Device/build/publication truth is the latest CURRENT/VALIDATION; older installed records below are historical.

# Installed and verified v1.6.1

OTA to192.168.1.31 verified version/buildUTC2026-10-14T11:45:51Z and app MD5; calibration and saved profiles preserved. AHT now produces live data. Konfigurasi QC lets you edit/add M profiles and toggle tests; M2 example saved with both DO OFF. Profile/CSV/browser transition checks passed. EEPROM Read works but Write still fails(Wire4); M1 keeps it OFF alongside Radio configuration/RS232/PD. Current installation evidence and limits are in VALIDATION.md; ignored is never PASS. Feature branch only, no automatic merge.

> Current source: **v1.6.1**. Fixes profile-change buttons remaining disabled after M1 -> M0/unselected -> selected. Installed1.6.0 already restored AHT data; EEPROM Read succeeds, Write is still rejected at0x3F80 (Wire error4), so M1 keeps EEPROM OFF. Check VALIDATION/CURRENT for final1.6.1 build/upload evidence; ignore older source-version claims below.

# Compact QC Tester v1.6.0 - current revision

Use **Konfigurasi QC** to edit M0/M1 or add M2/M3/dst (M0..M999, maximum16). Load a profile, toggle each test, then Save. Settings persist in qc-profiles/profiles-v1; select a profile after reboot. Editing the active profile starts a fresh session and stops outputs, while calibration/EEPROM are retained. Test OFF is IGNORED BY PROFILE, excluded from counters/overall and blocked by firmware endpoints; an empty test scope remains INCOMPLETE. RS485 controls both Modbus subtests. DO1/DO2 can each be excluded/re-enabled; DO GPIO32/33 remains active HIGH, LED MCP active LOW.

Default M1 excludes Radio configuration, RS232, STUSB4500 and unresolved EEPROM. Radio communication and RS485 remain enabled. Enable EEPROM after verified Write/Read; failures do not automatically disappear from the selected scope. Profiles have revision/mask snapshots in report, CSV and QC code. Configuration does not change pin mappings.

AHT exposes stage/status-valid/Wire/frame error details. It probes existing calibration before reset, waits350ms for reset/init, and reads one6-byte frame. EEPROM Read distinguishes a successful read of empty slots from bus failure. Queued clicks get immediate progress; Stop cancels pending commands; unchanged output buttons survive polling.

This current section supersedes older fixed M0/M1 procedures below. See QC_PROFILE_REVISION.md, VALIDATION and CURRENT worklog for exact build/device evidence. Approved HuaweiJIN STA, recovery AP12345678 and OTA default remain unchanged.

> Current source: v1.5.1 output startup/polarity diagnostic fix. DO1 GPIO32 and DO2 GPIO33 active HIGH per latest user confirmation. GPIO readback is not physical load confirmation. See current TEAM_HANDOFF and VALIDATION; hardware test pending.

# Compact QC Tester v1.5.1

Current workflow: [operator guide](04_Dokumen/QC_OPERATOR_GUIDE.md), [task checklist/checkpoint](ai/tasks/2026-10-13-qc-revision-backlog.md), [branch worklog](ai/branches/feature--qc-operator-ota.md). User resumed after the time limit; source1.2.0 ->1.5.0 implements output/I2C/sensor/analog/serial/network/UI revisions. Team uploads; compile/UI verification is separate from physical acceptance.

Serial laptop fixture: run `./Start-Laptop-QC.ps1`, open http://127.0.0.1:8770/ and follow [protocol/wiring](04_Dokumen/COMMUNICATION_QC.md). RS485 now also has real two-tag Modbus master requests. E70 has read validation and explicit Apply config; some persistent settings require a radio restart to become active. UART1 RX4/TX5, UART2 RX16/TX17. No ESP NVS layout/partition or pin remap.

Bench QC firmware for **classic ESP32 / 16MB**, TOP `TMC_T_V2_R1` + BOTTOM `TMC_B_V3_R0`, unchanged [V2_R1 pinlist](https://docs.google.com/spreadsheets/d/1dNlma5VXHjytXgDrzcwWcurq-axKmhhd/edit?gid=1745042722#gid=1745042722). Source is `compact_qc_tester/`; the sketch is thin and drivers/session/network/OTA/watchdog/calibration remain separate. Version truth: `firmware_version.h`. See [changes](CHANGELOG.md), [validation](VALIDATION.md), and [team installation/bench handoff](04_Dokumen/TEAM_HANDOFF.md).

## QC workflow

- **Inputs:** exactly DI1-4 (GPIO13/14/25/26) and DMM/Normal/Webserver. DMM GPIO2 and Webserver GPIO39 active HIGH. User confirmed Normal is both LOW; it is a logical position, no extra pin. Firmware automatically latches PASS after two complete HIGH–LOW–HIGH cycles (`H L H L H`) with each state stable ≥50ms. An initial static HIGH does not count as a cycle. All seven counters run together; no input dropdown/operator confirmation. Reset session clears counters; browser reload does not.
- **OLED:** dedicated visual confirmation for text visible vs blank/unreadable. SSD1306 128×64 0x3C rotates QC module/status, firmware, serial and TOP/BOTTOM; alternate screen shows the actual build UTC. Visual PASS requires successful initialization and user observation.
- **Output controls:** every LED and DO has independent ON/OFF state; both DO channels can stay ON together. Default OFF. Per-output60s since last ON command timeout, heartbeat loss3500ms/Stop/OTA stop all. DO1 GPIO32 and DO2 GPIO33 are active HIGH (ON=HIGH, OFF=LOW); LED MCP is active LOW (ON=LOW, OFF=HIGH). Blue control reflects accepted firmware state. One Sesuai/Tidak sesuai observation finalizes QC; later ON/OFF preserves verdict until a new session. Hardware latch readback is not visual acceptance.
- **4–20mA:** user confirmed ADS1115. Continuous AIN0-GND ±4.096V /128SPS, config readback; raw sample every150ms, validity age<1.5s and conservative0–3.3V. Dashboard shows raw ADC, voltage and transformed mA. Uncalibrated conversion explicitly assumes100Ω/gain1/offset0; actual conditioning is still unknown. Do not use provisional numbers for acceptance.
- **Calibration:** editable references default4/10/15/20mA. Apply calibrator current then capture16 new samples (~2.4s); reject stale/saturated/out-of-range data or sample SD>30count. Fit all four with ordinary least squares `mA=gain*raw+offset`; display gain(mA/count), offset(mA), R², max absolute residual and captured-point curve. Reject duplicate references/raw, insufficient span, R²<.995 or residual beyond configured .01–1mA(default .10). Save four points/coefficients/CRC as one NVS blob `qc-adc/cal-v1`, read back before activating. Reset capture leaves previous active calibration intact. EEPROM hardware identity stays separate. Re-check at independent currents after fitting; residual on calibration points is not independent accuracy evidence.
- **I2C:** button scans non-reserved7-bit addresses0x08–0x77 and lists ACK only. Names are pinlist hints, not model proof. User PASS/FAIL confirms the scan list only, not identity/protocol proof. Unknown/ATtiny devices receive no guessed command.
- **AHT10 / DS3231:** dedicated temperature/humidity and clock/temperature cards. AHT10 has asynchronous reset/E1/status/busy polling and bounded recovery, six-byte measurement(no AHT20 CRC), 2s interval. Valid reading is protocol/data PASS, not absolute accuracy certification. DS3231 read-only BCD clock, 12/24h decode, century bit, temperature and OSF warning; it does not set time or measure battery voltage. User confirmed these models.
- **EEPROM:** C7559 is M24128-BWMN6TP16KB at0x57. Write+verify TOP/BOTTOM, explicit Read shows stored labels/generation/CRC plus raw QC bytes and empty/corrupt/foreign slot classification, never form defaults as read data. Preserved two64-byte QC01 slots at0x3F80–0x3FFF; latest valid generation selected. Write inactive slot in16-byte chunks, ACK polling, CRC/readback; refuse foreign bytes. Failed read clears cached labels. Team must reserve this last128-byte region in production.
- **PD Sync:** STUSB4500 fixed PDO5–20V/50mV, current10–5000mA/10mA, no PPS. Runtime writes/readback all three sink PDOs individually; PDO1=5V, PDO2/3=target; active count1/2 preserves reserved bits and removes stale PDO3=20V. Soft reset asks source to renegotiate. Dashboard exposes sink/source capabilities, source RDO position, PE state, mismatch/timeout. Source must offer compatible voltage/current. Contract voltage is a protocol value, not a voltmeter. If source capabilities were missed, retry bounded Get_Source_Cap; timeout FAIL without guessing voltage. No NVM programming. Actual reported12V→20V problem remains a required bench test.
- **Ethernet / SD:** controller SPI and PHY cable/link check, not packet acceptance; W5100 UNKNOWN link remains uncertain. SPI-only check uses documentation IP192.0.2.2. Separate network worker performs DHCP/static and MQTT connection tests while main-loop watchdog stays serviced. Ethernet reset MCP2A1 untouched because polarity is unknown. SD uses unique temporary write/read/delete without overwriting production files.
- **RS485 / RS232:** laptop binary QC frames validate pattern/CRC/run/sequence/ACK. MCP2A2 HIGH RS232 /LOW RS485; UART1 RX4/TX5. Default9600/8N1/100packets. Separate loopback-only laptop web/backend, JSON/CSV exports. Two independent Modbus tags support FC01/02/03/04, offset0/count1–2, Unsigned/Signed/Float32 and ABCD/CDAB, real request/CRC validation and separate user verdicts. Configuration alone is not communication PASS.
- **Report:** fresh server JSON includes results/evidence, serial`TMC`+six uppercase STA MAC bytes, hardware, calibration, deferred statuses and firmware/build identity; control token removed. Optional operator name is metadata. Session results are RAM, lost at reboot. Overall stays INCOMPLETE for DETECTED/DATA VALID/CALIBRATED/deferred items; FAIL propagates. No full electrical certification is implied.

## Build / metadata

Pinned Arduino-ESP32 **3.3.10**; libraries ArduinoJson7.4.3, AdafruitGFX1.12.6, SSD1306 2.5.17, BusIO1.17.4, Ethernet2.0.2, PubSubClient2.8 (`arduino-cli lib install "PubSubClient@2.8"`); Wire/SPI/WiFi/WebServer/SD/Update from core. Python3 and Node needed for helper/testing. Install dependencies with Library Manager/arduino-cli; script does not install/unpin automatically.

```powershell
./Build-Firmware.ps1
node tests/web-ui.test.cjs
python tests/test_metadata.py
node tests/preview.cjs
```

Build creates ignored `build_metadata.h` with actual start time ISO8601 UTC, shared by all translation units. Header mismatch fails compile. Final app gate checks matching version/time marker in binary, classic ESP32 chip and2MB size; creates ignored `build/firmware-manifest.json` with version, buildUTC and SHA-256. Missing/stale metadata gate is tested. Build UTC is not RTC, uptime, reboot or upload time. Do not directly compile without generating/verifying metadata. No automatic device upload.

Preview at http://127.0.0.1:8769 is disconnected, rejects hardware actions and never fabricates PASS. Offline embedded dashboard CSS requires no CDN/internet. Design reference values/pins were not adopted as board facts.

## Network / OTA / watchdog

Approved shared lab STA **HuaweiJIN**, recovery AP`COMPACT-QC-xxxx` password**12345678** at192.168.4.1. Wi-Fi manager compares stored driver profile and replaces only divergent STA settings, keeps retries in RAM, does not clear unrelated NVS. Customer profiles need explicit authorization.

OTA `/update` uses Digest login, one-use form nonce2min, upload timeout15s, inactive2MB app slot and controlled restart. Output stops before OTA; DONE TPL serviced in main loop/flash callbacks. Integrated owner decision 2026-10-12: without private override use intentional public LAB `admin / CompactLab050!` from `lab_ota_defaults.h`; customer/production credentials require a private override. Keep private `ota_secrets.h`/`ota-access.txt` and private binaries out of public distribution. `-PublicLabRelease` rejects private overrides and produces the validated portal package after committing source.

Preserve legacy partition table and explicit USB offsets. Portal merged USB at offset0 is permitted for deliberate LAB reset: merged fast write resets NVS/settings; full erase deletes storage too. Component USB preserves data; never use Arduino default merged/0x10000 layout. OTA uses application `.ino.bin` only and preserves NVS/storage. Detailed offsets/checks in team handoff. TPL DONE MCP1B7, MCP23017-E/ML confirmed; bit operations preserve B3–B7. Software pulse counter does not prove physical waveform/reset. SD/Ethernet share SPI18/19/23 with CS12/15. Strap and GPIO39 external bias require bench attention.

## Workspace rules read

`2026 Vault Firmware Rules/00_Start_Here/Firmware_AI_Vault_Home.md`, `01_Rules/Rules_Nama_dan_Folder_Workspace.md`, `01_Rules/Rules_Firmware_Standar.md`, `01_Rules/Rules_GitHub_Gitflow.md`, `01_Rules/Rules_Kredensial_WiFi.md`, `04_Profiles/WiFi/WiFi_HuaweiJIN.md`, plus full `01_Rules/Rules_Versioning.md` and `01_Rules/Rules_Change_Notifications.md`. Repository AGENTS links these entry points. Source layout remains portable and unchanged; push feature branch only, no main/develop merge.

Primary references: [NIST least-squares fitting](https://www.itl.nist.gov/div898/handbook/pmd/section4/pmd431.htm), [ADS1115 datasheet](https://www.ti.com/lit/ds/symlink/ads1115.pdf), [AHT10 manufacturer datasheet](https://www.aosong.com/userfiles/files/media/AHT10%E8%A7%84%E6%A0%BC%E4%B9%A6v1_0_181213.pdf), [DS3231 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ds3231.pdf), [STUSB4500 programming guide](https://www.st.com/resource/en/user_manual/um2650-the-stusb4500-software-programing-guide-stmicroelectronics.pdf), [ST driver](https://github.com/usb-c/STUSB4500/tree/master/Firmware/Project), [M24128 datasheet](https://www.st.com/resource/en/datasheet/m24128-bw.pdf).

## Device variant and QC export

Choose **M0 or M1 before QC**. Reboot requires choosing again; variant/session/results are RAM only, not EEPROM hardware identity. Changing variant or starting a new session stops outputs and resets results/input counters/capture points, retains active calibration and EEPROM. Requests wait for any active PD negotiation to finish before reset. M0 includes PD; M1 marks STUSB4500 **NOT APPLICABLE**, excludes it from applicable/pending/fail counts, disables its request endpoint/controls and all PD register polling/writes. Generic all-address I2C ACK scan remains available. RS485/RS232 tests are implemented for both variants; the earlier deferral was superseded. Physical acceptance still requires the external fixture and actual test evidence.

QC1 code format: `QC1-M0|M1-TMC<MAC>-PASS|FAIL|INCOMPLETE-<session8hex>-<CRC32hex>`. CRC32 covers version/build UTC, selected variant, serial, session and length-prefixed ordered test names/status/details/overall. It changes when evidence changes; it is a snapshot identifier, **not a signature, certification or guaranteed unique production serial**. Production serial remains TMC + STA MAC. Live ADC/RTC readings and export date/PIC are outside this evidence checksum. Code shown by API/dashboard and included with full JSON/CSV; code may represent FAIL/INCOMPLETE.

`GET /export/qc.csv?qcDate=YYYY-MM-DD&qcPic=...&productionDate=YYYY-MM-DD` returns a fresh server snapshot, UTF-8 BOM, RFC4180 quoting and CRLF. Profile must be selected. Dates optional, validated if supplied; PIC max64 bytes. Formula-leading user values are prefixed with apostrophe for Excel safety. Dashboard provides **Export CSV** and retains **Unduh JSON**. This is an actual .csv, not a renamed XLS/XLSX.

The first ten columns match Longhi's order: `serialNumber,productName,productType,productionDate,qcStatus,qcDate,qcPic,status,deliveredAt,deliveredTo`. Append QC code/schema/session, firmware/build UTC, EEPROM labels/validity, calibration/ADC/current, applicable/pass/fail/pending/N-A counts and each test's `.status`, `.detail`, `.applicable`. productType=M0/M1. Inventory status/delivery values remain blank because this firmware has no warehouse data. INCOMPLETE is preserved, unlike Longhi's boolean PASS/FAIL fallback. Missing PD on M1 is N-A, not FAIL. Data-only/deferred results still prevent overall PASS; CRC is never a substitute for completing tests.

Reference inspected read-only: Longhi [`fix/modbus` export](https://github.com/TaufikAS0/esp32-s3-longhi-board-test/blob/26f9a1fb7fd449d076c9b41ed0f0bf091163d0fb/02_Firmware/esp32s3_hwtest/web_server.cpp) at26f9a1f and feature/longhi-modular-upgrade3a45448. Remote develop9aa0fc1 currently lacks this exporter. No pre-existing QC code format was found, so QC1 is defined here. Longhi worktree/source were not changed.

## Update1.4.0

AHT10 uses asynchronous reset/init/status and busy polling, with exact errors and bounded recovery. Five-point analog QC is separate from four-point calibration: default4/8/12/16/20mA, capture16 readings, user confirmation plus tolerance for all5 =>PASS. Calibration fingerprint changes invalidate old QC evidence. Tests are software; team still uploads/physically verifies.

## Update1.5.0

Separate serial, peripheral SPI and network/radio menus; local result badges; worker WiFi/ETH/MQTT tests; explicit E70 AT writes/readback; EEPROM raw-slot/PD diagnostic messages; masked MCP RMW avoids interleaved TPL updates. JSON/CSV include five-point analog evidence and two tag configs/results. MQTT connects using TCP3.1.1/CONNACK then disconnects; credentials stay RAM and are omitted from reports. WiFi QC reapplies supplied credentials, including the same SSID; blank HuaweiJIN password uses the approved lab profile. Static EthernetIP alone is UNVERIFIED until broker/TCP success. Actual device192.168.1.55 still1.2.0 during diagnosis; PD28 absent, EEPROM lacks QC identity. No Codex device upload.
