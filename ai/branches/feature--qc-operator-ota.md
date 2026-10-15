CURRENT | V2.40 | 2026-10-15 | Codex | Installed1.13.0/build11:18:57Z on144/C28C; direct all-module/DI/DO reset + summary PIC/clickable error help; compile/software/live reset/reboot PASS | Own M1/sessionB6361F85/PCB0004/34PASS/0FAIL/4ignored, RTCquick rev1 and raw analog/Modbus exact/clean checkpoint; source397858b pushed beforeOTA, final docs-only push/no merge
READING RULE | Read the CURRENT line and the first two entries under Latest Updates before editing.

# Branch Worklog

- Branch: `feature/qc-operator-ota`
- Project: `compact-qc-tester-v2-r1`
- Purpose: checkpoint pekerjaan QC agar dapat dilanjutkan setelah interupsi.
- Task source: [Backlog revisi QC](../tasks/2026-10-13-qc-revision-backlog.md).
- Sequence V1.1 adalah versi worklog, bukan versi firmware. Firmware baseline tetap1.2.0.

## Latest Updates

### V2.40 | 2026-10-15 | Codex

# Module reset / Operator / error navigation1.13.0 - installed and verified

Installed via authorized app-only OTA192.168.1.44, original1.12.0 ->1.13.0/build2026-10-15T11:18:57Z /18:18:57 WIB. App1544496 bytes SHA256e58ac6ef069863e82df7c821ebe36d6456729ba07658029f56ba4293703bcb91; runtime/fileMD596356ffdcefeb5ff17cd31f0cc7dd20b. Source397858b published on feature/qc-operator-ota BEFORE OTA. HTTP200, same serialTMC88F1559CA28C/classicESP32-D0WD-V3, profiles/calibration/hash/build verified; no erase/pin/polarity/WiFi/partition change. Build metadata/legacy gates, Node UI/error/direct reset regressions, Python14, portal packaging2 and compiled dependency assertions PASS. No firmware edits after final build started.

Owner reconnected after timeout and had rebooted1.12.0: own saved sessionB6361F85 now33PASS/1RadioFAIL/PCB0004/RTCquick revision1. Original checkpoint/state/CSV archived, same-build resume performed BEFORE upload then latest checkpoint saved again. Explicit approved1.12.0->1.13.0 upgrade/readback/resume preserved EVERY original PASS/FAIL/detail/PCB/session/RTC receipt. Older25PASS and other-unit34PASS are not current evidence. Negative actual API token403/confirm400/unknown400/session409/profile409/staleQCcode409/disabled409/RTC revision409 rejected without changing evidence.

Actual installed browser: missing PIC error click focuses Operator field in Ringkasan QC; direct DI1 and DO1 buttons open exact one-channel reviews, then cancelled to preserve their existing PASS. All7 input/11LED/2DO and other module cards/summary reset controls present. Actual Radio FAIL reset confirmed: affected Radio communication+configuration; communication becomes BELUM DIUJI, configuration remains IGNORED BY PROFILE, reset receipt and checkpoint saved/readback; all original33 PASS/details unchanged. Then explicit READ & VALIDATE (no Apply config) obtained real E70 AT/version/channel CNCFG0 readback and communication PASS. WMCFG unreadable; configuration stays profile-ignored, no configuration/RF/coverage PASS claimed. Latest own unit now34PASS/0FAIL/0pending/4ignored/overallPASS. Operator remains empty for the real user to fill; no invented PIC/dates/Warranty report.

Identical-binary second OTA reboot -> explicit same-build resume -> save/readback: all34 statuses/details, latest reset receipt, original RTCquick/revision1, PCB0004/sessionB6361F85 and clean checkpoint verified. Original pre-upgrade raw analog and Modbus evidence compare EXACT to final checkpoint; final fullQC CSV PCB0004 verified. No output ON, EEPROM write, PD request, radioApply or RTC/calendar write performed. Evidence ignored build/module-reset-pre*, -found-after-owner-reboot, -pre-upload*, -post-ota, -negative-checks, -radio-applied, -after-radio*, -verified.json/-final.csv. Installed UI screenshots module-reset-live-operator-v1_13_0.jpg, -live-do-review-v1_13_0.jpg, -final-v1_13_0.jpg; earlier preview screenshots are explicitly simulations.

Usage: Ringkasan QC ->Operator/PIC; click error to open required field/module; Reset QC on the relevant card/DI/DO/LED or Summary ->Semua ->row ->review ->confirm. Reset affects the listed evidence; shared RS485+tags and Radio groups are explicit. Save progress before power-off and Resume before new session. RTC evidence reset blocks active run/tick and does not set time. Remaining physical limits: DI/DO destructive reset was software-tested plus live review/cancel, not cleared on this completed production session; real24h/battery/RF/voltage accuracy not certified. No Warranty import tested. Main35733ba unchanged/no merge; final docs-only publication follows, no rebuild/extra OTA required.



### V2.39 | 2026-10-15 | Codex

# Module reset / Operator / error navigation1.13.0 - software verified, OTA blocked by offline target

Final source1.13.0/build2026-10-15T11:18:57Z /18:18:57 WIB compile/metadata/legacy partition gates PASS. App1544496 bytes SHA256e58ac6ef069863e82df7c821ebe36d6456729ba07658029f56ba4293703bcb91; sketch1544353 bytes73%, staticRAM57480 bytes17%. No firmware source edits after final build began. Candidate09:28:18Z is superseded and was never uploaded. Node UI tests cover direct DI/DO/all-module review, Stop/context/QCcode race, profile exclusions, RTC run locks, operator draft/focus, error destinations and previous CSV/RTC regressions; Python14 and portal packaging2 PASS, static dependency scope assertions compiled, diff check PASS.

User-facing: fill Operator/PIC in Ringkasan QC; report page links/focuses that field. Click error text to open/focus the required field or module; generic errors open the current workflow for diagnosis. Reset QC is directly on all7 input cards,11 LED rows,both DO rows,every other result card and all38 summary rows. Ignored tests stay disabled; RS485+tags and Radio groups explicitly reviewed. Reset clears selected result/counter/observations/raw QC only, preserves calibration/session/PCB/unrelated results, stops outputs and saves/readbacks checkpoint. RTC reset remains evidence-only/clockRevision and blocks active run/tick, no calendar write. Read MODULE_RESET.md for API/507 behavior.

Actual in-app browser PREVIEW only: missing PIC error click focuses summary Operator input; direct DO1 reset opens correct one-channel review/cancel; all DI1..4/switch buttons and all module cards visible. Ignored build/module-reset-preview-operator.jpg/-do-review.jpg/-di.jpg. This is software/UI proof, NOT hardware reset/PASS/OTA evidence. Real192.168.1.44 timed out twice during final build; owner asked asynchronously to reconnect/provide new IP. Last actual device1.12.0/sourcebuild08:59:41Z, TMC88F1559CA28C/sessionB6361F85/PCB0004/25PASS/0FAIL/9pending/4ignored/RTCquick revision1 saved and archived. No1.13.0 OTA, no actual module reset or reboot test yet. Never overwrite these results or reuse old unit34PASS.

Continuation: source feature publication first; when target responds, re-check same unit/chip/outputOFF/TPL/profile/calibration/no active workers/RTC run; save latest evidence again because operator may have added results. Sibling _codex_tmp/module_reset_runtime.py upload gates exact final11:18:57Z artifact and archives pre-upload state/checkpoint, uses app-only OTA/no erase; then resume explicitly ONLY exact1.12.0/build08:59:41Z->1.13.0, or same-build resume. Run verify to compare EVERY prior PASS/FAIL/detail/PCB/session/RTC receipt/revision; module_reset_checks.py negative guards. Test actual reset only a still-pending module via UI, preserve completed operator QC. Then identical binary OTA/reboot/resume and verify again. No hardware PASS fabricated, no RTC reset/EEPROM write/PD request/output ON to test UI. main35733ba unchanged; feature only/no merge.



### V2.38 | 2026-10-15 | Codex

# Module reset + actionable errors1.13.0 - final build pending

Owner steering: Operator/PIC field moved from report page to Ringkasan QC, report button jumps/focuses it; polling preserves drafts and loads saved operator per session. All error banners are keyboard/click actionable with field/module destinations: PIC, PCB, QC/production dates, QC pending, session/profile, RTC/AHT/analog/PD/EEPROM, serial/network/SPI and report/archive. Endpoint context accompanies server errors; unknown errors open the current workflow for inspection, not an invented root-cause diagnosis. FAIL badges open the related test page. Rendering remains text-only.

Direct reviewed reset buttons now on EACH DI1..4, DMM/Normal/Webserver input, EACH LED and DO1/DO2, every result card and all summary rows. Shared dependency/RTC/checkpoint policies remain MODULE_RESET.md. First09:28:18Z build compiled/gates PASS but is NOT final and was NEVER uploaded; subsequent UI edits require new final build. Final frozen build2026-10-15T11:18:57Z /18:18:57 WIB running, no further firmware edits. Node operator focus/draft/error/direct DI/DO/module reset and all earlier UI tests PASS. Actual baseline25PASS/PCB0004 retained; re-save latest before OTA. Feature push/OTA/explicit upgrade, runtime negative guards, actual pending-module reset and same-build reboot still pending.



### V2.37 | 2026-10-15 | Codex

# Module reset1.13.0 - implementation/build acceptance pending

Owner requests reset QC per module plus push/OTA. See04_Dokumen/MODULE_RESET.md: per-row reviewed reset preserves session/operator/PCB/calibration/other results, stops outputs and saves/readbacks checkpoint. RS485+two tags and Radio communication/configuration are explicit shared groups; network tests/input/output rows independent. RTC retains checked evidence-only reset, revision increment and active-run/tick lock; no clock write. Token/confirm/session/profile/QCcode guards prevent stale reset. Receipt in state/checkpoint/QCcode,507 reports RAM-only reset when save fails. Ignored tests remain ignored.

