# Installed operator workflow1.8.0 -2026-10-14

- Source/device1.7.0 ->1.8.0, final Build-Firmware.ps1 PASS: UTC2026-10-14T14:54:18Z, app1315136 bytes, SHA256f1b81864417bcbbb78854bca766bbd25f337b7f7ae1a6bb5d34095b387819125; core3.3.10/classic ESP32/16MB, legacy partition and2MB app gates PASS. No firmware edit after final build began. Sketch1314993 bytes62%, RAM56712 bytes17%.
- Authorized app-only OTA192.168.1.31 returnedHTTP200; same serialTMC88F1559CA478, exact version/build and runtime/fileMD53e7505c7ee48874729c5e77d408219c9 verified. Saved M0/M1/M2 profiles (M2 rev6) and calibration gain/offset equal pre-OTA snapshot. No NVS/storage/partition erase; existing approved lab STA/AP/pins/TPL/polarity retained. Mandatory versioning/change rules read via issue1; prior repo/current/handoff rules retained.
- Node expanded operator/archive/draft/context/Stop/PD regressions PASS, Python10 and portal2 PASS. Tests cover operator/technician preference, active versus un-applied selection, pending/fail/all, grouped clone/diff, no mutation before confirmation, report immutability, exact CSV, token stripping, quota/corrupt/full rejection/no eviction, double save/context expiry and Stop during refresh. Compile confirms C++ formatter/endpoint/RTC integration.
- LIVE API: RTC communicationValidtrue, OSFtrue/timeValidfalse, RTCUNVERIFIED and overallINCOMPLETE; no RTC write. Report endpoint missingPIC400, invalidFeb30 date400, stale session/revision/QCcode409. Valid CODEX SOFTWARE CHECK snapshot200, JSON/CSV exact code/status/PIC/profile/revision/every test; first10 Longhi columns and safe quoting retained. New session doesn't change saved ignored report files. Tests are software evidence, not physical QC certificate.
- GET installedHTML confirms new view/draft/review/progress/clone/report/archive controls and script present. Browser visual/layout/archive-persistence testing was interrupted previously by Computer Use URL policy; not claimed as PASS. Native browser revisit pending separately; no policy bypass. Laptop archive features are Node-tested; browser storage can be cleared by browser cleanup, max50 rejects new rather than evicting old. Download JSON/CSV/bundle for durable file handoff.
- Final deviceM1 with PD/EEPROM/Radio configuration/RS232 excluded; outputsOFF/faultfalse, AHT fresh six-byte data (~34.09C/46.36%RH at check). No outputON, EEPROMWrite, PDnegotiation, radioApply or physical sensor accuracy/load/WC proof. PD0x28 absence and old EEPROMWire4 remain hardware followup, not silently resolved. Source/docs/tests ready feature-only commit/push, no merge or general serial-web work.
- Ignored evidence build/pre-ota-state.json,post-ota-state.json,runtime-final-v1_8_0.json,operator-report-runtime-v1_8_0.json/.csv,firmware-manifest.json. This is exact new1.8.0 evidence; older pending/install entries below historical. Current publication/next action in CURRENT/Git history.

# WEB-01..07 revision1.8.0 local -2026-10-14

Implemented operator workflow/confirmation/grouped profiles/report snapshots+browser archive and RTC OSF semantics. Node/Python10/portal2 PASS; preview/build/OTA pending. Device prior1.7.0. Scope04_Dokumen/OPERATOR_WEB_REVISION.md; next action/current publication in CURRENT. Older installed checkpoints below historical.

# Final PD controls - installed1.7.0 /2026-10-14

