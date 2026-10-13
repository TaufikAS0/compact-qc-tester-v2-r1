# Changelog

## 1.3.0 — 2026-10-13

- Independent LED/DO states and per-channel60s timeout; ON no longer stops other outputs. Startup OFF, heartbeat3500ms/Stop/OTA safeguards retained. Latest user correction: LED MCP active LOW; DO remains active LOW. Masked MCP latch/direction readback added.
- Valid output commands renew activity; UI deduplicates overlapping polls/heartbeats, displays rejected commands and highlights accepted ON/OFF state. Physical intermittent communication diagnosis remains pending.
- One Sesuai/Tidak sesuai observation finalizes output QC; later controls preserve PASS/FAIL until session reset. Add I2C scan/user confirmation evidence and local result badges for OLED, sensor, storage, PD, Ethernet and serial/radio cards.
- Version1.2.0 ->1.3.0; new RAM fields outputs.finalized and qc.i2c, no new NVS keys, no pin remap, no device upload. Larger batch remains unfinished; see task checklist.

## 1.2.0 — 2026-10-12

- Reopen RS232 and RS485 QC per user, automatic RS485 direction confirmed. Separate local laptop web/backend defaults9600/8N1/100packets; CRC/pattern/sequence/ACK and matching device result required. Echo/timeout/corruption/restart cannot PASS. Cancellation INCOMPLETE. JSON/CSV automatic local reports; tokens excluded.
- Add `/api/serial/start`, `/api/serial/stop`, legacy `/api/rs232/*` aliases and `qc.serial`. UART1 RX4/TX5, route MCP2A2 HIGH RS232/LOW RS485 with readback. Conflicting QC operations blocked, UART loop bounded, no new NVS.
- Add `/api/radio/start` and `qc.radio`: E70-915NW14S CC1310/15.4 star, not LoRa; UART2 RX16/TX17, mode MCP1B4/B5 temporarily HIGH with masked restoration including Stop/OTA. Read-only AT and seven target comparisons; unreadable UNVERIFIED. Editable factory references are not approved production profile. No SF/BW/CR/LoRaCRC PASS, RF payload test or persistent radio write.
- Version1.1.0 ->1.2.0, generated UTC build identity retained. Wi-Fi, partitions, EEPROM, calibration and OTA credentials unchanged. Team uploads; no Codex flash.

## v1.1.0 — 2026-10-12

Adds explicit session M0/M1 selection and Longhi-style QC export after the local validated1.0.0 build; published baseline remains0.5.1 until this feature push.

- Select variant before QC mutations; required variant parameter on /api/qc/new, reject unknown models before reset. RAM profile/results/session ID reset on reboot/change, independent of EEPROM/NVS calibration. Active PD negotiation must finish before profile change.
- M0 all current modules; M1 PD Sync unpopulated: NOT APPLICABLE, excluded from overall/pending/failure, no PD register reads/writes/polling, rejected PD requests. ACK-only I2C scan unchanged. RS485/RS232 still explicitly deferred.
- QC1 evidence ID includes model/serial/status/random session/CRC32 of ordered evidence + version/build. API/dashboard, JSON and CSV share it; not a cryptographic signature or physical acceptance.
- New /export/qc.csv server snapshot, Longhi ten-column prefix plus QC code, metadata and detailed applicable results. UTF8 BOM/CRLF/quoted fields, quotes doubled, formula-leading text guarded, dates validated. No invented warehouse/delivery/production metadata, no fake XLS, no INCOMPLETE-to-FAIL/PASS coercion.
- Existing user correction retained: DO active LOW, MCP LED active HIGH. No device upload, no pin/EEPROM/WiFi credential change; no new NVS keys for variant/export.

## v1.0.0 — 2026-10-12

Previous source baseline v0.5.0. Major bump because QC input acceptance and operator/output API contracts change. This is a bench firmware feature build, not a production release or a device upload.