Live baseline192.168.1.44/TMC88F1559CA28C checked1.12.0/build08:59:41Z. User added evidence: latest backup25PASS/0FAIL/9pending/4ignored/sessionB6361F85/PCB0004/RTCquickPASS revision1, not previous1PASS checkpoint. Preserve ALL original accepted details/RTC receipt; ignored build/module-reset-pre.json/.csv/-checkpoint.json archived after explicit save. No other-unit evidence. Only approved exact1.12.0->1.13.0 resume; no erase. Source frozen build2026-10-15T09:28:18Z started; firmware untouched afterward. Node reset/Stop/race/sampling/profile and prior UI PASS; Python14/portal packaging2 PASS. Final compile/gates, feature push, OTA/upgrade/runtime reset/reboot/browser verification pending. main35733ba unchanged.



### V2.36 | 2026-10-15 | Codex

Firmware on replacement M1 at192.168.1.44 upgraded1.9.1 ->1.12.0, actual build2026-10-15T08:59:41Z /15:59:41 WIB. App-only OTA HTTP200; actual serialTMC88F1559CA28C/build/runtime MD5 matched, profiles and inactive calibration preserved, no erase. Feature source d1ef3d2 was pushed BEFORE OTA. App1530208 bytes SHA256cf55d345afa0215f6daf1ac1c5a2b5ac6985276ad6a9c1cebed8da2c47a31635, MD5 7a17c29e84860296b63d37805c2ccd72. No firmware edits after final build began.

Actual Operator browser created this unit's own M1 rev1/sessionB6361F85. Temporary PCB000TEST-PCB saved/read back, full QC CSV preserved all previous columns in order with pcbNumber LAST, and the same session/PCB survived identical-binary OTA reboot and explicit resume. Temporary PCB then cleared and checkpoint saved/read back clean; final PCB empty awaiting actual production number. Actual browser download C:/Users/taufi/Downloads/Compact_QC.csv verified this serial and empty final PCB. Screenshot/evidence remain ignored under build/warranty-144-*.

Actual Warranty endpoint correctly rejected incomplete QC409, missing PIC400, invalid date400 and production-after-QC400. Exact10 header/serial-only filename/successful PASS row are software-tested; live successful Warranty download/import remains unverified because this replacement is only1PASS(AHT communication)/0FAIL/33pending/4ignored/overallINCOMPLETE. No manufactured PASS, no old unit34PASS copied. RTC clockRevision0/OSF active, no quick RTC/time write on this unit. No output ON, EEPROM write, PD request or radioApply. Build/Node UI/Python14/portal packaging2/date+PCB assertions PASS. main unchanged35733ba; feature only, no merge.

- Continuation: keep this unit sessionB6361F85; enter real PCB via Ringkasan QC ->Tinjau & simpan. Resume saved session after reboot before creating any new session. Complete actual production QC and PIC/dates before CSV Warranty; do not fake PASS to exercise export. Historical1.11.0/34PASS below belongs to different serialTMC...478, not this target. Do not rerun temporary metadata/prepare helpers or old-unit RTC migration scripts. Final docs-only publication follows; no rebuild/extra OTA needed.


### V2.35 | 2026-10-15 | Codex

- Final1.12.0/build08:59:41Z compile/metadata/legacy gates PASS, app1530208 SHA256cf55d345afa0215f6daf1ac1c5a2b5ac6985276ad6a9c1cebed8da2c47a31635, sketch1530061 bytes72%, staticRAM57416 bytes17%. No firmware edits after build began. Node UI metadata/Warranty/legacy/RTC regressions PASS; Python14 and portal packaging2 PASS, static calendar/PCB/date assertions compiled. Owner confirmed Warranty exact10/no PCB; docs WARRANTY_CSV.md matches. No actual Warranty web/import tested or requested.
- New M1 target144 original1.9.1/unselected0PASS archived; baseline TMC88F1559CA28C, not olderTMC...478. Same classic chip/board/TPL/outputOFF confirmed. Prepare helper ota_warranty_144.py gates exact artifact/newserial/sourcebaseline, old profiles/calibration and no erase. Publish feature first, then authorized app-only OTA, select NEWM1 session, test PCB metadata+oldCSV and Warranty rejection while QC incomplete, reboot/resume, clear temporary PCB test label. Successful PASS Warranty body/filename conformance is software-tested; never fabricate new hardware results to force a live success. Runtime acceptance/publication next.

### V2.34 | 2026-10-15 | Codex

- Owner asks separate Warranty CSV matching D:/inventory-2026-10-15.csv and serial-only filename; preserve previous CSV format, add PCB input to summary and exports. Clarified answer: PCB only in full QC CSV; Warranty EXACT original10 columns. Defaults Telemetric Module Compact / Monitoring and Control / INVENTORY, delivery fields blank. ActualQC status requiredPASS, operator/date/production requiredvalid and production<=QC; example personal names/dates never copied. UTF8 no BOM for Warranty (sample), CRLF/RFC4180 and formula-safe; old fullCSV retains BOM and all existing column order, pcbNumber appended last.
- New /api/qc/pcb confirmed token/session/revision/expected previousPCB mutation saves/readbacks own checkpoint, no test reset/time write. PCB max64 text/control guard/leadingzero preservation, JSON/snapshot/print/archive and QCcode provenance. New session clears PCB, resume restores. UI protects unsaved drafts/review and exports separate header+serial match. Explicit1.11.0/build08:16:55Z ->1.12.0 checkpoint upgrade only, same identities/no RTCrun, retains previous upgrade provenance; arbitrary cross-build resume still blocked.
- Original192.168.1.31 HTTP timeout BEFORE backup/OTA; no mutation. OWNER redirected target to M1 at192.168.1.44. Actual GET identifies DIFFERENT serialTMC88F1559CA28C/classicESP32-D0WD-V3, source1.9.1/build00:44:55Z, unselected0PASS/0FAIL/INCOMPLETE, outputsOFF/faultfalse/TPLready; sanitized original state archived ignored build/warranty-144-pre-state.json. NEVER migrate old TMC...478 results here. New target EEPROM identity invalid; no Write attempted. Select freshM1 after OTA only; do not fabricate physical PASS to exercise Warranty download.
- Rules/AGENTS/CURRENT/latest2/task/README/VALIDATION/HANDOFF and full issue1 vault snapshots read. Clean feature9bb404d/fetched at start, main35733ba unchanged. Node PCB draft/review/leadingzero + Warranty header/filename/PASS/date/unit guards PASS. Calendar/PCB validation assertions added. Final source frozen Build-Firmware.ps1 started08:59:41Z; no later firmware edits. Sibling _codex_tmp/ota_warranty_144.py exact target/build; compile/OTA/newM1 metadata/CSV/reboot/native/push pending. No portal/web Warranty code or imports authorized/changed.

### V2.33 | 2026-10-15 | Codex

- Final quick production QC1.11.0 source/tests/contract/evidence published3d45c9d on feature/qc-operator-ota. This docs-only publication checkpoint follows; no firmware edits/rebuild/OTA after V2.32. Final HEAD/origin/ls-remote parity checked after push; main35733ba unchanged, no merge, portal repository untouched.
- Owner fast-QC scope complete: actual Operator write challenge/readback/restoration PASS, automatic checkpoint and same-build reboot/resume verified. Daily/battery/60s sampling separate and NOT claimed. Current sessionEB4CDCB2/revision2,34PASS/no outputs/no active run/checkpoint clean. Do not rerun legacy migration or reset clock casually. UI proof build/rtc-quick-final-v1_11_0.jpg ignored; original state/CSV/checkpoint archived. Use same-build Resume after future reboot, explicit Save before power-off.

### V2.32 | 2026-10-15 | Codex

- Owner requested fast production Write/Readback QC; daily/battery checks remain separate sampling. Final1.10.6 ->1.11.0, actual build2026-10-15T08:16:55Z /15:16:55 WIB. App1519696 bytes SHA256dfeb0873e23e5b79378517a434404967b292634bff5ee23bd6642f30d8aaab5f, runtime/fileMD5ef0d6918932ae6ff356e979950fd0fe0. Build-Firmware.ps1/metadata/legacy partition gates PASS; sketch1519541 bytes72%, staticRAM57400 bytes17%. Authorized app-only OTA HTTP200, same serial/calibration/profiles retained; no erase, GPIO/WiFi/polarity/partition changes. No firmware edits after final build start.
- Operator QC cepat button: choose reviewed zone/time, explicit confirmation; existing /api/rtc/time accepts qcQuick=1. Writes a calendar challenge120/240s different from actual previous time, verifies readback, always attempts correct-time restoration and verifies again. Both required for quickPASS; cannot pass by merely reading an already-correct clock. Bounds/ownership/auth/context/clockRevision/OSF guards remain. Immediate progress save/readback; returned507 never means durable save. Normal time sync without qcQuick cannot create quick PASS. Earlier RTC evidence is invalidated only through reviewed write/reset; other QC untouched.
- Production criterion WRITE_READBACK is explicit in state/report/checkpoint. QC-RTC-QUICK-1 receipt contains challenge/final requested+read UTC, source/offset/session/revision and verified flags. QuickPASS satisfies production RTC; sampling60s/daily/retention remain separate, not fabricated. Real hardware/OSF/sampling failures still override quickPASS. Readback/calendar/challenge boundary assertions compile; Node quick-review/zone/revision race/sampling lock/approved-upgrade UI tests PASS, Python14 PASS, portal packaging2 PASS, diff check PASS.
- Before upgrade actual1.10.6/sessionEB4CDCB2/33PASS checkpoint saved and state/checkpoint/CSV archived ignored build/rtc-quick-pre-*.json/.csv. Explicit compatible resume permits ONLY exact approved1.10.6 build ->1.11.0 with same serial/calibration/profile/preset/clockrevision and no run; writes/readbacks new header and source provenance before restoring. Default/missing or wrong source rejects. Original33/status/details/raw analog+Modbus and82 CSV columns exact. No legacy1.9.1 import or invented quick evidence.
- Actual installed in-app browser modeOperator ->QC cepat/review/WIB420/confirm: real challenge+final readbacks within0..1s, actual clockRevision2, quickPASS, overall34PASS/0FAIL. Checkpoint saved immediately; samplingComplete false, tick/external sampling absent after the reviewed clock write. API negative token/confirmation/revision/qcQuick/UTC/run-lock cases reject BEFORE writes. CSVrtcEvidence contains exact quick receipt. No output ON/EEPROMwrite/PD request/radioApply.
- Identical-binary OTA reboot, explicit same-build resume and explicit save after live RTC re-evaluation: quick receipt/34PASS/revision2/WIB and all33 historical evidence/82 CSV fields survive; outputOFF/faultfalse, no active run, checkpoint clean. Initial final-check assertion saw dirty=true from live RTC row re-evaluation after resume, not lost/corrupt data; explicit save/readback completed and full check passed. Evidence ignored build/rtc-quick-verified-v1_11_0.json/.csv and rtc-quick-after-reboot-v1_11_0.json/.csv. Do NOT rerun --prepare, legacy import or reset RTC casually now.
- Portal contract updated COMPACT-RTC-1 with productionCriterion/quick/samplingComplete and approved upgrade semantics; no portal code edited. No24h accuracy, battery/supply-removal or independent laptop absolute-time certification. PIC remains original/empty; no fabricated production report metadata. Final live Operator UI after reboot shows quickPASS, sampling separate and clean checkpoint; screenshot ignored build/rtc-quick-final-v1_11_0.jpg. Feature-only publication next, main remains35733ba; no merge. Operator procedure: I2C & Sensor ->zone ->QC cepat: Write + Readback ->review/confirm. Save/download report as usual; sampling via portal separately.