- Source/device1.6.1 ->1.7.0. Final Build-Firmware.ps1 PASS: UTC2026-10-14T13:42:34Z, app1287984 bytes, SHA256205b4a636a1db5b5b10f03dfdc5969c458b0b2380a0ddfe9c0d708bb06f6fe44; core3.3.10/classic ESP32/16MB/2MB OTA slot and legacy partition gates PASS. No firmware edits after final build started. First13:39:48Z compile failed (uncaptured server variable); fixed to module web pointer, superseded by final13:42:34Z build. Never uploaded failed candidate.
- App-only OTA192.168.1.31 returned200; same serialTMC88F1559CA478, version/build and runtime/fileMD5557d9c223ada1b62577b00c7c94a424e verified. Calibration gain/offset and all saved profiles exactly preserved. No NVS/storage/partition erase; approved existing lab STA/AP unchanged. Rules_Versioning/Rules_Change_Notifications reread from GitHub issue1 snapshot; current repo entry/checkpoints/docs read.
- Node expanded UI tests PASS; Python10/portal2 PASS. Browser preview and actual installed browser verify presets/draft/slider/review/cancel; live20V review cancelled, slider19.95V draft only. /api/pd missing confirm rejected400 before any PD request. Stop during fresh-state confirmation, double click, rejection, draft/session/cap change, mismatch/current/unknown/M1 gates covered by Node. No accepted voltage negotiation performed by Codex.
- Read-only M0 snapshot before and after OTA: PD NOT DETECTED/no I2C response0x28, valid=false/attached=false/pending=false/requestedMV0/sourcePDOs empty. This is distinct from source voltage mismatch; population/power/bus cause unknown. Source capability and actual VBUS remainUNVERIFIED. STUSB4500 lacks PPS;16/17/18V require matching fixed source PDO. See04_Dokumen/PD_CONTROLS.md for primary sources and protocol alternatives.
- Final deviceM1, PD/EEPROM/Radio configuration/RS232 OFF, outputs OFF. M2 revision6 retained with both DO tests OFF. AHT remains valid real six-byte/Wire0 data (~34.24C/49.30%RH at test); no accuracy/load/WC acceptance claimed. EEPROM not rewritten; old Wire4 limitation retained. Runtime/build evidence ignored underbuild/, screenshotpd-review-live-v1_7_0.jpg shows staged confirmation, not negotiated voltage. Preview screenshot also labelledPREVIEW.
- Source/docs/tests ready for feature/qc-operator-ota commit/push, no main/develop merge or general serial-web changes. Final publication status is recorded in CURRENT/Git history.

# PD revision1.7.0 in progress

Source changes and software checks complete; final build/OTA pending. See04_Dokumen/PD_CONTROLS.md and CURRENT. M1 PD staysOFF; Codex sends no voltage command. Existing installed1.6.1 evidence below is historical.

# Final installed handoff - v1.6.1

Installed and verified via authorized app-only OTA at192.168.1.31: buildUTC2026-10-14T11:45:51Z, app1282720 bytes/SHA256dc4f8de4d78331931b85c840038a95089559c0a38542262f5eb74666c12c7958, runtime/fileMD5ba772733fa865400ed57a91eff4390a5. Classic ESP32/16MB legacy layout retained; no NVS/storage erase. Calibration and saved profiles survived OTA reboot.

AHT now produces actual data.1.6.1 browser verifies M1->M0 buttons re-enable and one-click EEPROM Read works. M2 example is saved(revision6) with DO1/DO2 tests OFF; both were independently toggled and saved in browser. Add further profiles in Konfigurasi QC, max16 IDsM0..M999. Final active profile M1 excludes Radio configuration, RS232, PD and EEPROM, retaining radio communication/RS485. All hardware outputs left OFF.

EEPROM Write remains unsuccessful(Wire4 at0x3F80); region still blank. WC actual level/wiring remains unknown, asked through GitHub issue1. Keep EEPROM excluded on M1 until successful physical Write+Read evidence; do not fabricate PASS or drive an unknown WC net. Physical relay loads, reference sensor accuracy, power-cut storage/OTA recovery and TPL waveform still require bench evidence. Full QC overall remainsINCOMPLETE. Current final instructions supersede pending/historical entries below.

# Current patch handoff - v1.6.1

1.6.1 recomputes direct button gates so selecting M0 after M1 enables EEPROM/AHT/scan/SD controls again; added regression covers both profile and selection transitions. Later source/build state supersedes1.6.0 below. Build starts2026-10-14T11:45:51Z, final gate/upload still pending at this checkpoint.

On installed1.6.0 (OTA already verified), AHT produces fresh six-byte data, status0x18, Wire0, approximately36.1C/29.8%RH. Accuracy against a reference remains untested. EEPROM Read returns bothEMPTY FF; Write failed twice (API and one browser click) at0x3F80, Wire4,18-byte transfer including address/16 queued data bytes. Fresh Read after the first failure confirms no QC bytes changed. WC level/pin wiring unknown; asked laptop-main AI through issue1. ST datasheet section2.4 makes WC HIGH/data NACK a plausible explanation, not a measured fact. Keep EEPROM OFF on M1; do not drive an unknown WC pin or erase foreign bytes. No EEPROM Write/readback PASS claimed.

