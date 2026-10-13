# Current validation - v1.3.0 work in progress / 2026-10-13

Firmware source1.2.0 ->1.3.0. Independent outputs/LED active LOW/latched visual QC/I2C user confirmation and local module badges implemented. No device upload or hardware acceptance. Batch remains partial.

- Software tests PASS: Node dashboard regression (accepted state highlight, failed command, final result latch, I2C confirmation gates, shared poll); Python10 tests; portal packaging2/2; git diff check. New production C++ assertions test independent output timers/default OFF/wraparound, pending final ESP compile.
- Final build attempt generated identity2026-10-13T06:44:01Z /13:44:01 UTC+7, through Build-Firmware.ps1 core3.3.10 FlashSize16M. Build stopped before13:52 deadline while compiling libraries; no completed compile or manifest gate. No confirmed compiler error before stop. This timestamp is build start, not device clock or upload. No claim of new valid binary until gates finish. First attempt was cancelled to apply latest user correction LED LOW.
- Browser localhost8769 verified current1.3.0 preview, per-row default OFF/Sesuai/Tidak sesuai and local communication badges. All hardware controls remain locked in preview; no invented PASS. Screenshot build/dashboard-output-v1_3.png stays local/ignored.
- API RAM adds outputs.finalized and qc.i2c; no pin remap or new NVS keys. Eight mandatory vault files read: Firmware_AI_Vault_Home.md, Rules_Nama_dan_Folder_Workspace.md, Rules_Firmware_Standar.md, Rules_GitHub_Gitflow.md, Rules_Kredensial_WiFi.md, WiFi_HuaweiJIN.md, full Rules_Versioning.md and full Rules_Change_Notifications.md. STA HuaweiJIN; AP12345678. Existing selective STA driver migration unchanged; unrelated NVS preserved. OTA override/binaries not published.
- Remaining bench: active-LOW physical LED/DO behavior, independent outputs, heartbeat/timeout/OTA, masked MCP fault recovery and communication/endurance. Remaining software batch: AHT10 diagnosis, analog5-point QC, Modbus2 tags, RS232 scenario, menus/network/MQTT/radio configuration, EEPROM/PD diagnosis.

# Historical validation - v1.2.0 / 2026-10-13

Firmware1.1.0 ->1.2.0. New E70 read-only communication/configuration QC and separate laptop RS232/RS485 binary QC. User confirmed automatic RS485 direction without DE/RE. Modbus transactions remain deferred. No hardware upload by Codex; team installs firmware before bench testing.