### V2.31 | 2026-10-15 | Codex

-1.11.0/build08:16:55Z compile/metadata/legacy gates PASS, app1519696 SHA256dfeb0873e23e5b79378517a434404967b292634bff5ee23bd6642f30d8aaab5f, sketch1519541 bytes72%, staticRAM57400 bytes17%. Authorized OTA HTTP200 actual version/build/serial/runtime MD5ef0d6918932ae6ff356e979950fd0fe0 verified, calibration preserved. Source unchanged after build started.
- Runtime prepare: ordinary cross-build resume and unknown upgrade rejected; exact approved1.10.6 migration/readback/resume preserves all33/raw points/Modbus/coefficients, source provenance and old60s evidence; no quick PASS manufactured. Auth/missing confirmation/stale revision/invalidqcQuick/out-of-rangeUTC/run-lock cases reject before writes.
- Actual in-app browser modeOperator QC cepat button/review/WIB420/confirm succeeds. API verifies real challenge +/-120/240s readback0..1s plus reviewed real-time restoration0..1s, newclockRevision2, quick resultPASS/schema/scope/session, metadata valid/OSFclear. Actual34PASS/0FAIL/overallPASS; ALL33 prior statuses/details and82 CSV analog/Modbus columns remain exact. Sampling tick/external results intentionally absent after reviewed new write; samplingComplete false, no fake daily/battery acceptance. Quick checkpoint saved/read back automatically and CSVrtcEvidence contains exact quick receipt.
- Same exact binary OTA reboot now running; then sibling _codex_tmp/rtc_quick_runtime.py --after-reboot. NEVER rerun actual quick/migration script casually now (expects revision2); prepare phase is complete. Preupgrade snapshot/checkpoint/CSV and first OTA evidence remain ignored and safe. Next finish resume proof, screenshot, finaldocs and feature publication. No portal edits/output ON/EEPROM write/PD/radioApply.

### V2.30 | 2026-10-15 | Codex

- Owner explicitly changes production RTC acceptance: quick functional Write/Readback now enough; daily accuracy/battery handled by later sampling. Source1.11.0 adds operator QC cepat button with zone/time review/confirmation, qcQuick=1 on existing time route. Real different calendar challenge (+/-120/240s), verified readback, always attempt reviewed real-time restoration and readback; both required PASS. Clears ONLY previous RTC evidence/revision, not other QC. Immediate checkpoint/readback; reports expose QC-RTC-QUICK-1/scopeWRITE_READBACK and samplingComplete separately. Real sampling failures/OSF/calendar faults still override PASS. Normal time sync alone cannot create quick PASS.
- Preserve current33: actual live1.10.6/sessionEB4CDCB2/revision1 checkpoint saved and sanitized state/checkpoint/CSV archived ignored build/rtc-quick-pre-*.json/.csv. No active run/tick/output. Explicit resume upgradeFromBuild permits ONLY exact QC_BUILD=1.10.6|2026-10-15T06:57:45Z ->source1.11.0, same serial/calibration/profile/recipe/clockrevision and no active run; NVS CRC/readback and provenance retained. Other cross-build restores still rejected. Never import legacy1.9.1 now.
- Read AGENTS/CURRENT/latest2/task/README/VALIDATION/HANDOFF and full issue1 Versioning/ChangeNotifications/Home; approved WiFi/pins/polarity/partitions kept. Feature clean/fetched at7dbc7b9 before edits. Node UI quick review/zone/revision/sampling lock/compatible-upgrade regressions PASS. Pure C++ calendar challenge/readback boundary assertions added. Source frozen build08:16:55Z running. No firmware edits after start. Sibling _codex_tmp/ota_rtc_quick.py targets exact final artifact; OTA/actual quick/UI/reboot acceptance pending. No portal source changes.

### V2.29 | 2026-10-15 | Codex

- Final firmware corrections, installed evidence and portal contract published b01243e on feature/qc-operator-ota. This docs-only publication checkpoint follows; no firmware edits/rebuild/extra OTA since V2.28. Final HEAD/origin/ls-remote parity checked after push, main remains35733ba with no merge.
- RTC firmware scope complete. Real24h reference accuracy/physical battery test and portal integration remain explicitly pending, owned by operator/portal task. Do not reset RTC or import old legacy results into revision1. Retain saved sessionEB4CDCB2, use explicit save before power-off and same-build resume afterward.

### V2.28 | 2026-10-15 | Codex

- Installed final1.10.6/build2026-10-15T06:57:45Z /13:57:45 WIB. App1513328 bytes SHA2569fa0b047d6a49c3355da12793bfe61930377e7c131fa91017a5e195e60a9a9e7; runtime/fileMD52195e3d16f29b88cdec16a7f477c46ba. Build-Firmware.ps1 metadata/legacy gates PASS; sketch1513177 bytes72%, staticRAM57336 bytes17%. Authorized app-only OTA HTTP200, same serialTMC88F1559CA478/calibration/profiles preserved, no erase. No firmware edits after final build began.
- Fixed null runId falsely interpreted as text null/active; initial legacy guard now diagnoses individual conditions, avoids full qcState allocation for profile/recipe validation. Original migration uses required-field projection11420 bytes and multipart, preserving EVERY original CRC-covered identity/result/detail and raw evidence. Full old URL-encoded payload hit ESP32 heap/transport limits; do not claim50KB guaranteed capacity. Negative CRC/coefficient/captured calibrationID each rejected for correct reason; real original import/NVS readback/resume PASS.
- Original sessionEB4CDCB2 restored with ALL33 historical PASS/details, outputsOFF, no fabricated PIC/acceptance. Actual RTC UTC set+WIB420/readback/OSF clear/revision1 PASS; auth, confirmation, stale context, bounds and microsecond timestamp brackets PASS. Actual60s monotonic ticking PASS; RTC overall remains UNVERIFIED because accuracy/retention evidence is absent. No24h or physical supply-removal/battery PASS.
- Run bind blocks time/reset/new-session, malformed evidence rejected, abort releases lock, explicit checkpoint replacement/readback retains complete schema/build/profile/calibration/session headers. First identical-binary OTA reboot then explicit resume restores same33/tick/timezone/revision. Separate OWN administrative run CODEX-PERSISTENCE-CHECK survives second identical-binary reboot and resume, then explicitly aborted; no real portal run disturbed, no external evidence submitted. Normal OTA helper still refuses real active runs; isolated same-build exception was only our named administrative check.
- Final independent comparison after BOTH reboots: original analog points and both Modbus tag evidence equal original, all82 AnalogPoint*/ModbusTag* CSV columns exact, original JSON hash99fe1e83761fa93e1fb157afc51bb68080e26e8479b8e23c7548b5d98768a8fb unchanged. Final33PASS/0FAIL/1RTC pending/4ignored, checkpoint clean, outputsOFF/faultfalse, no active run, calibration retained. Evidence ignored build/rtc-final-v1_10_6.json/.csv, rtc-after-reboot-v1_10_6.json, rtc-lock-*.json; first OTA files preserved separately.
- Node webUI regression PASS, Python14 PASS, portal packaging2 PASS, compiled calendar/tick/evidence assertions PASS. Native Chrome screenshot observed installed1.10.6/AHT values; native accessibility/input cached wrong portal tab, so further semantic checks used separate in-app browser. Live browser verified activeM1/rev1/session, clean checkpoint, summary33/0/1, RTC running-test LULUS and accuracy/retention pending, Technician WIB/time-review panel. No additional clock reset/outputs from browser. Screenshot ignored build/rtc-final-summary-v1_10_6.jpg. No mobile/printer or independent absolute time certification.
- Portal owner can integrate COMPACT-RTC-1 in04_Dokumen/RTC_PORTAL_CONTRACT.md; portal repository not edited. Before physical power-off: save progress successfully; after reboot use explicit Resume before NEW profile/session. Same exact firmware/build/profile/preset/calibration required. Autosave may lag60s; one device checkpoint, archive reports externally. DO NOT reimport legacy or reset RTC now (clockRevision1 and real tick evidence). Portal24h/reference/battery procedures still separate pending work. Feature-only publication next; main remains35733ba, no merge.


### V2.27 | 2026-10-15 | Codex

- Final1.10.6/build06:57:45Z compile/gates PASS app1513328 SHA9fa0b047d6a49c3355da12793bfe61930377e7c131fa91017a5e195e60a9a9e7. Authorized OTA HTTP200 same serial/calibration/runtime MD52195e3d16f29b88cdec16a7f477c46ba. No firmware edits after build freeze. Null-runId false lock fixed; source-version diagnostic probe identified it, not a real bound portal run.
- Runtime patch6 required-field projection11420 bytes (unchanged original CRC/evidence) via multipart: negative CRC, changed coefficients and captured calibrationID each rejected with exact reason; original import/NVS readback/resume succeeded. ALL33 original PASS/status/details/sessionEB4CDCB2 and calibration preserved, restoredEvidence marked, outputsOFF. Original archive remains unchanged. Full old form payload is memory-heavy; use verified projection/multipart for this bridge, not reconstructed evidence.
- Actual confirmed UTC set/readback with offset420/WIB succeeded: OSF cleared, writeVerified/timeValid true, clockRevision1. Auth/confirmation/stale revision/bounds and sample timestamp brackets PASS. Actual60s ticking now running in helper process (exec session76152); no24h/battery proof. DO NOT rerun full runtime script/reimport legacy/reset after revision advances. Finish current process/run guards/checkpoint headers/CSV, then exact SAME1.10.6 binary OTA reboot and --after-reboot. Preserve before-reboot evidence separately before replay overwrites OTA files. Final native check/docs/push pending.

