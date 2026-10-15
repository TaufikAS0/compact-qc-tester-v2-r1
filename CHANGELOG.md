# Installed diagnostic readability1.9.1 -2026-10-15

- Firmware1.9.0 ->1.9.1. Final build UTC2026-10-15T00:33:08Z /07:33:08 WIB, app1426336 bytes SHA256f0045e69e579e3fd44dfa3845b91c2106edb38adddc5df8ad57c1413d334ed8e. Build-Firmware.ps1/metadata/legacy gates PASS. Sketch1426189 bytes68%, RAM56848 bytes17%. No firmware edits after final build start.
- Authorized app-only OTA192.168.1.31 HTTP200; same serialTMC88F1559CA478, new boot/version/build and runtime/fileMD578216abd7926c5556ee64df06701584b verified. Profiles/recipes/calibration match before OTA; no erase. FinalM1 PD OFF/outputsOFF/faultfalse. First immediate postboot AHT was not ready; subsequent native browser shows fresh valid32.8C/41.9%RH. Do not misread initialization as permanent failure.
- Native installed page reloaded: large32.81C/41.94%RH/3.67mA values visible with units, sample ages, numeric Y grid, elapsed seconds X, min/mean/max and proportional lines. Desktop1920 screenshot self-check confirms all three actual graphs; no mobile/printer/accuracy acceptance claim. Minimum spans1mA/2C/10%RH prevent exaggerated tiny changes; historical valid values stay separate from stale/offline current blank. Point titles provide exact value/time, native hover behavior not separately certified.
- Node readability/station/operator regressions PASS (axes/units/time/gaps/minimum span/stale/offline/empty/single/negative), Python14 and portal2 PASS, diff checkPASS. No API/pin/NVS/driver/WiFi/polarity/calibration policy change. Read AGENTS/CURRENT and full Versioning/ChangeNotifications issue1 at start; shared lab WiFi retained. Feature-only publication, no merge.
- Ignored evidence build/chart-pre-ota-v1_9_1.json,chart-post-ota-v1_9_1.json,chart-final-v1_9_1.json,firmware-manifest.json. Physical RTC OSF/battery/time, PD absence, EEPROMWrite/WC and load tests remain open, not changed by chart UI.

# Diagnostic readability1.9.1 -2026-10-15

Large latest values+units, numeric/time axes, min/mean/max, point value/time and honest minimum scale spans replace unlabeled exaggerated mini-lines. Stale/offline shows no current value; timestamp gaps remain gaps. Node tests PASS; build00:33:08Z running, OTA/native revised UI pending. No pin/NVS/API/driver changes. Latest CURRENT is continuation truth.

##1.9.0 -2026-10-15

ST01..07: perM test recipes in independent CRC/revisioned NVS, readiness, clickable schematic map, bounded output guide with explicit ON/OFF observations and firmware timeout, unit history/laptop append-only archive, offline QR print/reference and bounded stale-aware mini charts. Recipe identity exported/hashed; old profile/calibration/pins/partitions preserved. Never infer physical PASS. See QC_STATION_REVISION.md and CURRENT/VALIDATION for exact build/install evidence.

##1.8.0 -2026-10-14

Operator/Technician view, explicit active versus draft profile with session/save review, guided applicable-test progress/filter/navigation, grouped profiles/clone/diff, optional technical details and retained DOM. New/api/qc/report captures authoritative QC/hardware snapshot plus matching CSV after OFF, requiresPIC/date/session/revision/QCcode, rejects stale/active capture. Laptop browser archive50 records/no silent eviction, JSON/CSV/bundle downloads and visible quota/corruption errors; immutable snapshots remain FAIL/INCOMPLETE. RTC OSF nowUNVERIFIED, communication/time validity separate. NVS/EEPROM/pins/polarity/WiFi/legacy partition retained. See04_Dokumen/OPERATOR_WEB_REVISION.md and VALIDATION for actual deployment.

##1.7.0 -2026-10-14

PD presets5/9/12/15/20V, draft slider and explicit review/confirm; source-capability snapshot explanations including fixed PDO/PPS limitations. /api/pd requires confirm=1 before hardware action. Stop/context changes invalidate staged requests. Existing profiles/calibration/pins/WiFi/partition retained. See04_Dokumen/PD_CONTROLS.md and VALIDATION for physical/build/upload evidence.

## 1.6.1 - 2026-10-14

- Browser testing of installed1.6.0 caught a sticky-disabled regression: M1 exclusion/unselected state disabled EEPROM/AHT/scan/SD buttons, but moving to an enabled profile never re-enabled them. Recompute direct-control gates on every render; preserve sensor/worker/OTA prerequisites. Add actual-DOM transition regressions.
- Clarify EEPROM transfer error with WC/write-protect/model/power checks; actual WC level remains unknown. Write failure is not reported as PASS or hidden in M0. User-authorized M1 EEPROM exclusion retained.
- Source1.6.0 ->1.6.1; build started2026-10-14T11:45:51Z. Final binary/runtime verification follows in VALIDATION/worklog. No new storage schema/pin/WiFi/partition change from1.6.0.