- Final compile PASS: Arduino-ESP32 core3.3.10 / FlashSize16M / app maximum2097152. BuildUTC **2026-10-12T17:00:08Z**, **2026-10-13 00:00:08 UTC+7** (actual build start, not device clock/uptime). Sketch1190270 bytes(56%), application1190416 bytes, globalRAM54844 bytes(16%). App SHA256 **13b73d00ed26c02213b8113cab86658c688a8fa40bcb94369b16ae2a181face9**. Generated identity/manifest/binary gate PASS; legacy partition SHA256bc35a6a93f6204d86e25f28df16a6fb3e38a9052d8ca55a375c4e37c414c5c72, factory0x20000/2MB OTA slots PASS. Earlier attempts stalled on suspended compiler processes/RAM pressure and then exposed an ArduinoJson detail namespace collision; internal variables renamed, API detail field retained, final fresh build passed. No firmware source changes after this final build began.
- Software validation PASS: Python9 protocol/worker tests (100 fragmented round trips, CRC/pattern/echo/sequence/timeout/cancel/reboot rejection, settings and CSV safety); Node dashboard regression includes E70 targets/unreadable state and serial conflict/stop controls; metadata regression1/1; portal packaging2/2. Production constexpr serial CRC/frame assertions compiled successfully alongside existing input/calibration/PD/CSV/date assertions.
- Browser localhost preview inspected: firmware dashboard shows separate serial and E70 containers, targets and mandatory M0/M1. Separate laptop web shows RS232/RS485 selection, COM/baud/packet/timeout controls and JSON/CSV. Start without adapter rejects locally; IDLE and no ESP access. Screenshot build/laptop-qc-v1_2.png is software preview evidence, not physical PASS. Local servers8769/8770 stay available; live ESP tab untouched.
- Pinlist V2_R1 read from user's authenticated Chrome spreadsheet export: UART radio RX16/TX17, modeM0 MCP1B4/M1 MCP1B5; UART serial RX4/TX5, routeMCP2A2 HIGH RS232/LOW RS485. E70 technology/AT queries checked against vendor product/manual; factory references are not approved production targets. No persistent radio writes. Missing readbacks UNVERIFIED, LoRa SF/BW/CR/CRC NOT SUPPORTED.
- Firmware changes add /api/serial/start, /api/serial/stop, /api/rs232 aliases, /api/radio/start and qc.serial/qc.radio. No new NVS/EEPROM keys. DO active LOW/LED active HIGH, masked MCP/TPL service and legacy partitions retained. Radio mode restoration checked including Stop/OTA; physical waveform/reset still unverified.
- Mandatory vault files read in order: 00_Start_Here/Firmware_AI_Vault_Home.md; 01_Rules/Rules_Nama_dan_Folder_Workspace.md; 01_Rules/Rules_Firmware_Standar.md; 01_Rules/Rules_GitHub_Gitflow.md; 01_Rules/Rules_Kredensial_WiFi.md; 04_Profiles/WiFi/WiFi_HuaweiJIN.md; full01_Rules/Rules_Versioning.md; full01_Rules/Rules_Change_Notifications.md. STA HuaweiJIN, default recovery AP12345678. Existing divergent STA driver config migrates selectively; unrelated NVS retained. Private OTA override stays ignored; no secret/binary publication.
- Bench acceptance remains pending: independent USB-RS232/RS485 adapters/cabling, disconnected/corrupt/noisy serial, multiple baud/endurance, radio wake/query/exit/restore, TPL waveform and physical upload. Software simulation cannot certify transceiver voltage, RF link or device hardware.

## Historical validation - v1.1.0

# Current validation - v1.1.0 / 2026-10-12

Published-source target0.5.1 to1.1.0 (local1.0.0 build was also validated). Team uploads the device; no USB/OTA upload by Codex.