### V2.26 | 2026-10-15 | Codex

-1.10.5/build06:53:19Z compile/metadata/partition gates PASS app1513376 SHA8789390aa9054f3ea341e9ca628d35b4b787a33075c9fcb7e7abcbeafd7169bb; authorized OTA verified runtimeMD51bea5a4d1ddfc20ddec5bfe04f392ab1/same serial/calibration. Diagnostic pinpoints false active RTC run: unset JsonVariant runId.as<String>() produces text null, length4, so rtcRunActive wrongly true even when no run. No RTC hardware writes, revision0, no legacy replacement happened.
- Source1.10.6 uses explicit empty-string default and direct const char pointer for rtcRunActive; null no longer means active. Source frozen build06:57:45Z running. Helpers patch6 exact target. Next gated OTA/migration/negative CRC+calibration diagnostics/actual60s/run/checkpoint/CSV/reboot/native. Preserve original33 and unrelated NVS. Source local since89bbf75; no24h/battery acceptance.

### V2.25 | 2026-10-15 | Codex

- Owner says online again; actual GET/state and Digest readiness succeed. Authorized1.10.4/build06:42:50Z app-only OTA HTTP200, exact serial/build/runtime MD5cdf7d5cc7fa10852d18617aaf740c3ac and calibration preserved. No erase, outputsOFF. Full legacy JSON form rejected IncompleteInput; required-field-only projection11420 bytes parses but valid import rejects combined initial guard. Projection only omits unrelated catalogs/live UI; all original CRC-covered identities/results and evidence remain unchanged. Multipart attempt same guard, no clock writes/NVS replacement/session selection.
- Source1.10.5 splits guard failures into explicit source-version/serial/idle/run reasons and validates profiles/recipes without constructing full live qcState tree. Build06:53:19Z running; no edits after source freeze. New sibling helpers patch5 target exact timestamp, original archive unchanged. Next gated OTA then migration/runtime/actual60s/reboot/native proof. Read V2.23 for full sequence; no new RTC/24h/battery PASS. Source changes local since published89bbf75.

### V2.24 | 2026-10-15 | Codex

- Implementation/contract/tests/checkpoint published b577e0e3a7259805b4a510a929d8c73ffccdc922 on feature/qc-operator-ota. HEAD/origin/ls-remote matched; main remains35733ba17a8e790fc68dafe9ba771295329a26fd, no merge. This docs-only publication checkpoint follows; no firmware edits/rebuild/OTA after V2.23 final artifact.
- Third HTTP probe still timed out. Owner asked current device IP asynchronously; answer pending. Cannot honestly claim new OTA, original33 migration on1.10.4, actual60s RTC or reboot recovery success. Resume exact V2.23 procedure once target responds. Full implementation contract now public on feature branch for portal owner; portal code unchanged here.

### V2.23 | 2026-10-15 | Codex

- Source1.10.4 final build2026-10-15T06:42:50Z /13:42:50 WIB PASS. App1513088 bytes, SHA2561f803f31e9656e85304e84b7797c518d1b8e53edca8867235a5156a6c1edf819; sketch1512941 bytes72%, staticRAM57336 bytes17%. Build-Firmware.ps1 metadata and legacy partition gates PASS. Candidates06:35:54Z/06:39:52Z failed compilation (String diagnostic argument), never uploaded. Final fixes diagnostic string lifetime/type; no firmware edits after final build start.
- Node web UI regressions, Python14 tests, portal packaging2 tests and diff check PASS. RTC pure calendar/tick/evidence assertions compiled. These are SOFTWARE results, not actual RTC acceptance. New filtered legacy import and full checkpoint-header/reboot path still require target runtime validation.
- Device last VERIFIED installed1.10.3/build06:28:27Z. Despite owner confirming power ON, direct HTTP/api/state twice timed out and ping2/2 failed from laptop192.168.1.122 around13:42..13:46 WIB. No1.10.4 upload, no actual RTC time write, no60s/24h/battery acceptance. Asked current device IP asynchronously; do not infer it or scan unrelated devices.
- Existing original33 PASS/sessionEB4CDCB2 remain independently archived in ignored build/rtc-legacy-original-v1_9_1.json/.csv (SHA99fe1e83761fa93e1fb157afc51bb68080e26e8479b8e23c7548b5d98768a8fb). Intermediate device checkpoint body retains old evidence but headers invalid; ordinary resume must reject. Do not erase/reconstruct headers. Narrow original legacy retry is allowed only with exact original evidence and no newer RTC progress. Original33 are owner historical results, not Codex new physical tests.
- Resume at sibling _codex_tmp/ota_rtc_patch4.py --upload only after fresh actual identity/calibration/profile/output checks and target IP confirmation. It gates exact final build/hash/classicESP32/same serial/no override/no erase. Then rtc_runtime_patch4.py: original CRC/calibration rejection cases, real migration33/details/CSV, actual explicit UTC+420 set/readback,60s tick, run locks/abort, checkpoint header/readback. Only after success perform identical-binary OTA reboot and rtc_runtime_patch4.py --after-reboot; native UI still pending. If any RTC revision advances, never retry legacy import/reset casually. No portal repository edits, main remains35733ba; feature-only publication, no merge.


### V2.22 | 2026-10-15 | Codex

- Final1.10.3/build06:28:27Z app1509280 SHA5b496b347c08fb01e6857ed13f6a6f15e9eeee5b4e9ae45d614bc88ce10c1102 OTA/runtime MD59bef4993ae6757ec88fade7ce266db9b verified. Header construction fixed and guarded; per-Modbus-tag historical merge preserves fresh data for other tags. Original legacy import then rejected Invalid legacy JSON before any RTC mutation: full JSON parse plus existing cached checkpoint exceeded available RAM.
- Source1.10.4 releases cold checkpoint cache before parsing, filters only required legacy fields, reloads cache for validation, and reports precise parse/NoMemory errors. Candidate06:35:54Z did not produce matching binary; gate rejected stale1.10.3 artifact (never uploaded). Fresh source-frozen build06:39:52Z running. Sibling _codex_tmp/ota_rtc_patch4.py and rtc_runtime_patch4.py target exact1.10.4 timestamp. Next gated OTA, original33 migration/readback, RTC set/actual60s, guarded run/checkpoint/CSV, identical-binary reboot/resume, native browser, docs/push. No physical accuracy/retention claim. Original archive unchanged; only draft contract7cf325d is pushed so far.


### V2.21 | 2026-10-15 | Codex

-1.10.1/build06:08:26Z app1505808 SHAf83adf47fae2ef825121a205526c7def93e7fcb9cdb4a82e169f72a48d9625b2 authorized OTA verified MD5bdceb4e0765389ad9b77bbde978f5868. Corrected decimal migration guard works. Initial original import wrote valid NVS but immediate large-request readback failed; same-binary reboot loaded it and explicit resume restored ALL33 PASS/details/sessionEB4CDCB2, outputsOFF. Small normal checkpoint save/readback then succeeded. No RTC hardware writes yet; clockRevision0.
-1.10.2/build06:15:29Z app1508176 SHAac165cfa1f37da835504d7999a099705a26a634493ea051539a7b2c5327fb18e OTA verified MD59d476c095ad8f1fb356b791adb18206d. Frees temporary MessagePack/input/validation trees before readback; preserves exact legacy retry only when all original nonRTC evidence/operator match and no RTC progress/revision/run. But pre-runtime header assertion exposed prior qcSaveProgress using next.to<JsonObject>() AFTER header initialization: this erased schema/build/profile/calibration metadata although results remained. Current1.10.2 unselected with available checkpoint/sessionEB4CDCB2, storedBuild/profile null; original immutable1.9.1 JSON+CSV still safe. DO NOT fabricate a missing header or ordinary resume.
- Source1.10.3 fixes to as<JsonObject>() and validates complete checkpoint identity BEFORE any write. Source-frozen Build-Firmware.ps106:22:11Z running, no later firmware edits. New helpers sibling_codex_tmp/ota_rtc_patch3.py (gated exact build, same serial/classic chip/size/hash/no private override; old intermediate upgrade requires RTCrevision0/no new tick), rtc_runtime_patch3.py (actual original import, negative CRC/calibration/ID, set/readback,60s tick, run guards, checkpoint HEADER+CSV checks) and --after-reboot. Next finish compile, authorized OTA, corrected original retry, actual RTC and same-binary restore proof, native browser and feature publication. No24h/physical battery PASS.
- User power-off caused old1.9.1 RAM loss but had been archived BEFORE interruption. Portal docs now exist (RTC_FIRMWARE_CONTRACT/CHECKPOINT), read-only mapping compatibility documented in local RTC_PORTAL_CONTRACT. Only draft contract7cf325d pushed; implementation/docs/tests local. Existing original archiveSHA99fe1e83761fa93e1fb157afc51bb68080e26e8479b8e23c7548b5d98768a8fb, originalQCcodeQC1-M1-TMC88F1559CA478-INCOMPLETE-EB4CDCB2-88DB3594. Preserve saved evidence and unrelated NVS.


### V2.20 | 2026-10-15 | Codex