## 1.6.0 - 2026-10-14

- Add persistent editable QC profiles M0..M999, maximum16, with36 stable test switches. Default M1 excludes Radio configuration, RS232, PD and unresolved EEPROM; Radio communication/RS485 remain enabled. All-OFF is INCOMPLETE, exclusions never PASS. Enable repaired EEPROM explicitly after physical Write/Read evidence.
- Profiles stored only in qc-profiles/profiles-v1 with schema/CRC/readback and optimistic revision validation. Active profile edits reset session/results/captures and stop outputs; calibration and PCB identity retained. Direct API enforcement, input/background acquisition/result guards, profile mask/revision in JSON/CSV/QC code.
- AHT10: consistent ahtDetail, stage/Wire/frame diagnostics; calibrated startup probe before E1/reset,350ms command/reset settle, one six-byte measurement transaction, bounded busy/short-frame recovery. No AHT20 command/model inference. Physical sensor acceptance pending.
- EEPROM: transport Read success is distinct from missing/empty identity; bounded read/address/write/ACK/readback errors identify failing phase. Protect foreign bytes/old valid slot; no EEPROM erase or region change.
- UI retains unchanged output/table DOM rather than replacing buttons every poll. Immediate progress, serialized8-command queue and priority Stop cancelling queued work. Polling1.5s pauses during requests; heartbeat remains1s.
- Source1.5.1 ->1.6.0; installed baseline observed1.5.0 at192.168.1.31. No pin/partition/OTA-credential/WiFi-profile change. Build/upload/runtime evidence will be recorded in VALIDATION/worklog when completed.

## 1.5.1 � 2026-10-14

- Fix DO startup: preload GPIO latch with ESP-IDF before pinMode and explicitly reassert OFF after Arduino GPIO registration. Core3.3.10 ignores digitalWrite before pinMode.
- Per-channel DO polarity: DO1 GPIO32 and DO2 GPIO33 active HIGH per latest user confirmation that both were physically inverted on1.5.0; failed relay reported as hardware issue. LED polarity unchanged.
- Startup, command, Stop/OTA/heartbeat and per-channel timeout use the same relayLevel mapping. Add GPIO readback/mismatch command rejection and dashboard diagnostics; GPIO pad evidence is not output-load acceptance.
- Firmware1.5.0 ->1.5.1. No NVS/partition changes or device upload. Relay failure identified by user as hardware; both-channel polarity correction requires installed-firmware acceptance; regression build/tests recorded in VALIDATION.md.

# Changelog

## 1.5.0 — 2026-10-13

- Real asynchronous Modbus master2 independent tags (FC01/02/03/04, CRC/slave/function/length/exception, count1–2, datatype/word order, user verdicts). Separate serial/SPI/network menus and explicit RS232 fixture scenario.
- Add network worker: WiFi association/IP, Ethernet DHCP/static and MQTT3.1.1 TCP CONNACK test. StaticIP alone UNVERIFIED; successful MQTT overEthernet proves its network path. Volatile settings and credentials; credentials excluded from state/report. PubSubClient2.8 pinned dependency. Conflicting operations blocked; watchdog remains serviced.
- Explicit E70 Apply AT config + ACK/readback, persistent settings may require radio restart. Read mode remains non-mutating; no guessed LoRa parameters, no factory reset.
- MCP LED/radio RMW no longer calls TPL service between snapshot and write; preserves live unrelated/watchdog bits. Larger local PASS/FAIL badges and aggregate color.
- EEPROM Read adds slot/raw QC-region diagnostics, distinguishes readable uninitialized/foreign data from bus failure, preserves foreign data. PD diagnostics separate missing IC/unknown ID/USB-C not attached; bounded source capability retry and explicit negotiation outcomes. Actual1.2.0 device192.168.1.55 has EEPROM ACK/no valid identity and noPD28 ACK; no write/PDO/upload in diagnosis.
- Explicit WiFi QC reapplies credentials even on the same SSID; MQTT password without username rejected. AHT recovery budget resets only after valid measurement.
- Version1.4.0 ->1.5.0; new RAM/API only, no new ESP NVS keys, no pin remap/partition change. Hardware acceptance pending.

## 1.4.0 — 2026-10-13

- Replace one-shot AHT10 read with asynchronous power settling/reset/E1/status/measurement state machine; busy retry up to500ms, bounded recovery, exact failure diagnostics and fresh-data validity. Valid six-byte protocol reading yields PASS for communication/data availability, not calibrated sensor accuracy. No AHT20 CRC/init.
- Add separate five-point4–20mA QC (default4/8/12/16/20mA): capture16 fresh stable ADC samples, active calibration fingerprint, target/tolerance/error, user Sesuai/Tidak sesuai. All5 within tolerance and confirmed =>PASS; any rejected point FAIL. Changing calibration invalidates captured QC. Separate four-point calibration remains intact.
- New /api/current/qc/capture, /confirm, /reset; hardware.currentQc and AHT diagnostics included in JSON/report; CSV aggregate detail carries point evidence. No new NVS layout or device upload.

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
