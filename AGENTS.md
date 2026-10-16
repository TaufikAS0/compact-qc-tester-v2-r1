# Guarded OTA1.17.0 branch

Read04_Dokumen/GUARDED_PORTAL_OTA.md for supplemental changelog/handoff. Isolated feature/portal-ota-guarded from8dbe087, no auto-merge into ongoing firmware branch or device upload. Archive evidence; no arbitrary checkpoint migration. Existing pins/NVS/public LAB policies retained.

ACCEPTANCE1.16.0 CURRENT V2.57: installed144/C28C build06:31:10Z exactOTA verified; general+PD confirmations are centered native modals, actual radio-reset/checkpoint review CANCEL tests preserve scroll/focus and ALL33PASS1FAIL own6F614 inclradioCNCFG1. No actual reset/radioGET/PD/RTC writes for UI tests. Full evidence comparison PASS/clean checkpoint. Ctrl+F5 required on old browser HTML.

Modal1.16.0 owner instruction: ALL existing confirmation/review panels use centered native dialogs (general AND PD); no scroll/page jump, Batal/Escape no mutation, keyboard loop, Stop accessible in popup. Preserve all actual33PASS1FAIL own6F614 incl valid radioPASS; no newGET/radio reset for UI tests. Upgrade ONLY exact1.15.1/build04:18:24Z->1.16.0. CURRENT V2.56 tracks finalbuild06:31:10Z;06:28 candidate cancelled.

ACCEPTANCE1.15.1 CURRENT V2.55: actual GET alone MENUNGGU SEND, M1 SEND enabled, real SEND bareOK+GET verified PASS, samebuild reboot/resume retains valid cyclePASS. Final NEW GET intentionally waiting operator SEND:32PASS1FAIL1pending own6F614, all unrelated evidence preserved, checkpoint clean. Latest acceptance supersedes pending-build notes; no profile reset/mask changes.

Radio1.15.1 actual E70 write ACK is bareOK; accept exact trimmed OK OR +OK, never substring/echo. GET-only still MENUNGGU SEND; both profile gates per1.15.0. Narrow upgrade ONLY1.15.0/build04:05:15Z->1.15.1. CURRENT V2.54 is actual status; realSEND failed old ACK parser, final verification pending.

Owner correction1.15.0: SINGLE radio QC GET->SEND->GET; GET alone MENUNGGU SEND neverPASS. radio_comm enabled authorizes SEND even legacy radio_configOFF, bothOFF denies. No profile/mask reset. Read RADIO_WORKFLOW.md and CURRENT V2.52. Invalidate legacy read-only radioPASS on resume; preserve unrelated results. Narrow exact1.14.1/build03:40:40Z->1.15.0 upgrade.

ACCEPTANCE1.14.1: CURRENT V2.51 installed144/C28C build03:40:40Z; actual Get7PASS and serial.csv browser download verified; own6F61433PASS1FAIL preserved. M1 writeOFF, physicalwrite untested, no profile reset. Latest acceptance supersedes earlier pending-build notes.

Radio1.14.1 actual root cause: E70V1.0 WMCFG replies Mode:4. Parse matching Mode alias ONLY index1 bounds0..4. Upgrade ONLY exact1.14.0/build2026-10-16T03:35:28Z->1.14.1. CURRENT V2.50 authoritative. Preserve own6F614; no profile reset/writes invented.

Owner clarification2026-10-16: radio Get auto-fills actual stored values, no CPU defaults; Send+Get changed fields requires ACK/readback match. Actual completed read/write failure FAIL; stale context/cancel never PASS. Temporary fullQC CSV filename serial.csv both browser/server; content unchanged. See CURRENT V2.48 for final build/deployment.

Radio1.14.0: read04_Dokumen/RADIO_WORKFLOW.md. Owner simple READ/EDIT/WRITE/READBACK. Keep stable radio test/profile IDs; partial read UNVERIFIED, draft mismatch not hardwareFAIL, read never writePASS. Only explicit changed fields + confirm/readId/context/preflight may write. Preserve current own144 session6F614ECF (NOT olderB6361F85); latest CURRENT authoritative. Upgrade ONLY exact1.13.1/build11:36:34Z->1.14.0.

Owner decision2026-10-15 / Warranty1.13.1: PASS OR FAIL may export Warranty; qcStatus MUST remain actual, no forced PASS. INCOMPLETE/busy/ON still blocked, exact10/noPCB/serial.csv unchanged. This supersedes historical PASS-only instructions below. Current upgrade exception ONLY exact1.13.0/build11:18:57Z->1.13.1 same unit; preserve all current evidence. See WARRANTY_CSV.md/CURRENT.

Module reset1.13.0: read04_Dokumen/MODULE_RESET.md. Preserve current target144 evidence25PASS/PCB0004/RTC revision1; current exact deployment in CURRENT. Reset per row only, shared RS485+tags/Radio explicitly reviewed, no calibration/EEPROM erase. Upgrade exception ONLY exact1.12.0/build08:59:41Z->1.13.0 same unit.