- Final1.10.0 build06:02:33Z app1506432 SHA36bea5c9cdc885e0a4f4ae9187897db13f178e95637d878afc7b3307768b1fb7 gates PASS, authorized app-only OTA HTTP200 same serial/file-runtimeMD5f13399af53d7d25c7f9e5a839dd4f6f1; old profiles/calibration retained. Source had been frozen before build. Earlier03:01:50Z and05:58:05Z compile candidates passed but NEVER uploaded.
- User confirmed power back on. Old1.9.1 RAM session was lost during actual power-off; original33 PASS already safely archived JSON+CSV (seeV2.19). New1.10.0 legacy negative CRC rejection PASS, valid import rejected calibration comparison BEFORE any NVS checkpoint/select. Actual coefficient JSONs agree; strict internal double versus rounded serialized JSON was wrong. No forced import, no RTC hardware writes, no invented PASS, original archive untouched.
- Corrective source1.10.1 compares coefficients within JSON serialization precision (gain1e-9/offset1e-8) while exact captured calibration CRC IDs stay mandatory. Separate errors identify profile/preset versus coefficients. Build-Firmware.ps1 source-frozen06:08:26Z running. No later firmware edits. Next gated1.10.0->1.10.1 OTA, real original33 import/NVS readback, RTC set/tick, negative run guards and same-binary reboot/resume proof, browser/push. Helpers ignored sibling_codex_tmp/ota_rtc_patch.py and rtc_runtime_patch.py. Do not upload older artifacts.
- New progress blob uses internal MessagePack+deduplicated test text, CRC+atomic own NVS key/readback; logical16KB/packed9KB cap retains32KB partition. No erase. Added sample start/end/response monotonic microseconds and age bound; portal docs discovered and read; existing old GET/state adapter compatible, new routes require owner mapping. Draft contract committed7cf325d, latest additions local. Portal repository read-only, no changes there.


### V2.19 | 2026-10-15 | Codex

COMPACT-RTC-1 contract04_Dokumen/RTC_PORTAL_CONTRACT.md: explicit RTCset/readback/OSF handling+offset metadata/clockRevision, monotonic60s tick, persistent portal run lock, bounded immutable external accuracy+retention summaries and RTC-only retest. RTC no longer PASS from clean OSF alone. Progress own NVS CRC/readback, explicit resume (same build/profile/recipe/calibration), completed evidence retained/labeled; outputs OFF and partial jobs not resumed. Autosave opt-in after save/bind/report, max1/min; explicit save before power-off. No erase.

Live1.9.1 user sessionEB4CDCB2 has33 existing PASS/RTCUNVERIFIED, not Codex physical acceptance. Original sanitized state+CSV archived ignored build/rtc-legacy-original-v1_9_1.json/.csv; JSON17866 bytes SHA25699fe1e83761fa93e1fb157afc51bb68080e26e8479b8e23c7548b5d98768a8fb. Original operator empty, do not fabricate PIC. One-time explicit1.9.1 CRC-verified import bridge preserves these33 results after first upgrade, origin build/code traced; never force ordinary cross-build resume. Do not upload until original evidence safely saved and bridge tested.

Build03:01:50Z is a compile CANDIDATE ONLY: source changed afterward for safety/legacy migration fixes. Never upload it. Need fresh final source-frozen Build-Firmware.ps1, metadata/legacy gates, API/actual60s tick and same-binary reboot/resume proof. Node prior regressions pass; additional RTC cases underway. No portal repo edits; portal integration/24h/physical battery acceptance pending. Read rules/version/current/task/readme/validation/handoff; feature baselinea20e067 clean/fetched, existing WiFi/pins/polarity/partition/calibration kept.


### V2.18 | 2026-10-15 | Codex

- Firmware1.9.0 ->1.9.1. Final build UTC2026-10-15T00:33:08Z /07:33:08 WIB, app1426336 bytes SHA256f0045e69e579e3fd44dfa3845b91c2106edb38adddc5df8ad57c1413d334ed8e. Build-Firmware.ps1/metadata/legacy gates PASS. Sketch1426189 bytes68%, RAM56848 bytes17%. No firmware edits after final build start.
- Authorized app-only OTA192.168.1.31 HTTP200; same serialTMC88F1559CA478, new boot/version/build and runtime/fileMD578216abd7926c5556ee64df06701584b verified. Profiles/recipes/calibration match before OTA; no erase. FinalM1 PD OFF/outputsOFF/faultfalse. First immediate postboot AHT was not ready; subsequent native browser shows fresh valid32.8C/41.9%RH. Do not misread initialization as permanent failure.
- Native installed page reloaded: large32.81C/41.94%RH/3.67mA values visible with units, sample ages, numeric Y grid, elapsed seconds X, min/mean/max and proportional lines. Desktop1920 screenshot self-check confirms all three actual graphs; no mobile/printer/accuracy acceptance claim. Minimum spans1mA/2C/10%RH prevent exaggerated tiny changes; historical valid values stay separate from stale/offline current blank. Point titles provide exact value/time, native hover behavior not separately certified.
- Node readability/station/operator regressions PASS (axes/units/time/gaps/minimum span/stale/offline/empty/single/negative), Python14 and portal2 PASS, diff checkPASS. No API/pin/NVS/driver/WiFi/polarity/calibration policy change. Read AGENTS/CURRENT and full Versioning/ChangeNotifications issue1 at start; shared lab WiFi retained. Feature-only publication, no merge.
- Ignored evidence build/chart-pre-ota-v1_9_1.json,chart-post-ota-v1_9_1.json,chart-final-v1_9_1.json,firmware-manifest.json. Physical RTC OSF/battery/time, PD absence, EEPROMWrite/WC and load tests remain open, not changed by chart UI.


### V2.17 | 2026-10-15 | Codex

- User reports diagnosis graphs unreadable. Native existing1.9.0 page and source confirm no numeric/time axes/current reading and exaggerated tiny auto-range changes. Revision1.9.1 adds large fresh current readings+units, explicit stale/offline blank, numeric Y grid, elapsed-seconds X axis using real timestamps, min/mean/max, point titles with value/time, minimum spans1mA/2C/10%RH. Disconnect and >6s sample pauses split lines; history stays bounded120. Responsive cards put temperature/RH first. No sensor/driver/pin/NVS/API changes.
- Node regressions PASS incl chart axes/units/timestamps/gaps/minimum span/stale/offline/empty/single/negative data. Final Build-Firmware.ps1 started UTC2026-10-15T00:33:08Z; no firmware edits after start. Build/OTA/native revised UI acceptance pending. Baselinee5a451f clean/fetched before edit, main unchanged. Versioning/ChangeNotifications read fully from issue1; approved WiFi/legacy partitions/calibration preserved.
- Next: finish compile/gates, authorized app-only OTA exact manifest/hash/serial; compare old profiles/recipes/calibration, native reload+readability self-check, feature publication without merge. Do not promote physical acceptance.


### V2.16 | 2026-10-15 | Codex

- Source1.9.0 published55c0a578d9a094c7dd70963105b9eb54bea0730a on feature/qc-operator-ota; local/origin matched after push. This docs-only publication checkpoint follows; final remote parity checked after it. No firmware edits/rebuild/extra OTA since V2.15. No main/develop merge or other project changes.
- Native Chrome existing192.168.1.31 tab initially had old HTML with new runtime version. Visible Reload after activation loads new station navigation. Readiness, schematic map and diagnostic charts observed at desktop widths960/1920. Fresh AHT around32.7C/42.3%RH; readiness says accuracy unmeasured. Preset revision2/CRC6EAB66E6 loaded from NVS. RTC OSF remains PERIKSA/UNVERIFIED. No output ON or fixture acknowledgement sent.
- Native navigation can retain prior scroll position and immediate accessibility snapshots can lag the click; re-observation confirmed readiness/map/charts. History/print/preset visual acceptance not claimed in this pass; software/API/independent QR checks remain PASS. A later history click landed summary during changed browser geometry, so no assertion that history was visually verified. No mobile/printer/physical-load proof.
- Final continuation: firmware1.9.0 installed and app-only OTA/reboot persistence proven; loopback archive127.0.0.1:8770/history available while server runs. Physical PD0x28, EEPROM WC/read-write, RTC battery/time and output fixture acceptance remain open. Preserve existing profiles/calibration and explicit physical versus software evidence.

### V2.15 | 2026-10-15 | Codex

- Final1.9.0 Build-Firmware.ps1 PASS UTC2026-10-15T00:17:08Z, app1421824 SHA7448e56df50c5a5f8850583dd7def877717d2211397aff2cb481d79af7822038, metadata/legacy gates PASS; no later firmware edits. Candidate failed ArduinoJson/detail collision superseded by recipeDetail fix. Node/Python14/portal2 and independent QRdecode PASS.
- First app-only OTA1.8->1.9 HTTP200, exact serial/build/file-runtimeMD50adb38e994b4ad773d456918a9ea8f4d. Real recipe confirm/bounds/stale/readback/active-inactive session/export guards PASS. Same exact binary uploaded a second time to prove fresh boot token and recipe persistence; recipes+old profile masks/calibration exactly match before reboot. New ownqc-recipes namespace, no erase. M1/M2 presets default settings revision2 after tests; old M2 profile remainsrev6/DO testsOFF.
- Real local laptop archive API Host/token/import/dedup/list/JSON/CSV/history PASS, server127.0.0.1:8770 session98948 running, serial idle. Browser station UI pending/native policy only; no alternate control bypass. First QRasset verification failed verifier default encoding; UTF8 comparison nowPASS, no firmware change.
- FinalM1/PD OFF/outputOFF/faultfalse. No actual outputON, EEPROMWrite, PDrequest/radioApply or physicalPASS. Evidence ignored build/first-v1_9_0-pre/post-ota-state.json,pre/post-ota-state.json,station-report-runtime-v1_9_0.json,recipes-before-reboot-v1_9_0.json,runtime-final-v1_9_0.json,laptop-archive-runtime-v1_9_0.json,qr-reference-test.json/png.
- Source/docs/test ready feature commit/push, origin2b5af1a. Next publish/parity then native browser check self-tested new UI; no merge/general serial-web changes.


### V2.14 | 2026-10-15 | Codex

- Added unselected-boot editor/default selection, guide phase ordering and bounded offline chart-gap regressions; Node all PASS. Python14/portal2 PASS, OpenCV independently decodes exact offline QR. Pin map is explicitly schematic, not PCB component placement. No actual outputON, EEPROMWrite, PD request or radioApply.
- First00:09:54Z compile candidate failed qc_recipes global detail versus ArduinoJson namespace collision. Renamed recipeDetail. Final Build-Firmware.ps1 starts2026-10-15T00:17:08Z; no later firmware edits. Never upload the failed/candidate artifact or relabel stale binary.
- Actual loopback laptop service started on free127.0.0.1:8770 (exec session98948), no COM opened. HTTP archive Host/token403 guards, import/dedup/list/JSON/CSV/history PASS with historical1.8 CODEX SOFTWARE CHECK INCOMPLETE report, ignored under tools/laptop-qc/reports/archive. Worker remains idle. Service must be restarted from updated source on another laptop; no Windows startup automation added.
- Read-only device readiness1.8.0/build2026-10-14T14:54:18Z, classic ESP32, outputOFF/faultfalse/TPLready, OTA Digest200. Helper gated to1.9.0 artifact+new boot token. Next finish compile/gates, authorized OTA, actual recipe bounds/readback/stale/session/export/unchanged old profiles+calibration, then second identical gated OTA to prove new recipe persistence. Preserve first-upload evidence; no erase. Browser self-check only through allowed native skill, no URL-policy bypass.