Profiles M1 direct-exclusion rejection and M2 DO-off/re-enable/all-OFF INCOMPLETE/stale-edit guards passed on device. M2 saved as the user's example, both DO tests OFF. Verify profile persistence across the upcoming1.6.1 OTA reboot; calibration already survived the first OTA. User tab tested directly; source1.6.1 upload still pending here.

# Current handoff - v1.6.0

- OTA target authorized by user:192.168.1.31. Latest read-only baseline: classic ESP32-D0WD-V3, source1.5.0/build2026-10-14T05:16:49Z, TPL ready, no active output/fault. Match serial and exact manifest before and after app-only OTA; no merged/partition/NVS erase.
- Use Konfigurasi QC for saved M0/M1/M2/M3 profiles.36 switches, up to16 profiles M0..M999. Test OFF becomes IGNORED BY PROFILE in reports/OLED/counters, never PASS. M1 default: Radio configuration, RS232, PD and unresolved EEPROM OFF; Radio communication/RS485 ON. If EEPROM Write/Read succeeds physically, enable it in M1 and test it. Active profile edit/change creates a new session; calibration/identity stay.
- New storage is qc-profiles/profiles-v1 only. Schema/CRC/readback, stale-edit rejection and explicit errors; corrupt storage blocks selection rather than silently narrowing QC. DO32/33 active HIGH and LED MCP LOW retained; TPL safety/partition/WiFi/OTA unchanged.
- Current device evidence: AHT E1 initialization NACK; EEPROM transport readable with two EMPTY FF slots. Missing QC identity is not Read/bus failure. Test Write only in approved region0x3F80-0x3FFF, read back and verify labels/CRC; WC still unknown. No foreign bytes erased.
- New UI gives immediate progress and queues rapid clicks; Stop clears queue and sends immediately. Repeated status polling retains unchanged output controls. Bench check negative requests/Stop/heartbeat and physical loads separately.
- Rules read from GitHub assistance issue1: complete Rules_Versioning, Rules_Change_Notifications and Home snapshots; related firmware/folder/Gitflow/credential/color/planning/connectivity/baseline rules read from private vault aa0bb9e. Current HuaweiJIN profile was retained under explicit existing LAB authorization; no customer profile introduced.
- Runtime upload outcome and final build hash will be recorded in VALIDATION/CURRENT. Until then1.6.0 is local source, not an installed claim.

# Output fix1.5.1 � current instructions

DO1 GPIO32 and DO2 GPIO33 are active HIGH (OFF LOW), per latest explicit user confirmation that both were inverted. LED MCP stays active LOW. This supersedes historical blanket DO active-LOW instructions below.

After team upload: verify build/version, reboot defaultOFF, independently ON/OFF each DO, bothON, Stop, heartbeat loss and per-channel timeout. Dashboard now shows expected polarity and measured GPIO pad level. If GPIO32 follows LOW/OFF and HIGH/ON but DO1 load stays unchanged, inspect the driver/module/wiring; do not report physical PASS based on GPIO alone. If GPIO mismatches, firmware rejects the command and stops outputs. User identified the failed relay as hardware; verify the repaired fixture physically. No firmware upload performed by this change.

# Team handoff — v1.5.1

Read [current operator guide](QC_OPERATOR_GUIDE.md), [validation](../VALIDATION.md), [backlog/checkpoint](../ai/tasks/2026-10-13-qc-revision-backlog.md), [CURRENT worklog](../ai/branches/feature--qc-operator-ota.md). Source1.2.0 ->1.5.0 revisions: independent active-LOW outputs, latched confirmation, I2C list/user verdict, AHT10 recovery, five-point analog QC, two-tag Modbus, serial/SPI/network menu separation, worker WiFi/ETH/MQTT, explicit E70 AT Apply/readback and EEPROM/PD diagnostics. Team uploads; no Codex flash. Later update sections supersede historical procedures below.

No pin remap or ESP NVS layout change. LED MCP is active LOW; DO1 GPIO32 and DO2 GPIO33 are active HIGH (ON=HIGH, OFF=LOW). STA boot profileHuaweiJIN/AP recovery12345678; old STA driver config migrates selectively, unrelated NVS stays. Test-specific network/tag config is RAM. E70 Apply may persist radio settings and require restart; read validation remains read-only. Preserve TPL B7 through atomic masked RMW/restoration. No automatic EEPROM foreign-data erase or STUSB NVM writes. Exact final build/metadata/hash in VALIDATION.