Warranty/PCB1.12.0: read04_Dokumen/WARRANTY_CSV.md. Warranty EXACT10 supplied columns/noPCB, actualPASS only and serial.csv; old fullQC CSV unchanged columns plus pcbNumber LAST. PCB session metadata/checkpoint only, no EEPROM/test reset. OWNER TARGET192.168.1.44 serialTMC88F1559CA28C is DIFFERENT from earlierTMC...478: NEVER transfer old unit PASS. Approved1.11.0->1.12.0 checkpoint exception same unit only. Actual installed state in CURRENT.

Owner decision2026-10-15: production RTC quick Write/Readback PASS is allowed after real different-time challenge AND verified restoration. Daily/60s/battery sampling separate; never fabricate their PASS. qcQuick=1 explicit reviewed mutation, same run/context guards. Preserve current results via documented exact-build checkpoint upgrade; no general cross-build bypass.

RTC1.11.0: read04_Dokumen/RTC_PORTAL_CONTRACT.md. New qc-rtc/qc-progress namespaces only. RTC writes explicit confirmed/readback, no automatic NTP. Persistent run locks, explicit same-build checkpoint resume, retained historical evidence never output state. Preserve existing33 PASS legacy1.9.1 via verified original bridge; do not fabricate results or clear data. Current progress/actual installed identity in CURRENT.

Station1.9.0: read04_Dokumen/QC_STATION_REVISION.md ST01..07. Own qc-recipes NVS schema1+CRC/revision, never erase old profiles/calibration. Preset loading form-only, active save resets session; recipe identity in reports/QCcode. Guided sequence uses explicit ON/OFF, two observations and firmware1..10s limit; Stop/session/disconnect cancels. Map is schematic verified pins, not PCB layout. Offline QR vendor MIT/provenance; QR is trace reference, never signature/PASS. Laptop archive imports append immutable reports locally, no COM/device contact.

Operator workflow1.8.0: read04_Dokumen/OPERATOR_WEB_REVISION.md WEB01..07. Operator/Technician is presentation only; profiles/calibration/pins unchanged. /api/qc/report requires PIC/date/session/revision/QCcode, guards worker/capture/stale result, returns snapshot+CSV from same evidence after OFF command. Browser archive50/no eviction/errors/exact downloads, not deviceNVS. RTC OSF =>UNVERIFIED; no RTC writes. Preserve reviews, Stop/epoch/disconnect cancellation, profile mask/revision exports. Never fake productionPASS/physical voltage or Write acceptance during UI tests.

PD revision1.7.0: read04_Dokumen/PD_CONTROLS.md. Fixed PDO only/no PPS; presets/slider draft-only, explicit review then confirm=1. Do not send physical voltage commands solely to validate UI; preserve M1 PD OFF. Node tests cover Stop during pre-confirm refresh/context expiry. See CURRENT for actual build/upload status.

# Compact QC firmware

Current revision1.6.1 includes persistent editable QC profiles (qc_profiles.* / qc_profile_math.h). Read04_Dokumen/QC_PROFILE_REVISION.md and CURRENT for source/build/upload status.36 stable switches, max16 profiles M0..M999; namespace qc-profiles/profiles-v1 only. M1 defaults Radio configuration/RS232/PD/unresolved EEPROM OFF; keep Radio communication and RS485 ON. If EEPROM Write/Read is physically verified, enable its M1 switch explicitly. Exclusions are IGNORED BY PROFILE, never PASS; all-OFF is INCOMPLETE. Enforce endpoints/results/inputs/PD/background acquisition plus JSON/CSV/QC-code profile snapshot. Active profile changes stop outputs and reset session; calibration/EEPROM retained. The historical DeviceVariant helpers are not the runtime policy; qc_profiles is authoritative.

User authorizes OTA and direct browser testing on192.168.1.31 for this revision. Gate exact app/version/build/hash first, use app-only OTA, verify same serial/new build after reboot; no NVS/storage/partition erase. AHT10 baseline E1 NACK; use real stage/Wire/frame errors, no inferred AHT20 commands. EEPROM baseline Read transport succeeds with EMPTY FF slots; missing identity is not bus failure. Keep actual Read/Write/soft-reboot evidence separate from physical power-cycle/load/accuracy acceptance. UI changes preserve stable output DOM, bounded queued clicks and priority Stop; never let a pending click run after Stop.

Missing vault rules were resolved through GitHub issue1. Complete Home/Rules_Versioning/Rules_Change_Notifications snapshots are there; related rules are in private TaufikAS0/2026-vault-firmware-rules at aa0bb9e03447afe4a3826516f7318a463c86551e. Read those accessible sources when the old local vault path is absent; no need to assume another laptop's reading applies here.