- Final compile PASS: Arduino-ESP32 core3.3.10 / FlashSize16M / maximum app2097152. BuildUTC **2026-10-12T08:25:49Z**, **2026-10-12 15:25:49 UTC+7**. This is actual build start, not device clock/uptime/upload time. Sketch1164158 bytes(55%); application **1164304 bytes**; globalRAM54100 bytes(16%). App SHA256 **b84d30b27ae73a8e39826b8e4ac512dfd70f3291434fdd44f46a0eb8ca6f6cde**. Binary contains exact metadata, CSV route/QC1/N-A and final OTA UI text; binary chip/size/legacy partition gates passed. Partition SHA256bc35a6a93f6204d86e25f28df16a6fb3e38a9052d8ca55a375c4e37c414c5c72.
- Production C++ assertions compiled successfully: input debounce/full cycles, calibration gain/offset/residual, PDO/source-RDO, M0/M1/unknown applicability, standard CRC32 vector, CSV commas/quotes/CRLF/formula-prefix/short-buffer, leap-year/date validation. These shared functions are used in actual firmware.
- Node dashboard tests PASS including profile selection, output lock before selection, M1 PD lock, N-A excluded from pending, fresh CSV download and server-rejection/no-download; original QC/JSON/escape/reboot/offline regressions retained. Node portal2/2 PASS; Python metadata gate regression PASS.
- Local browser preview: startup requires M0/M1; M1 pending count31->30 and PD request/voltage/current disabled, M0 re-enables them. M1 report shows code and CSV/JSON/date/PIC controls. Browser-downloaded CSV was parsed with Python: UTF8 BOM, Longhi first10 headers, M1/INCOMPLETE/PD N-A preserved. It is explicitly mock PREVIEW evidence, not device QC. Screenshot ignored build/dashboard-M1-export-v1_1.png and mock CSV build/export-preview-M1.csv. Agent preview tab/server closed; user's device tab untouched.
- Longhi read-only reference: remote fix/modbus26f9a1f and feature/longhi-modular-upgrade3a45448 contain CSV exporter; develop9aa0fc1 lacks it. No QC code format found there. Kept Longhi first10 columns, added QC1 schema/detailed results; no fake XLS extension, production/delivery dates or inventory data. Longhi worktree stayed clean.
- M1 skips only STUSB4500: N-A is immutable under result updates, excluded from overall and counts, endpoint/direct request/polling gated. Generic I2C ACK scan unchanged. Both variants retain deferred external RS tests, and data-only statuses remain INCOMPLETE. No added NVS keys for variant/export; RAM session/profile reset on reboot/change. Active calibration/EEPROM retained. Code CRC is evidence identity, not signature or physical proof.
- DO1/DO2 active LOW and MCP LEDs active HIGH retained per final user correction. Profiles enabled; boot OFF, heartbeat/timeout, fault locks and unrelated MCP/TPL bits retained. No pin/EEPROM format/WiFi change from1.0.0.
- WiFi approved HuaweiJIN STA / AP12345678; updates only divergent driver STA profile, unrelated NVS retained. Approved public LAB OTA fallback preserved from remote; ignored private override used in this local build. Private password/artifacts absent from tracked source; no binary or secret published. Public release packaging is not executed on this private build.
- Git publication scope: feature/qc-operator-ota only; merged remote e7c72e5, preserving portal packaging/default LAB OTA changes. No force push/main/develop merge or device flash.
- Physical acceptance still requires team bench tests: M0/M1 workflow on actual boards, input/output/OLED levels, independent4-20 accuracy, EEPROM power cycle, sensors, actual PD12V VBUS, Ethernet packets, OTA recovery and watchdog waveform/reset. No physical PASS inferred from simulation/compile.

## Historical validation

# Current validation - v1.0.0 / 2026-10-12

Supersedes historical validation below. This feature integrates remote v0.5.1 portal packaging; team uploads the device. DO1/DO2 active LOW and LED MCP active HIGH are the user's final correction; both profile flags are enabled, runtime MCP fault locks retained.

- Final v1.0.0 integrated compile: PASS, core3.3.10, buildUTC2026-10-12T07:46:46Z (14:46:46 UTC+7), app1147392 bytes, sketch1147250 bytes(54%), globalRAM54076 bytes(16%). SHA2569e05c99a309f163524cf77639cc691a80ee49f2ed4ad39241f0160bf9c06f169; metadata/legacy partition gate passed. No upload.
- Node VM dashboard tests: PASS for seven automatic inputs, per-row LED/DO controls and server state confirmations, rejected capture without false PASS, EEPROM read, I2C list, AHT/RTC, safe text, fresh report without control token, reboot/disconnect handling.
- Node portal package tests: 2/2 PASS for legacy placement/erased NVS and rejecting changed offsets/oversized/bad images. Python metadata regression: PASS for missing/stale/matching metadata and version changes. Production C++ algorithm assertions are compiled with firmware: debouncing/full cycles, least-squares gain/offset/residual, fixed PDO encoding and source RDO indexing.
- Browser localhost preview: all eight navigation pages inspected, input/OLED containers, LED table and separate DO containers, four capture controls, AHT10/DS3231, EEPROM Read, bounded PD request and report/OTA link visible. Hardware actions reject; no fabricated physical PASS. Desktop screenshot saved in ignored build/dashboard-input-v1.png. Preview tab/process closed; live device untouched.
- Partition parser accepts the legacy table and distinguishes a modified factory offset. Public packaging reads firmware_version.h and refuses missing/stale app identity/hash. Actual public release packaging is not performed: local build uses an ignored private OTA override and is not distributable.
- Private OTA password and .bin/.elf/.map/generated metadata are absent from tracked source. Deliberate public LAB defaults from remote remain supported in clean checkouts. No secret or binary pushed.
- Source NVS changes: new qc-adc/cal-v1 CRC calibration blob; existing EEPROM QC01 slots and qc-settings remain. HuaweiJIN STA, AP12345678; driver profile updates only when different, unrelated NVS preserved. Deliberate portal merged LAB reset is an exception: fast merged resets NVS, full erase also deletes storage.
- Physical tests NOT VERIFIED: input/output levels, OLED appearance, 4-20mA independent accuracy, EEPROM persistence/power interruption, AHT/RTC hardware, STUSB actual12V VBUS/contract and detach, Ethernet packets, OTA recovery/watchdog waveform/reset. RS485/RS232 external tests remain deferred. PD software corrections are not measured proof that the reported20V issue is fixed. No device upload performed.
- Git publication scope: feature/qc-operator-ota only, merged remote e7c72e5 without discarding team changes. No main/develop push or merge.