Historical v1.2.0 read-only bench diagnosis at192.168.1.55: EEPROM57 ACK/no valid identity, PD28 absent. New software cannot negotiate with an absent/unpowered IC; verify M0 population or select M1 if intentionally unpopulated. The later checkpoint observed v1.5.0/build2026-10-14T05:16:49Z; old EEPROM/PD findings were not revalidated on that version. Current IP, installed version and relay repair status require a fresh check. Physical acceptance remains pending after team installation.

## Build and install

1. Verify actual port, classic ESP32 chip, MAC/16MB and current partition table. Last historical port was COM13 CH340; do not assume current port. Back up NVS/partition table. No Codex upload of v1.5.1 is performed.
2. Check `relay_profile.h`: DO GPIO32/33 active HIGH, ON=HIGH/OFF=LOW. LED MCP1 remains active LOW in `config.h`. Both output/MCP verification flags are true; BANK0/SEQOP0 is checked at runtime. Use a suitable isolated bench fixture and verify actual OFF before attaching loads.
3. Install pinned dependencies from README, Python3 and Node. Run `./Build-Firmware.ps1`; this generates UTC identity before compile and validates it in binary. Without a private override, use the intentional public LAB default admin / CompactLab050! from lab_ota_defaults.h (owner decision 2026-10-12). Private override builds stay private; -PublicLabRelease refuses them and packages only committed source.
4. Keep app and `build/firmware-manifest.json` together internally. Confirm manifest version/buildUTC/SHA-256 matches the exact `.ino.bin` to install. Build timestamp is build start, not device clock/upload time.
5. For initial USB installation use verified legacy offsets below; never Arduino default offsets; validated portal merged is allowed for deliberate LAB reset. For an installed OTA-capable firmware, `/update` uses application `.ino.bin` only.

| Partition | Offset | Size |
| --- | --- | --- |
| nvs | 0x9000 | 0x8000 |
| otadata | 0x17000 | 0x2000 |
| phy_init | 0x19000 | 0x1000 |
| factory | 0x20000 | 0x200000 |
| ota_0 | 0x220000 | 0x200000 |
| ota_1 | 0x420000 | 0x200000 |
| storage | 0x620000 | 0xF0000 |

USB component offsets: bootloader **0x1000**, partitions **0x8000**, application **0x20000**. If boot selection must be reset to factory, erase **only** otadata0x17000/0x2000. Preserve NVS/storage. Verify write hashes. Do not write Arduino boot_app0.bin at0xE000. Validated portal merged at offset0 is an alternative for deliberate LAB reset: even fast write resets NVS/settings; Full erase + write additionally deletes storage. Build this package with -PublicLabRelease in a clean checkout without a private override. Component USB is the data-preserving option. These instructions depend on matching the device's current layout first.

## Bench procedure