### V2.13 | 2026-10-15 | Codex

- User authorizes all seven follow-up ideas, self-check and app-only OTA. Baseline2b5af1a clean/fetched/push up-to-date first; read AGENTS/CURRENT/latest2/backlog/README/VALIDATION/handoff and full Versioning/ChangeNotifications via issue1. Scope04_Dokumen/QC_STATION_REVISION.md ST01..07; no other repository/merge.
- Local1.9.0 qc_recipes own qc-recipes/recipes-v1+crc-v1, schema/revision/normalized limits/readback; perM two Modbus tags/analog refs+tolerance/serial fixture settings/1..10sec output limit. No migration of existing profile/calibration NVS. Recipe rev/CRC enters QC code/JSON/CSV; active save resets session, load only fills forms. Invalid saved recipe blocks selection rather than silently fallback.
- UI readiness, schematic verified-pin map(not physical PCB placement), bounded/stale-gapped charts, staged guide with explicit ON/OFF and two physical observations; firmware TTL independent of browser. Stop/session/disconnect cancels. Mock discovered stale pre-command snapshot falsely cancelling guide; fixed fresh poll before observe phase. No actual ON command sent.
- Immutable browser unit history/comparison and loopback laptop archive import/serial-search/download; token/Host gates retained, no COM/device contact on import. Offline pinned QR qrcode-generator2.0.4 MIT npm SHA1 verified, license/provenance; print original snapshot/status, no CDN/signature claim.
- Additional boot-state test found empty preset editor when device UNSELECTED; fixed to first actual saved profile. Build started00:09:54Z is therefore a compile candidate only, NOT for OTA. A fresh final build after source freeze is required.
- Node expanded station/operator/archive/PD/Stop PASS; Python14 PASS incl append/dedup/no overwrite/corrupt import/CSV formula/multi-device; portal2 PASS. Independent OpenCV decodes exact generated QR reference. Firmware compile/runtime recipe persistence/browser/OTA still pending. All source/tests/docs local, origin2b5af1a. Next frozen final build, verify live1.8 baseline and artifacts then authorized OTA, runtime/archive/browser tests and feature publication. Physical PD/WC/accuracy/load unknown.


### V2.12 | 2026-10-14 | Codex

- Source/docs/tests1.8.0 pushed e4515d46979a5bb1ada2311c2da9fe54e3b94d0c; local/origin/ls-remote matched, main unchanged35733ba. Authorized OTA/build/runtime evidence V2.11; no extra firmware edit/build/upload. This final publication/browser-only checkpoint is committed/pushed next; exact SHA is branch HEAD. No merge/general serial-web changes.
- Native Chrome recovery succeeded for reload using visible Reload after activation/fresh screenshot. Actual installed Operator UI now visible: active M1/revision/session banner, next-step/filter controls, Technician-only navigation hidden and RTC localized unverified. Report page shows simplified form and archive.
- Actual browser PIC CODEX UI CHECK, date2026-10-14: Tinjau shows exact INCOMPLETE/33 pending/session/profile; explicit Confirm returns TERSIMPAN and archive1/50 with serial/M1rev1/operator/date/QCcode and JSON/CSV buttons. No physical PASS invented; no ON/EEPROMWrite/PDrequest/radioApply. Backend snapshot/CSV consistency and adverse storage cases separately PASS in V2.11 tests.
- Browser Control+R attempted only to check archive persistence across reload; Computer Use then stopped because it could not determine current URL with confidence for policy. No further UI input or alternate UI workaround. Archive-save and desktop layout are observed PASS; browser reload persistence/mobile widths not physically verified. This limitation does not undo completed exact-gated OTA or published source. Final M1/outputOFF before browser stop; calibration/profiles retained.

### V2.11 | 2026-10-14 | Codex

- Final1.8.0 Build-Firmware.ps1 PASS2026-10-14T14:54:18Z, app1315136 SHAf1b81864417bcbbb78854bca766bbd25f337b7f7ae1a6bb5d34095b387819125; identity/legacy gates PASS. No firmware edits after build start. Node expanded operator/archive/Stop/PD PASS, Python10/portal2 PASS.
- Authorized app-only OTA192.168.1.31HTTP200; same serial/version/build and MD53e7505c7ee48874729c5e77d408219c9 match. Profiles M0/M1/M2rev6 and calibration retained; no erase. LIVE RTCOSF becomesUNVERIFIED/timeValidfalse; report400PIC/date,409session/revision/QCcode guards PASS; snapshotJSON/CSV fields/code/everytest/first10 columns exact. Saved ignored snapshot retained after new session.
- FinalM1 PD/EEPROM OFF/outputsOFF/faultfalse; AHT fresh-valid. InstalledHTTPHTML has new controls/script. Prior native browser URL-policy stop means visual/layout/browser-storage acceptance not yet claimed; Node tests cover archive semantics, software-only boundary explicit. No physical ON/Write/PD/radioApply.
- Local source/docs/tests ready feature-only commit/push; origin stille320474. Evidence ignored build/runtime-final-v1_8_0.json and operator-report-runtime-v1_8_0.json/.csv. Next publish/verify parity; optionally native browser read/reload if policy allows. No automatic merge or general serial-web work.


### V2.10 | 2026-10-14 | Codex

- User explicitly resumes all work through OTA. Local changes V2.8/V2.9 preserved; baseline origin remains e320474, fetch done. Re-ran Node operator/archive/Stop/PD regressions PASS, Python10/portal2 PASS. Build-Firmware.ps1 starts2026-10-14T14:54:18Z; no firmware edits after start.
- Readiness probe192.168.1.31 shows installed1.7.0/build13:42:34Z classic ESP32-D0WD-V3, TPLready/OTA supported, outputsOFF/faultfalse, Digest page200. Probe only, no upload yet. OTA helper gated to manifest1.8.0 and exact SHA/identity/classic chip/size/no private override; will verify same serial/build/MD5/profile/calibration.
- Need actual final compile/gates and authorized app-only OTA, runtime reportAPI/RTC/profile checks and feature publication. Prior browser URL-policy stop is a visual-validation limitation; do not bypass it. No hardwareON/EEPROMWrite/PDrequest/radioApply; general serial-web untouched.


### V2.9 | 2026-10-14 | Codex

- Computer-use skill/runtime changed from earlier browser API to node_repl sky. Read current SKILL/guidance/confirmations/API; selected exact Compact QC Chrome window1379244 from returned inventory. Read-only state confirms device1.7.0 on192.168.1.31/#i2c, current M0 sessionE2F217F3. No physical controls changed.
- First attempted Ctrl+T for local preview was refused: Computer Use could not determine current browser URL with confidence to enforce policy and stopped this turn. No further UI input or alternative UI workaround. Browser preview/visual/archive validation remains undone. A preview server was started on loopback8769; stop/reuse it on resume, do not spawn duplicates.
- Local1.8.0 implementation and Node/Python10/portal2 tests PASS (V2.8); firmware not compiled, not OTA-uploaded and new source not committed/pushed. Remote remains baselinee320474; do not claim1.8.0 installed. No main/develop merge. Preserve all local modifications and current app/device evidence. Next complete preview/layout, C++ build and runtime API validation, then exact-gated app-only OTA and feature publication. This is a partial checkpoint, not task completion.

### V2.8 | 2026-10-14 | Codex

- User authorizes all six web recommendations after pushing baseline; git push up-to-date e320474, no merge. Read AGENTS/CURRENT/latest2/backlog/README/VALIDATION/handoff and issue1 Versioning/ChangeNotifications fully before changes. Scope04_Dokumen/OPERATOR_WEB_REVISION.md WEB01..07.
- Local1.8.0 operator/technician view, explicit active-vs-draft profile and staged session/profile/report confirmations; guided pending/fail/all summary, grouped profiles/clone/diff, retained inputs/results and optional raw diagnostics.44px controls/stickyStop. Existing PDconfirm/Stop/work-owner guards retained.
- RTC OSF nowUNVERIFIED, communication/timeValid separate; no RTC writes. New/api/qc/report server captures hardware/QC afterOFF, requiresPIC/date/session/revision/QCcode, rejects activeworkers/stale evidence; shared CSV formatter uses same snapshot. Laptop browser archive(max50/no eviction), exact JSON/CSV/bundle downloads, quota/corruption errors, dedup and no tokens. DeviceNVS/EEPROM/pins/WiFi/partition unchanged.
- Node expanded regressions PASS including archiveimmutability/CSV/context/Stop/quota/corrupt/full; Python10/portal2 PASS. Build/upload stillpending, actualdevice1.7.0. Native browser control plugin changed in this session; next initialize available skill/runtime, preview and verify then frozen final build. No ON/EEPROMWrite/PDnegotiation/radioApply.


### V2.7 | 2026-10-14 | Codex

- Source/docs/tests1.7.0 published e10f329621e74cdcb2e13f34e7d446c057146e8c; local/origin/ls-remote matched. main unchanged35733ba; no develop branch/automatic merge. This publication-only checkpoint is committed/pushed next; its exact SHA is branch HEAD, with no firmware edit or additional build/OTA.
- Installed1.7.0/build2026-10-14T13:42:34Z exact runtime/fileMD5 confirmed. See V2.6/VALIDATION for build hash and actual browser/API/OTA evidence. Presets/slider/review/cancel implemented; no accepted voltage command. Final M1 PD OFF, outputs OFF; saved profiles/calibration and AHT data retained.
- Physical followup: read-only M0 no I2C response0x28 before/after OTA, cause unknown. Fixed-only/no PPS is ST documentation fact; actual adapter capability/voltage is unverified. Do not treat slider draft or protocol contract as measured VBUS. User-owned tab left on installed PD page; unrelated serial-web untouched. No source/upload work pending.