- Dashboard follows supplied dark industrial workstation references, embedded offline CSS, navigation, module cards, per-row controls and live firmware state. Example pin numbers/specifications/measurements from design references are not copied as hardware facts.
- Inputs are exactly DI1-4 plus DMM, Normal, Webserver. Detect two complete HIGH–LOW–HIGH cycles (`H L H L H`) with 50ms stable levels, automatically latch PASS until session reset. Normal = DMM GPIO2 LOW and Webserver GPIO39 LOW. User confirmed this mapping and ADS1115/DS3231 on 2026-10-12. LoRa/display/boot inputs are removed from QC acceptance scope.
- Remove generic operator confirmation and old `/api/qc/start`, `/api/qc/feedback`, `/api/current/config` workflows. Optional operator name remains report metadata. OLED gets dedicated visual buttons; LED table has individual ON/OFF and physical confirmation. DO1 GPIO32 and DO2 GPIO33 have separate containers. ON observation gives requested visual PASS; OFF observation is recorded separately. Unattempted/wrong-state confirmation is rejected server-side. One output ON at a time, 60s maximum and 3.5s heartbeat stop. User corrected polarity to DO1/DO2 active LOW and LED MCP active HIGH; output/MCP profile flags enabled after that confirmation.
- I2C scan only ACK list 0x08–0x77, with names from pinlist. No generic protocol tests or scan-issued measurement/init commands. AHT10 uses E1 init and six-byte measurement, no AHT20 CRC. Separate AHT10 temperature/humidity and DS3231 clock/temperature containers; RTC read-only with oscillator-stop warning.
- ADS1115 AIN0 readback-verified continuous 128SPS, ±4.096V; raw and voltage/current telemetry. Before calibration, conversion is explicitly provisional 100Ω, gain1, offset0, not guaranteed conditioning. Four editable references default 4/10/15/20mA; capture mean and standard deviation of 16 new samples. Linear least-squares fit `mA=gain*ADC+offset`, R², maximum residual; reject incomplete/duplicate/insufficient span/noisy/stale/out-of-range data. Active coefficients + four points in CRC-protected single NVS blob `qc-adc/cal-v1`; EEPROM identity layout unchanged. Calibration residual is not an independent accuracy test.
- EEPROM explicit `/api/identity/read` refreshes stored TOP/BOTTOM, generation and CRC validity. Failed reads clear cached identities. Write keeps existing dual-slot QC01 format at 0x3F80–0x3FFF and rejects foreign data.
- PD writes/readback each of three sink PDOs; PDO1=5V, PDO2/3=requested target, only one/two active. Removes stale highest PDO3 and preserves count reserved bits. Soft reset requests renegotiation; capture Source_Capabilities, decode source RDO object position, expose sink/source PDOs, mismatch/timeout. Software reports detach/profile reload without automatically increasing voltage. No claim that software proves VBUS voltage, and no STUSB NVM write. Actual 12V issue requires bench verification; cause not proven by source inspection.
- Version source moved from config.h to firmware_version.h. Build script regenerates one UTC identity per invocation, requires ESP32 core3.3.10, verifies matching metadata in final application and writes SHA-256 manifest. Serial/API/dashboard/OLED expose version and build UTC. Generated metadata/artifacts/secrets remain ignored. Build UTC is not RTC/uptime/upload time.
- Wi-Fi remains approved HuaweiJIN STA and AP password12345678, targeted driver-profile migration preserves other NVS. OTA auth, TPL MCP1B7 bit preservation and legacy partitions remain. RS485/RS232 external tests remain deferred; Modbus configuration and MCP2A2 routing remain available.

Validation, actual build UTC, binary size/hash and publication evidence: [VALIDATION.md](VALIDATION.md). Physical upload is assigned to the team.

- Integrated remote v0.5.1 portal packaging and intentional public LAB OTA defaults; version parsing now uses firmware_version.h and packaging requires matching binary metadata/hash. Private overrides remain excluded from distribution.