- On boot verify serial `TMC` +12 uppercase STA MAC hex digits, source v1.5.1 and matching buildUTC on serial, `/api/state`, dashboard and rotating OLED screen. Wi-Fi STA HuaweiJIN; recovery AP`COMPACT-QC-xxxx`, password12345678 at192.168.4.1. Existing driver STA profile is updated only if different; unrelated NVS preserved.
- New Session resets RAM results/input counters and capture points, not active calibration or EEPROM. Operator name optional. Browser reload preserves firmware counters/results; device reboot resets RAM. Download report before reset/reboot. No input operator confirmation exists.
- DI1–4 and each DMM/Normal/Webserver: provide `HIGH LOW HIGH LOW HIGH`, holding each state≥50ms. Verify counter0→1→2 and latched automatic PASS; stuck HIGH/LOW/bounce must not pass. Normal is GPIO2 LOW AND GPIO39 LOW; DMM/WS HIGH contacts confirmed. GPIO39 needs external bias.
- OLED: inspect actual text and choose Teks terlihat or Blank. Verify every module status rotates, including OLED's own result and build UTC. Visual PASS rejected if initialization failed.
- LEDs/DO: ON, observe current physical state, then Sesuai/Tidak sesuai. One observation finalizes QC; controls remain available without changing verdict. New session resets results. Check both DO ON together and independent OFF/timers. Heartbeat loss/Stop/OTA stops all. Physical observations remain mandatory.
- ADS1115 confirmed, AIN0 conditioning/VDD still must be verified. Never feed current directly into GPIO. Before calibration conversion100Ω/gain1/offset0 is provisional. At4/10/15/20mA (or edited references), capture16 new stable readings. Firmware rejects samples≥3.3V, invalid/stale/saturated and SD>30count. Save fit with max residual(default.10mA), R²≥.995, distinct points and broad span. Confirm gain/offset/CRC persist reboot. Reset points keeps active calibration. Test independent currents such as7/12/18mA with a reference and compare accuracy; do not treat fitted residual as independent QC accuracy. Interrupted NVS write must retain a valid previous/new blob, never affect production namespaces. Calibration lives in `qc-adc/cal-v1`, not PCB EEPROM. ADC/OLED acquisition pauses for up to5s while PD negotiation is prioritized; capture and PD request cannot overlap.
- I2C: Scan lists ACK addresses0x08–0x77 only. No scan-triggered protocol/measurement/init. Expected labels are pinlist guesses; unknown/ATtiny gets no invented command. AHT10 automatically initializes, six-byte data, no AHT20 CRC; separate temp/RH card, compare reference. DS3231 clock/temp read-only, OSF warning; verify ticks/date/12h/century and battery retention physically. Model confirmed by user, not proved by address.
- EEPROM: Write TOP`TMC_T_V2_R1` /BOTTOM`TMC_B_V3_R0`, click Read and check stored labels/generation/CRC, then reboot/read. Failed reads clear cached labels. Reserve final128bytes0x3F80–0x3FFF; foreign data rejected. Test WC, missing chip and interrupted inactive-slot write with previous valid record preserved. No manufacturing bytes elsewhere are altered.
- PD: use adapter that offers desired **fixed PDO**, rated cable/load/voltmeter. Request5/9/12/15/20V where supported. Check sink active count1/2, PDO1=5V, PDO2/3=target (PDO3 inactive); source capabilities, source RDO position and PE ready. Soft reset is queued in main loop after HTTP response to reduce lost Source_Capabilities. ADC/OLED work pauses while negotiating; TPL service continues. If no source message captured, UNVERIFIED is correct; retry, do not infer voltage from sink index. Physically measure VBUS and test unsupported voltage/current fallback. Reproduce reported12V→20V behavior. The root cause was not proved by source inspection; patch prevents stale PDO3 selection and adds evidence, but still needs hardware validation. IC reset/power loss reloads NVM defaults; runtime requests do not permanently provision NVM.
- Ethernet: SPI controller/PHY link with known cable, static documentation test IP192.0.2.2, no DHCP/packet acceptance. W5100 UNKNOWN link needs separate bench verification. Reset MCP2A1 untouched(unknown polarity). SD unique temporary write/read/delete + shared CS tests; no production file overwrite.
- RS485/RS232 raw binary transport tests now use the separate laptop tool described in COMMUNICATION_QC.md. Two-tag Modbus master now available; see current operator guide. Modbus config stays in namespace`qc-settings`; route selects MCP2A2 HIGH RS232/LOW RS485.
- Measure TPL5010 DONE MCP1B7 waveform/interval/high width under load/OTA; force main-loop failure and observe hardware reset. Software pulse count alone is insufficient. Keep MCP B3–B7 preserved by LED operations.
- OTA `/update`: Digest admin + public LAB default or private build override, one-use nonce2min, inactive2MB slot. Test wrong image, expired nonce, dropped/slow upload, power interruption, successful reboot/version/buildUTC; old app survives rejected/incomplete update. Watchdog/flash timing requires bench test. No bootloader/partition change via app OTA.

## Evidence boundaries

Confirmed by user: pinmap, MCP23017-E/ML, STUSB4500QTR, M24128/C7559, AHT10, ADS1115, DS3231 Normal logic and corrected output polarity. Unknown: conditioning transfer, physical fixture acceptance, Ethernet reset circuit, ATtiny protocol, actual PD source PDO availability. Software regression/compile/browser tests do not substitute for waveform/current/voltage/physical upload acceptance. Overall intentionally stays INCOMPLETE while deferred/data-only modules lack applicable acceptance evidence.

## Device variant and QC export