Resume entry: read CURRENT and first two updates in ai/branches/feature--qc-operator-ota.md, then ai/tasks/2026-10-13-qc-revision-backlog.md before new QC work. User resumed implementation after the 2026-10-13 13:52 UTC+7 checkpoint; latest continuation has no new deadline. Read the checkpoint for completed versus pending work. Update task IDs/checkpoint/worklog after each meaningful step and before stopping; record local vs pushed state and next action. Backlog supersedes prior serial deferral/read-only radio scope only when implementation is authorized; protect unrelated NVS and preserve hardware evidence boundaries.

Read README.md, VALIDATION.md and 04_Dokumen/TEAM_HANDOFF.md before editing/build/upload.
Use feature branches from develop; never push directly to main or merge without review.
Read workspace vault `01_Rules/Rules_Versioning.md` and `01_Rules/Rules_Change_Notifications.md` fully before firmware/build/push. Links are in `../2026 Vault Firmware Rules/00_Start_Here/Firmware_AI_Vault_Home.md`.
Version truth: compact_qc_tester/firmware_version.h::FW_VERSION. Target classic ESP32 only, 16MB.
Build ONLY through Build-Firmware.ps1: generate ignored UTC metadata and gate final binary/manifest. Never manually label a stale binary with a fresh timestamp. Record changes in CHANGELOG.md and handoff; report version before/after, actual build UTC, tests and upload status.
Keep sketch thin; app, hardware QC, session, Wi-Fi, watchdog and OTA remain separate.
User-defined QC: DI1-4 and DMM/Normal/Webserver auto PASS after two debounced HIGH-LOW-HIGH cycles. Normal means GPIO2 LOW AND GPIO39 LOW (user confirmed). No operator confirmation for inputs. User visual confirmations accept OLED/LED/DO observations; label evidence accordingly.
Do not equate ACK, output command, software pulse count, PHY link, regression residual or PD contract voltage with measured electrical acceptance.
User confirmed ADS1115, DS3231 and AHT10. Do not send AHT20 initialization/CRC commands to AHT10.
Calibration namespace qc-adc/cal-v1 is independent of production namespaces and PCB EEPROM. No erase/clear of unrelated keys.
User reopened serial QC on 2026-10-12: RS232/RS485 laptop binary QC now authorized. RS485 automatic TX/RX with no DE/RE confirmed. User authorized two-tag Modbus master; source1.5.0 implements it. Read04_Dokumen/QC_OPERATOR_GUIDE.md. Read 04_Dokumen/COMMUNICATION_QC.md. UART1 GPIO4/5, UART2 E70 GPIO16/17.
Preserve legacy partitions.csv. Owner decision 2026-10-12 permits validated portal merged USB / full erase for deliberate LAB reset: merged fast write resets NVS, full erase also deletes storage. Component USB and app-only OTA preserve unrelated NVS/storage. Never Arduino default 0x10000/boot_app0 offsets.
Private OTA secrets, binaries and machine backups must never be committed. Owner decision 2026-10-12 permits public lab_ota_defaults.h in source/release assets; PublicLabRelease refuses private overrides. Shared HuaweiJIN lab profile is authorized; AP recovery 12345678. Customer credentials require explicit profile.
Output update 2026-10-14: DO1 and DO2 active HIGH per latest explicit user confirmation of both inverted on1.5.0; LED MCP active LOW; profile flags enabled after confirmation. Verify runtime BANK/SEQOP and bench fixture. Preserve MCP unrelated bits, particularly TPL MCP1B7.
EEPROM identity region 0x3F80-0x3FFF is reserved for QC; never overwrite foreign bytes. STUSB PDO runtime only, no NVM write.
Run firmware compile and Node UI tests; hardware acceptance must be reported separately. Do not simulate a physical PASS.

E70-915NW14S uses CC1310/15.4 star, not LoRa. Seven documented AT readbacks are compared against editable factory reference targets, not assumed production configuration. Missing readbacks are UNVERIFIED; unsupported SF/BW/CR/LoRaCRC are never PASS. Radio parameter writes are allowed only through explicit Apply config (apply=1), per latest user request; never automatic factory reset/restart. Document partial writes and settings needing restart. Restore only MCP1B4/B5 mode bits/direction, preserve TPL B7, including Stop/OTA. Serial/radio tests block conflicting mutations. Local laptop backend binds loopback, opens adapters only on explicit Start, excludes tokens from JSON/CSV and keeps reports ignored. Run Python protocol/worker tests too.

QC variant is mandatory before mutations: M0 full / M1 STUSB4500 unpopulated N-A, no PD register I2C; generic ACK scan allowed. Reject PD on M1 server-side; changing profile resets session and waits for active PD. Preserve N-A applicability in overall/counters/CSV. QC1 code is evidence CRC, not security signature or PASS; never coerce INCOMPLETE/deferred/data-only to PASS. CSV keeps first ten Longhi columns, UTF8 BOM/RFC4180 and Excel formula protection.