### V2.6 | 2026-10-14 | Codex

- Final1.7.0 build2026-10-14T13:42:34Z PASS, app1287984 SHA205b4a636a1db5b5b10f03dfdc5969c458b0b2380a0ddfe9c0d708bb06f6fe44; binary/legacy gates PASS. First13:39:48Z compile failed uncaptured server; corrected toweb pointer. No firmware edit after final build start. Node/Python10/portal2 and preview PASS.
- Authorized app-only OTA192.168.1.31 HTTP200; same serial/version/build and MD5557d9c223ada1b62577b00c7c94a424e match. Profiles/calibration retained exactly. Live browser presets20V review, Cancel and slider19.95V draft PASS; missing confirm API400 before PD transaction. No accepted PD voltage negotiation. M1 restored, PD/EEPROM excluded/outputs OFF; AHT fresh valid. No NVS/storage/partition erase.
- Live read-only M0 PD before/after NOT DETECTED/no0x28 response; cause unknown, source caps/VBUS unverified. Separate from STUSB4500 fixed-only/no PPS evidence.16/17/18 require exact fixed source PDO. Primary research/procedure in04_Dokumen/PD_CONTROLS.md.
- Source/docs/test local ready to commit/push on feature only; remote baseline8593a7c, no merge. Evidence ignored build/runtime-final-v1_7_0.json,pd-review-live-v1_7_0.jpg. Next publish/check parity then physical followup only with new hardware evidence.


### V2.5 | 2026-10-14 | Codex

- Source1.7.0 local: fixed-PDO5/9/12/15/20 presets,50mV draft slider, source snapshot warnings, explicit review/confirm. /api/pd requires confirm=1; state/context/draft/Stop guard prevents stale confirmed send. ST primary sources confirm no PPS;16/17/18 require exact fixed source PDO. No alternate protocol invented.
- Node expanded PD/Stop/confirmation tests PASS; Python10 and portal2 PASS. M1 still PD OFF, no actual voltage command. Pin/NVS/calibration/WiFi/legacy partition retained. Device prior1.6.1; no new build/upload yet.
- Rules_Versioning/Change_Notifications reread via issue1; repo AGENTS/CURRENT latest2/backlog/README/VALIDATION/handoff read. Remote8593a7c clean baseline preserved. Next preview then frozen final build, gates and authorized app-only OTA; physical PD remains unknown. No merge.


### V2.4 | 2026-10-14 | Codex

- Source/docs/tests1.6.1 published as b39d4471bdd25fd4141dae0540d8a5ac33067223 on feature/qc-operator-ota. Local HEAD/origin/ls-remote matched; working tree clean before this publication-only checkpoint. No main/develop merge.
- Installed1.6.1/build2026-10-14T11:45:51Z and MD5ba772733fa865400ed57a91eff4390a5 verified; final gates and runtime/browser evidence remain V2.3/VALIDATION. No firmware changes or additional upload for this docs checkpoint.
- Final active deviceM1, outputActivefalse/faultfalse; M2 revision6 saved with both DO tests OFF. AHT live data restored; EEPROM transport Read succeeds but Write failed Wire4, so M1 exclusion applies. Physical WC question remains issue1/comment6167590821; EEPROM may be enabled after successful Write/Read. Other physical load/accuracy/watchdog acceptance is not claimed.
- This docs-only checkpoint is committed/pushed next on the same feature branch; its exact SHA is Git branch HEAD. No local source/build work remains. General serial-web project untouched; user-owned device tab left on live AHT.


### V2.3 | 2026-10-14 | Codex

- Final1.6.1 Build-Firmware.ps1 PASS11:45:51Z,1282720 bytes SHAdc4f8de4d78331931b85c840038a95089559c0a38542262f5eb74666c12c7958; metadata/legacy gates and Node/Python10/portal2 PASS. No firmware edits after final build start. Source1.5.1->1.6.0->1.6.1.
- Authorized app-only OTA192.168.1.31 returned200; same serialTMC88F1559CA478, version/build and runtime/fileMD5ba772733fa865400ed57a91eff4390a5 verified. Device baseline1.5.0->1.6.0->1.6.1. Calibration and saved profiles exactly survived reboot; no NVS/storage/partition erase.
- Actual browser verifies M1->M0 EEPROM buttons re-enable without reload and Read succeeds; M2 DO1 on-test/rev5 then off-test/rev6 independently of DO2; current editor saves/readback/session reset. Two OFF clicks and priority Stop accepted; no actual output ON sent. CSV M2/rev6/ignored DO + first10 Longhi columns PASS. Final M1 active, outputs OFF.
- AHT remains fresh-valid six-byte/Wire0; screenshot ignored build/runtime-aht-card-v1_6_1.jpg. EEPROM Read200 blank FF; Write rejected Wire4 at0x3F80 on API/browser, no false PASS. M1 EEPROM remains OFF, WC question issue1/comment6167590821 unanswered at latest check; physical WC/reference accuracy/load acceptance unknown.
- Source/docs/test changes ready for feature-only commit/push; no merge. All old local docs preserved. Publication SHA will be recorded after push verification. Next only hardware followup for WC/EEPROM, then explicitly enable EEPROM if Write/Read succeeds; custom profile and AHT requests completed within documented evidence boundaries.


### V2.2 | 2026-10-14 | Codex

- Final1.6.0 build11:32:27Z PASS,1282320 bytes SHA474dfee45fc28123ba50eb7d3c2c730446fcc1a131a927bdbc96cea5f057687a; metadata/legacy gates PASS. Authorized OTA192.168.1.31 returned200, same serial/version/build and MD5 matched; calibration retained. User browser reloaded and showed1.6.0/AHT data.
- AHT now valid six-byte samples, status0x18/Wire0, about36.1C/29.8%RH. M1 exclusions/API reject and M2 persisted configuration/DO-off/re-enable/all-off-INCOMPLETE/stale edits passed on device. M2 left both DO OFF. No physical output/accuracy certification.
- EEPROM Read works, blank128 FF. Write first16-byte chunk at0x3F80 fails Wire4; repeated browser single click also fails, no fake PASS. WC/model/power unresolved; requested primary AI schematic/WC evidence via issuecomment6167590821. M1 EEPROM remains OFF per user condition.
- Actual browser found sticky-disabled controls after M1->M0/unselected->selected. Source1.6.1 fixes per-render direct gates, tests transitions; clarifies WC diagnostic. Node/Python10/portal2 PASS. Build start11:45:51Z; no later firmware edits.1.6.1 final compile/OTA pending.
- Git: all docs/source/tests local/uncommitted, remote still add83d9; no merge. Next finish1.6.1 gate, authorized OTA, verify profiles/calibration across reboot and actual browser controls, then document exact final runtime/build and publish feature only. Keep M1 EEPROM excluded unless physical Write succeeds.


### V2.1 | 2026-10-14 18:30 UTC+7 | Codex

- Issue1 answered: rules obtained/read; read private-vault related rules from aa0bb9e. Live192.168.1.31 classic ESP32/1.5.0/build05:16:49Z, AHT E1 NACK, EEPROM readable EMPTY FF, calibration active, TPL ready. User authorizes direct browser test and OTA. WiFi/credentials/pins/partition retained.
- Local1.6.0: qc_profiles/math/tests,36 switches/16 saved M profiles, M1 Radio config/RS232/PD/EEPROM exclusions, independent DO switches; NVS qc-profiles/profiles-v1 only. Mask/revision exported/hashed; API/result/input/background enforcement. Active edits reset session, preserve calibration/EEPROM.
- AHT diagnostics field corrected; calibrated probe/no unconditional reset,350ms recovery settling and one6-byte frame/bounded timeouts. EEPROM empty Read succeeds with identityValid=false; exact transfer/readback diagnostics, foreign protection unchanged.
- User adds heavy/multiple-click report: live single clicks accepted; GET latency5 samples median63.9ms. UI replaced stable output controls every poll and discarded busy clicks. Retained DOM, immediate progress, bounded sequential queue and priority Stop/cancellation implemented; no claim of general electrical acceptance.
- Node UI expanded regressions PASS; Python10 PASS; portal2 PASS; constexpr profile/AHT assertions require final compile. Candidate11:21:29Z is still running while later source edits occurred: not for OTA. Next fresh Build-Firmware.ps1 after edits freeze, then metadata/partition gates and authorized app-only OTA. No upload/push/merge yet; all changes local.


### V2.0 | 2026-10-14 | Codex

- User requested AHT/EEPROM repair, M1 exclusion of Radio configuration/RS232/PD/EEPROM if unresolved, and editable M0/M1/M2/M3 test profiles. Resume pause revoked for this scoped revision.
- Source diagnosis: aht10State writes hardware.detail while dashboard reads hardware.ahtDetail; missing error display is proven, sensor failure cause unknown. EEPROM failure requires fresh transaction/slot evidence.
- Wrote04_Dokumen/QC_PROFILE_REVISION.md with REV-01..04, persistence/enforcement/session-reset/export criteria and explicit ignored-vs-PASS boundary. Firmware unchanged1.5.1; no new test/build/upload. Existing six local doc changes retained.
- Created authorized GitHub assistance issue https://github.com/TaufikAS0/compact-qc-tester-v2-r1/issues/1 for primary-laptop AI: mandatory vault rules plus sanitized read-only AHT/EEPROM evidence. No comments at last check. Firmware/build/push requires those rules per AGENTS.
- Git remote unchanged add83d9; docs local/uncommitted, no push/merge. Next read issue response then implement REV-01..04, preserving DO HIGH/LED LOW/NVS/foreign EEPROM.


### V1.9 | 2026-10-14 17:49 UTC+7 | Codex