## Historical validation - v0.5.x and earlier

# Current validation - v0.5.0 / 2026-10-12

v0.5.1 adds owner-authorized standard full/merged USB distribution and optional
full erase, with explicit destructive-data warnings. This changes the runtime
version and release packaging only, not hardware output locks or the QC logic.
Build, package verification and portal tests must be completed for its release;
no physical v0.5.1 flash or QC evidence is claimed here.

This section supersedes historical local/uncommitted status below. Source prepared for team USB installation and future web OTA; v0.5.0 has not been uploaded to a device in this turn.

- Feature branch: feature/qc-operator-ota, renamed from feature/lab-wifi-com12; local develop baseline is upstream main 35733ba because remote develop does not yet exist. No direct main/develop push or merge.
- User confirmed unchanged V2_R1 pinmap for the two PCB hardware labels and C7559 EEPROM; external RS485/RS232 tests explicitly deferred. Config/routing implemented; no Modbus transaction acceptance claimed.
- Node VM tests: PASS (UI syntax, locked outputs, input stimulus request/ON level, server rejection visible without fabricated PASS, safe text rendering, fresh server report, no token export, deferred tests, ADC model confirmation, watchdog error, reboot notice, dynamically discovered I2C manual selector).
- Browser localhost preview: all 12 sections visible; PD controls bounded 5000-20000mV; relay/LED disabled; reports show INCOMPLETE/DITUNDA. Hardware actions rejected in preview. No device was contacted by preview.
- Staged-file checks: private OTA header/access file, binary/ELF/map, local backups and build tools excluded. Actual local OTA password absent from staged diff. Shared lab Wi-Fi profile is the approved workspace exception.
- Final source compile: PASS, Arduino-ESP32 3.3.10 / FlashSize=16M / upload.maximum_size=2097152. Application 1,120,002 bytes (53% of 2MB), global RAM 57,380 bytes (17%). Self-contained config header verified by full compile after include-order correction. Final corrections (no fictitious initial PD request and trimmed operator identity) included in this build.
- Generated partition binary parsed and compared: exact legacy NVS/otadata/factory/dual-OTA/storage offsets and sizes match partitions.csv. Application ESP32 chip ID, app descriptor magic, and <2MB size verified. No OTA/USB physical acceptance inferred from these checks.
- NOT VERIFIED ON DEVICE: new OLED workflow, ON/OFF stimulus gating, ADC model/conditioning/current accuracy, EEPROM writes/power interruption, PD renegotiation/actual voltage, Ethernet controller/link/packets, RTC battery, ATtiny protocol, output polarity, new OTA login/abort/reboot/partition selection, watchdog waveform/forced reset under new load. Automatic protocol evidence and operator observations are labelled separately in results/docs. Hardware acceptance is assigned to team.
- Current report notes waveform/physical reset unverified. Legacy no-core-dump partition limitation retained.

## Historical evidence (before v0.5.0)

### Initial validation - 2026-10-09