Choose **M0 or M1 before QC**. Reboot requires choosing again; variant/session/results are RAM only, not EEPROM hardware identity. Changing variant or starting a new session stops outputs and resets results/input counters/capture points, retains active calibration and EEPROM. Requests wait for any active PD negotiation to finish before reset. M0 includes PD; M1 marks STUSB4500 **NOT APPLICABLE**, excludes it from applicable/pending/fail counts, disables its request endpoint/controls and all PD register polling/writes. Generic all-address I2C ACK scan remains available. RS485/RS232 tests are implemented for both variants; the earlier deferral was superseded. Physical acceptance requires the external fixture and actual test evidence.

QC1 code format: `QC1-M0|M1-TMC<MAC>-PASS|FAIL|INCOMPLETE-<session8hex>-<CRC32hex>`. CRC32 covers version/build UTC, selected variant, serial, session and length-prefixed ordered test names/status/details/overall. It changes when evidence changes; it is a snapshot identifier, **not a signature, certification or guaranteed unique production serial**. Production serial remains TMC + STA MAC. Live ADC/RTC readings and export date/PIC are outside this evidence checksum. Code shown by API/dashboard and included with full JSON/CSV; code may represent FAIL/INCOMPLETE.

`GET /export/qc.csv?qcDate=YYYY-MM-DD&qcPic=...&productionDate=YYYY-MM-DD` returns a fresh server snapshot, UTF-8 BOM, RFC4180 quoting and CRLF. Profile must be selected. Dates optional, validated if supplied; PIC max64 bytes. Formula-leading user values are prefixed with apostrophe for Excel safety. Dashboard provides **Export CSV** and retains **Unduh JSON**. This is an actual .csv, not a renamed XLS/XLSX.

The first ten columns match Longhi's order: `serialNumber,productName,productType,productionDate,qcStatus,qcDate,qcPic,status,deliveredAt,deliveredTo`. Append QC code/schema/session, firmware/build UTC, EEPROM labels/validity, calibration/ADC/current, applicable/pass/fail/pending/N-A counts and each test's `.status`, `.detail`, `.applicable`. productType=M0/M1. Inventory status/delivery values remain blank because this firmware has no warehouse data. INCOMPLETE is preserved, unlike Longhi's boolean PASS/FAIL fallback. Missing PD on M1 is N-A, not FAIL. Data-only/deferred results still prevent overall PASS; CRC is never a substitute for completing tests.

Reference inspected read-only: Longhi [`fix/modbus` export](https://github.com/TaufikAS0/esp32-s3-longhi-board-test/blob/26f9a1fb7fd449d076c9b41ed0f0bf091163d0fb/02_Firmware/esp32s3_hwtest/web_server.cpp) at26f9a1f and feature/longhi-modular-upgrade3a45448. Remote develop9aa0fc1 currently lacks this exporter. No pre-existing QC code format was found, so QC1 is defined here. Longhi worktree/source were not changed.

## Historical output workflow — 2026-10-13

Every output starts OFF and has independent state/timer. ON on one does not turn another OFF. Command renews valid activity; loss of heartbeat3500ms still stops all. Per-output60s expiration stops only that output. MCP write/readback failure locks and stops all. Historical v1.3.0 polarity was LED MCP and DO active LOW; v1.5.1 supersedes DO to active HIGH. LED MCP remains active LOW. Visual QC accepts one Sesuai/Tidak sesuai observation after an ON attempt; current command must match observation. Final verdict stays while controls remain usable; new session resets it. No claim that both physical states were certified. I2C PASS/FAIL confirms the scanned list only; empty scan cannot PASS. No pin mapping/NVS/Wi-Fi/OTA credential changes.

## Update1.4.0

AHT10 status/busy/error recovery implemented; verify with installed firmware. Analog QC now has5 independent captures/confirmations, defaults4/8/12/16/20mA,16 fresh samples per capture. PASS requires all5 within tolerance and user agreement; calibration save invalidates evidence if fingerprint differs. Build08:42:43Z validated; no device upload. Actual board192.168.1.55 still1.2.0, EEPROM identity absent and PD28 absent from scan.

##1.5.0 installation additions

Install PubSubClient2.8 through Library Manager/arduino-cli. Verify WiFi wrong-password/SSID, MQTT wrong-auth/host, Ethernet DHCP/link/TCP, two slave tags/CRC/exception/timeout, serial fixture, output/watchdog RMW and Stop/OTA physically. Networking is a dedicated worker with exclusive SPI/settings ownership; main TPL loop continues. MQTT QC socket closes after accepted CONNACK; it is not a persistent telemetry service. Download CSV/JSON before session reset; CSV now includes analog point and Modbus tag fields.