- Audited clean local feature branch e7c72e5, fetched remote and fast-forwarded12 commits to exact checkpoint add83d971df4e2de5eb994beeae4a735058e1145. Remote feature matches; main35733ba, no develop. General serial-web project untouched.
- Read AGENTS/README/VALIDATION/TEAM_HANDOFF, CURRENT + V1.8/V1.7 and backlog. Source already1.5.1 with both DO active HIGH/LED MCP LOW. Corrected stale current operator/install docs that still said DO LOW, old boot version and serial deferred; preserved historical evidence. No firmware edits/version bump.
- Regressions: Node UI PASS, Python10 PASS, portal2 PASS. No fresh compile/device contact/upload. Old local app1120144 bytes SHA256231291610a98957b526c86115ccc9534da0ec1e35febba30538ba2e87ddb55c5 has no manifest/metadata; not the validated1.5.1 binary.
- Vault entry/versioning/change-notification paths missing; targeted search found no required rule files. Resolve/read before fresh build. Existing build1.5.1 at10:35:15Z is GitHub checkpoint evidence only.
- Publication: stopped on explicit user request before commit/push. HEAD and remote remain add83d971df4e2de5eb994beeae4a735058e1145; six documentation files are local/uncommitted. No new build/upload/merge. Resume together with upcoming revisions; if assistance is needed, user authorizes asking through GitHub for the AI on the primary laptop to answer.
- Next: resolve vault and obtain freshly gated1.5.1 app if needed; team install/verify version/buildUTC, repaired relay, both DO GPIO and physical ON/OFF, Stop/heartbeat/timeout. QC16 EEPROM/PD needs fresh bench observations; no automatic merge/upload.


### V1.8 | 2026-10-14 | Codex

- Live GET192.168.1.55: installed1.5.0 build2026-10-14T05:16:49Z, relayEnabledtrue, faultfalse; localhost8769 unavailable. No GPIO commands or flash issued.
- Confirmed core3.3.10 digitalWrite-before-pinMode ignored; fixed preload/reassert sequence. Both DO polarities changed toHIGH per latest user confirmation; relay failure is hardware per user. Added per-channel GPIO state/mismatch diagnostics.
- User follow-up confirms DO1 always physically OFF; DO2 OFF command physically ON and ON command OFF. Superseded by subsequent user correction: failed relay is hardware and BOTH DO1/DO2 invert ON/OFF.
- NodeUI PASS; Final compile via Build-Firmware.ps1 PASS10:35:15Z /17:35:15 UTC+7; app1261792 bytes SHA25623dfaaed92410e2404dc9cb61bd43ec6458376f02ae267bc7ac89331358a325a, metadata/legacy partitions PASS; logs build/build-v1_5_1-final.log. Python10 PASS, production init/mapping asserts compiled. Previous candidate09:07:34Z superseded. Source ready to push. User identified failed relay as hardware; bench compare GPIO and physical output after team install.
- Read eight mandatory workspace rules and versioning/architecture skills. Pinmap32/33/NVS retained; labSTA HuaweiJIN/AP12345678 and existing selective WiFi migration unchanged. No device upload.


### V1.7 | 2026-10-14 | Codex

- Published: source1.5.0 commit fa0624ea3d979f2a87ff3db118a46c4ad6ca0387 on feature/qc-operator-ota. HEAD and remote branch matched; working tree clean at verification.
- Build/tests: final1.5.0 gates from V1.6 remain valid; no further firmware edits or device upload. This checkpoint only updates publication tracking.
- Next: team upload and physical QC using 04_Dokumen/QC_OPERATOR_GUIDE.md. QC16 still requires bench followup; previous PD0x28 absence and missing EEPROM identity are historical device evidence, not a fresh device reading today.


### V1.6 | 2026-10-13 | Codex

- Did: completed1.5.0 integration, strict real-template DOM binding regression, same-SSID credential reapply, MQTT auth guard and bounded AHT failure count; detailed CSV point/tag evidence and operator guide.14 software IDs DONE;QC16 diagnosed/bench followup;ADSclosed.
- Final build: 2026-10-13T09:34:41Z /2026-10-13 16:34:41 UTC+7, 1260416 bytes SHA25644eb6bb7071b960c75fb3fd8fbc6a9be6dabcd67f82b31bcb4aac4c27469a29c; metadata/legacy partitions PASS. Latest logs build/build-v1_5-complete.log/.err. Node/Python10/portal2 tests PASS; C++ protocol/mask/5-point assertions compiled. No source edits after final build began.
- Browser: actual preview1.5.0,2 tag forms and RS232 mode scenario, separate menus; missing Apply element fixed and strict lookup catches this class of error. Screenshot ignored build/dashboard-serial-v1_5.png. No physical PASS fabricated.
- Git: feature branch only; source/doc checkpoint will be pushed after final diff/status verification. No upload/merge. Installed actual board remains1.2.0 at192.168.1.55;PD28 unavailable,EEPROM identity missing.
- Next: team install and operator guide physical tests; investigate QC16 raw EEPROM slots/PD population using real evidence. Software source implementation complete, not hardware certification.


### V1.5 | 2026-10-13 | Codex

- Did: two-tag Modbus master, serial/SPI/network menus, RS232 scenario, worker WiFi/ETH/MQTT, explicit E70 Apply config/readback, EEPROM raw-slot diagnostics, PD missing-IC diagnostics/Get_Source_Cap retries, larger badges and detailed CSV evidence. Atomic MCP LED/radio RMW avoids interleaved TPL service/clobber.
- Source: local1.5.0; published baseline1.4.0 b36f3d6. No upload/merge. New PubSubClient2.8 installed via Arduino library registry. Operator guide04_Dokumen/QC_OPERATOR_GUIDE.md.
- Tests: Node extended UI PASS (two tags, route visibility, network ownership, explicit apply and private-field exclusion); Python10 PASS. Production Modbus CRC/request-match/length corruption and MCP mask assertions added. First1.5 compile caught radio write stage misplaced in line parser; moved it to update state machine.
- Build: latest PID38844/helper2928; build/build-v1_5-verified.log/.err, fresh generated identity. Do not claim binary acceptance until Build passed/manifest metadata+partitions.
- Evidence: device192.168.1.55 still1.2.0; EEPROM readable but no valid identity; I2C no28. New code distinguishes absence/attachment/protocol errors without writes to physical device. QC16 remains physical/bench followup if PD absent.
- Next: build gate, preview new menus/fields and screenshot, update checklist with exact final evidence, commit/pushfeature. If interrupted, preserve source and report true state.


### V1.4 | 2026-10-13 | Codex

- Did: AHT10 async busy/status recovery;5-point analog capture/tolerance/user QC, calibration fingerprint invalidation.1.3.0 ->1.4.0.
- Tests: Node UI PASS; production aggregation assertions and final firmware/metadata/partition gates PASS08:42:43Z. Task-owned compiler suspension recovered. No upload.
- Evidence: actual192.168.1.55 remains1.2.0; EEPROM read lacks valid identity; PD28 absent from I2C ACK scan. No writes/voltage request.
- Next: apply Modbus drafts currently ignored in build/modbus_* and network draft build/network_qc.*; these are not part of published firmware yet. Finish remaining checklist.


### V1.3 | 2026-10-13 resumed | Codex

- Did: new explicit user continuation revokes previous deadline pause; no new deadline. Read8 mandatory vault rules and audited dirty feature branch. Build PID12988 fresh metadata; no source edits while this output baseline compiles.
- Next: complete1.3.0 compile/review/push, then AHT10 busy/status state machine, five-point analog QC and Modbus tags. No upload/merge.


### V1.2 | 2026-10-13 13:49 UTC+7 | Codex

- Did: authorized time-bounded implementation until13:52; source1.2.0 ->1.3.0. Latest user confirms LED MCP active LOW, DO remains LOW. Independent13 output states/timers, defaultOFF, accepted-state highlight, shared polling, single visual confirmation/latch, I2C user confirmation and module badges.
- Tasks: QC-02/03/04/06/08 source implemented, compile pending; QC-01/05 PARTIAL; QC-07/10/11/12/13/14/15/16 TODO; ADS CLOSED. No physical acceptance/upload.
- Tests: Node UI PASS; Python10 PASS; portal2 PASS; browser preview current1.3.0/defaultOFF/local badges; source constexpr assertions pending compile. Build PID32532 began06:44:01Z; logs build/build-v1_3-final.log/.err. Do not treat an unfinished build as binary acceptance.
- Files: app/config/version/hardware/session/UI/math/output_state.h; tests/web-ui.test.cjs; backlog/README/AGENTS/handoff/changelog/validation.
- Git: feature/qc-operator-ota; last pushed docs04d9725/source218f208. Current firmware edits local/uncommitted until gate completes. Keep explicit pending checkpoint if deadline interrupts compile.
- Next: preserve exact status at13:52 and stop. Next authorized session finish compile/review, then diagnose AHT10, analog5 points and Modbus2 tags. No main/develop merge.


### V1.1 | 2026-10-13 13:31 UTC+7 | Codex

- Did: merekam16 ID permintaan;15 TODO dan ADS1115 CLOSED karena user menyelesaikan hardware. Memisahkan fakta, laporan, unknowns, kriteria selesai dan aturan checkpoint.
- Files: `ai/tasks/2026-10-13-qc-revision-backlog.md`, file worklog ini, link masuk README/AGENTS/TEAM_HANDOFF.
- Baseline: lokal/remote `218f2081f0e11f7753586a5c6f6c46b46e157855`, firmware1.2.0; status awal Git clean. Publikasi dokumen mengikuti feature branch saja.
- Verification: perubahan dokumentasi saja; cek diff/tautan/checklist dan status remote setelah push. Tidak ada build atau physical QC baru.
- Next: **tunggu `kerjakan`**; implementasi belum dimulai. Setelah diizinkan, audit preview vs live device, lalu mulai ID output/status sambil memperbarui checklist/CURRENT.

Final V1.2 checkpoint: task-owned build parent32532 and descendants stopped before13:52 while compiling libraries. No new gated binary. Documentation-only checkpoint pushed on feature branch; local firmware/tests remain uncommitted. Remote source is still1.2.0/218f208. Working tree expected9 modified firmware/test files + untracked output_state.h; docs committed. Next exact action: Build-Firmware.ps1 fresh metadata, inspect source/review, publish only after gate passes, then QC-07.

V1.3 validation: generated1.3.0|2026-10-13T08:16:19Z,1193632-byte app SHA25658af51e3a11de0885e45433cdcad613263a15738491c46937e0fc2fa1631663e. Metadata/legacy partition gates PASS. Source publication includes output_state.h. Physical tests remain pending; no upload. Next implement AHT10 and analog5-point QC.