- Arduino CLI compile `esp32:esp32:esp32`, ESP32 core 3.3.10, ArduinoJson 7.4.3: PASS.
- Sketch storage: 1,035,018 / 1,310,720 bytes (78%). Global RAM: 48,972 / 327,680 bytes (14%).
- Node UI tests: PASS for output locks, rejecting unattempted LED confirmation, required report identity, missing-test status, FAIL propagation, rejecting overall PASS for partial evidence, and clearing results for a new inspection.
- Browser preview loaded and layout inspected. Preview is explicitly labelled disconnected from hardware and output actions are disabled.
- Configuration used: provisional classic ESP32 profile; OUTPUT_PROFILE_VERIFIED=false; MCP23017_PROFILE_VERIFIED=false.
- NOT VERIFIED: MCU physically fitted, boot straps, power levels, relay/LED polarity, AP on-device, I2C, SD, physical outputs, timing/disconnect on-device, watchdog behaviour, electrical performance or post-power-cycle operation.
- No firmware was flashed and no existing firmware project was edited.

## Local prepared v0.3.0 — 2026-10-09

- Upstream commit: 35733ba17a8e790fc68dafe9ba771295329a26fd.
- Branch: feature/lab-wifi-com12; changes local and uncommitted; not pushed.
- COM12 esptool detection: ESP32-D0WD-V3 rev3.1, 16 MB flash.
- Final compile core 3.3.10 / ArduinoJson 7.4.3, FQBN esp32:esp32:esp32:FlashSize=16M: PASS. Sketch 1,038,258 bytes; global RAM 48,988 bytes.
- Node UI tests including TPL error/status, continued output locks and report physicalResetTestVerified=false: PASS.
- Binary partition readback: NVS 0x9000/0x8000; otadata 0x17000/0x2000; factory 0x20000/0x200000; storage 0x620000/0xf0000 retained.
- STA HuaweiJIN and AP password 12345678 implemented. Driver config migration compares profiles; application NVS is retained. No on-device verification of new profile yet.
- TPL5010 DONE mapping verified from V2_R1 pinlist; MCP1 model and hardware timer interval remain unknown. TPL_MCP23017_CONFIRMED=false. No new firmware uploaded; original firmware retained.
- Local original layout and full NVS backups are in ignored build/ files. No secrets or backups pushed.

## MCP confirmed and enabled build — 2026-10-09

- User confirmed MCP23017-E/ML. TPL_MCP23017_CONFIRMED=true; relay/LED tests still locked.
- Enabled build: PASS, 1,038,646 bytes program; 48,988 bytes global RAM. Node UI tests: PASS.
- Flash attempted with preserved layout addresses 0x1000 / 0x8000 / 0x17000 / 0x20000. esptool could not open COM12 (FileNotFoundError). No flash writes started.
- Windows serial and present PnP inventory confirmed COM12 and CP210x absent at this attempt. Await USB reconnect before upload and runtime checks.

## Successful COM13 upload — 2026-10-09

- User authorized replacement COM13 CH340 board, ESP32-D0WD-V3 rev3.1, 16 MB, MAC verified locally (not published).
- Fresh compile and UI tests: PASS. esptool flash: PASS; bootloader, partition table and application hashes verified.
- COM13 layout matches preserved NVS 0x9000/0x8000, otadata 0x17000/0x2000, factory 0x20000/0x200000 and storage 0x620000/0xf0000. Original NVS backed up locally; no NVS or storage erase.
- otadata alone erased to select factory and remove stale empty OTA slot selection. Final reboot boots v0.3.0.
- STA HuaweiJIN CONNECTED with assigned LAN IP; AP COMPACT-QC-xxxx at 192.168.4.1, password 12345678.
- HTTP root 200; dashboard rendered firmware 0.3.0 and TPL DONE active at MCP1B7. UI relay/LED actions disabled.
- Same session token across uptime 17765 -> 128414 ms; DONE counter 680 -> 4761; tplReady=true, tplError empty. Active DONE state and stable execution observed; physical waveform and forced watchdog reset not tested.
- A parallel HTTP client had one 8-second timeout while dashboard polling remained responsive; subsequent API reads succeeded. No device restart detected in the comparison.
- Boot reports missing core dump partition; flash crash capture unavailable under preserved old layout.
- Branch feature/lab-wifi-com12, local uncommitted changes, no push.
