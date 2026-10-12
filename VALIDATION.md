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
