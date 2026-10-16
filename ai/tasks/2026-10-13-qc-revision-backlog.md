### V2.57 | 2026-10-16 | Codex - popup OTA complete

Installed1.16.0/build2026-10-16T06:31:10Z (13:31:10 WIB) app-only OTA144/C28C HTTP200; exact runtime build/MD5/unit/profiles/calibration verified. Source8b91818 feature pushed BEFORE OTA. Compile/metadata/legacy gates PASS; app1559536 SHA256b13a33eaada3a52e28a7eca0da0c5dbc578f92a9cee6ec156066745bc030e75c MD576067848314b534a5e48afc2ac56d231, sketch1559385 bytes74%, staticRAM57672 bytes17%. No firmware source edits after final build start; cancelled06:28 candidate NEVER uploaded. No main merge/erase/profile or hardware mapping changes.

Explicit exact1.15.1->1.16.0 checkpoint resume; preserved own6F614ECF M1rev3 ALL33PASS1FAIL0pending, including actual fullcycle radioPASS with stored CNCFG1 (never restore olderchannel0/waiting evidence). Compared EVERY prior PASS/FAIL/detail, all analog/Modbus/network/serial/radio raw evidence, RTC quick/revision, session/operator/PCB/lastReset/profiles/calibration; checkpoint clean before and AFTER modal tests. No fresh radio GET/SEND/reset, no RTC/PD/EEPROM/outputON commands for UI testing.

ACTUAL browser on installed1.16.0: radio module RESET REVIEW opened CENTERED native popup, background dim/blur and inert, Batal initial focus. Clicked Batal; radioPASS retained. Reopened from same position: window.scrollY2101 BEFORE/OPEN/AFTER all equal, native:modal true, body scroll locked only while open. Tab from confirm wraps to modalStop, Escape closes, focus returns to original Reset QC Radio button, scroll unlocks. Checkpoint save review also native popup; Batal closes/focus returns saveProgress, no save/reset mutation performed. Actual screenshot ignored build/modal-live-v1_16_0.jpg. General+PD popup and PD Tab/Escape/Batal visually checked separately on LOCAL PREVIEW ONLY, no physical PD negotiation; M1 PD staysOFF. Preview is not hardware proof.

Node UI PASS including modal open/Escape/Batal/focus/Tab/ShiftTab/Stop/navigation plus previous PD/radio/RTC/session/draft/epoch guards; Python14 PASS; portal packaging2 PASS; all firmware assertions compile PASS. All existing reviewed actions share popup; no added confirmation stages for direct observations. User should Ctrl+F5 to replace old inline-panel HTML after OTA. Progress and modal contract in CONFIRMATION_MODALS.md. Agent preview/tab cleanup and final docs push/parity only remain.

### V2.56 | 2026-10-16 | Codex

Owner requests ALL existing review/confirmation panels become centered popups, plus GitHub push and OTA. Source1.16.0 converts shared changeReview (radio/session/profile/preset/PCB/report/RTC/checkpoint/module-reset/guided-start) AND PD review to native modal dialog. Common-root placement avoids hidden section ancestry. Dim/blur backdrop, bounded scrolling content, body scroll lock, page inert, safe Batal focus, Escape/Batal discard, Tab/Shift+Tab wrap inside dialog, original trigger focus restored. Stop output remains accessible INSIDE each popup. Only one review at a time; navigation/disconnect/context/Stop guards retained. No scrollIntoView on confirmation. Existing direct observations remain direct; no extra confirmation stage added.

Node UI PASS including previous radio/RTC/metadata/reset/profile/PD guards plus modal open/cancel/focus/keyboard/Stop/navigation. Real LOCAL PREVIEW browser confirms general+PD dialogs centered; page scroll445 unchanged across opening and focus returns to selectDevice; PD Tab now wraps to modalPdStop, Escape/Batal closes, no PD commands/hardware. First compile06:28:26Z cancelled for explicit keyboard wrap improvement, NEVER upload. Final frozen1.16.0/build2026-10-16T06:31:10Z (13:31:10 WIB) running; no later firmware edits.

Actual144/C28C1.15.1 had rebooted/unselected; found exact same-unit/build saved6F614ECF M1rev3 fullcycle radioPASS. Archived checkpoint BEFORE explicit same-build resume; current33PASS1FAIL0pending restored and backed up ignored build/modal-*. Preserve CURRENT actual baseline (not prior intentional waiting result from1.15.1 task). No radio GET/SEND/reset or profile changes for visual tests. New narrow upgrade ONLY exact1.15.1/build04:18:24Z->1.16.0, same unit/profile/recipe/calibration/RTC revision. Compile/sourcepush/OTA/resume/full ALL verdict+raw radio evidence preservation/actual modal cancel-only visual acceptance pending. Never mark hardwarePASS from preview.

### V2.55 | 2026-10-16 | Codex - complete actual acceptance

Installed1.15.1/build2026-10-16T04:18:24Z (11:18:24 WIB) via app-only OTA144/C28C. Compile/metadata/legacy gates PASS; app1557440 SHA256a02b1de193c17ffb982361ce443918fa36444cbaadd5b9f8b6cc49c10e92228a MD5bcbf71f24a6e0e168b2190e67e71c708, sketch1557289 bytes74%, staticRAM57672 bytes17%. Source5eccfcb feature pushed BEFORE OTA, exact runtime build/MD5/unit/profiles/calibration verified. No firmware edits after build start. No main merge/erase/profile revision or mask change.

ACTUAL browser acceptance: upgraded narrow exact1.15.0->1.15.1, retained actual failed earlier transaction and ALL unrelated own6F614ECF M1rev3 results/evidence. GET ->MENUNGGU SEND (32PASS1FAIL1pending), SEND enabled despite legacy configOFF. Clicked Send+Get, review visibly scrolls into view, confirmed CNCFG0->0. Real TX AT+CNCFG=0 / RX OK, all7 GET readbacks valid, ACK mask1/write mask1/writeAttempted+writeVerified true ->cyclePASS (33PASS1FAIL). This tests real write ACK/readback, not changed-value or power-cycle persistence. All7 final settings identical to original; no factory defaults/autorestart/RF frames.

Uploaded IDENTICAL final binary once more to test ESP soft-reboot/checkpoint recovery. Same-build explicit resume retained fullcyclePASS and receipt GET_SEND_GET_V1; ALL unrelated PASS/FAIL/details/rawanalog/Modbus/network/RTC/PCB/operator/reset/profile/calibration remained exact. This is checkpoint retention, not radio power-cycle certification. Then deliberately started a NEW GET through actual UI to prove oldPASS cannot carry into a new cycle: final MENUNGGU SEND, writeAttempted/writeVerified false, SEND ENABLED, totals32PASS1FAIL1pending; remaining pending is RADIO waiting operator SEND, existing DO2FAIL retained. Final checkpoint saved/readback clean. User can edit then Send+Get and confirm; no profile switch/reset needed.

Node UI tests PASS (M1 SEND, GET-only pending, no-edit actualchannel resend/review, bothOFF, old Stop/stale guards); C++ cycle/ACK/Mode alias negative+positive assertions compiled PASS; earlier samecycle Python14 and portal2 PASS. Final live temporaryCSV HTTP200 serial-only filename/pcbNumber retained; Warranty format unchanged. Ignored evidence build/radio-cycle-* (1.15.0 realGET/oldACKfailure), build/radio-ack-* (1.15.1 migrated/get/pass/resumed/ready), screenshots radio-cycle-pass-v1_15_1.jpg and radio-cycle-ready-v1_15_1.jpg are REAL device, not preview. Final docs publication/parity only remains. No outputsON/RTC write/EEPROM/PD commands or test reset for unrelated modules.

### V2.54 | 2026-10-16 | Codex

Actual1.15.0/build04:05:15Z compile PASS app1557408 SHA2560ce062d1f15cd8bf20925cb7eb1cc2e87f936256574153f267bc617d885018f8; source18cd46a published BEFORE app-only OTA144/C28C HTTP200, exact runtime/MD5/profiles/calibration verified. Explicit resume invalidated ONLY legacy read-only radioPASS ->32PASS1FAIL1pending; all unrelated evidence retained. Real browser GET ->MENUNGGU SEND and SEND ENABLED on M1 with unchanged old configOFF. Review scrolls into view, currentchannel0->0 confirmed through actual web. Real module wrote reply bare OK (not +OK); strict old parser rejected it ->cycleFAIL, correctly noPASS. Diagnostics captured TX AT+CNCFG=0 / RX OK; final settings unchanged, no RF/output/RTC writes.

Patch1.15.1 accepts ONLY exact trimmed OK or +OK; parameter parser still rejects bareOK as data; negative ACK echo/substring tests retained. All7 readbacks+sent-key matching still mandatory. New narrow upgrade exact1.15.0/build04:05:15Z->1.15.1 only. Node UI PASS. Final frozen1.15.1/build2026-10-16T04:18:24Z (11:18:24 WIB) running; no later firmware edits. Current own6F61432PASS2FAIL, radioFAIL explicit transaction until GET retest. Final compile/sourcepush/OTA/resume/repeatactualSEND/receipt persistence/finalfreshGET readiness pending. No invented changed parameter target.

### V2.52 | 2026-10-16 | Codex

Owner correction: radio QC is ONE GET -> SEND -> GET cycle, GET-only MUST NOT PASS; SEND must be available on current M1 without resetting profile. Source1.15.0 replaces read-only PASS with MENUNGGU SEND. Actual acknowledged SEND plus all7 fresh reads and sent-key matching required for cyclePASS; errors FAIL, Stop/stale context noPASS. Radio communication now combinedcycle gate (old configOFF no longer blocks it); either existing radio switch permits cycle, bothOFF deny browser AND backend. Stable IDs/profile masks/revisions unchanged; legacy config result hidden in radio card, primary result labels fullcycle. No peer/RF test added.

No draft change -> review explicitly resends ACTUAL read channel only (not factory/default), still sends AT write and requires +OK/readback; proves transaction, not changed-value persistence. Changed fields still selective/reviewed/context-guarded. New GET starts fresh cycle, clears write proof. Snapshot criterion GET_SEND_GET_V1; resume invalidates old read-only radioPASS only, keeps unrelated PASS/FAIL and actual radioFAIL; new valid cyclePASS may survive reboot with receipt. Narrow upgrade exact1.14.1/build03:40:40Z ->1.15.0 only, same unit/profile/recipe/calibration/clock guards. Static policy tests cover GET-only/ACK/mismatch/missing-read/resume receipt; Node UI tests M1 SEND without config switch, no-edit review sends actualchannel, bothOFF gate and previous Stop/stale controls PASS.

Final frozen build1.15.0/build2026-10-16T04:01:50Z (11:01:50 WIB) running. Device144/C28C actual1.14.1 own6F614ECF33PASS1FAIL/outputsOFF/TPLready verified; latest backup beforeOTA required. Compile/sourcepush/OTA/resume/actual GET waiting+SEND ACK/readback/UI final verification pending. Preserve all unrelated results, no global reset/erase/profile change. Operator may edit parameters; bench check may resend actual channel preserving all final settings.

### V2.51 | 2026-10-16 | Codex - installed and verified

Final1.14.1/build2026-10-16T03:40:40Z (10:40:40 WIB) compile/metadata/legacy gates PASS; app1555920 SHA25680d5b01902c4481474bfcadc29121db2c8ecaec37987d4a2649ea126a6dad984, MD5e893bb27af1ebdea8751c6b659a5c82d, sketch1555765 bytes74%, staticRAM57680 bytes17%. Source be8c82a published BEFORE app-only OTA144/C28C HTTP200; exact runtime build/MD5/unit/profiles/calibration verified. Explicit narrow1.14.0->1.14.1 resume preserved own6F614ECF M1rev3. Compared all original non-radio PASS/FAIL/details and rawanalog/Modbus/network/RTC/operator/PCB/reset metadata; checkpoint saved/readback clean. No erase/profile reset/outputON/radio writes/RF test/RTC write.

Actual Get now PASS all7: CNCFG0 WMCFG4 TMCFG1 PIDCFG65535 UBCFG7 UPCFG0 PWCFG0. Live E70V1.0 responds WMCFG query Mode:4; matching alias fixed root cause. Real browser1.14.1 confirms auto-filled actual values; M1 write switch stillOFF / DIABAIKAN PROFIL, so no physical write acceptance claimed. Final totals33PASS1FAIL0pending; remaining FAIL Relay:1 / DO2 retained, never coercedPASS. User may enable Radio - Tulis + verifikasi in a future configured session; active profile change resets session by existing design. Operator must supply target, review and confirm; no CPU factory defaults/autorestart/peerRF.

Real browser clicked CSV QC sementara and downloaded C:/Users/taufi/Downloads/TMC88F1559CA28C.csv. Actual HTTP filename serial-only verified; downloaded UTF8BOM CSV retains qcCode/pcbNumber/full columns. Warranty exact10 export unchanged. Node UI regressions PASS incl autoGet/draft retention/serialfilename/write-review/Stop/stale readId; Python14 and portal2 PASS; Mode alias and bound/wrongkey assertions compiled. Screenshot ignored build/radio-final-v1_14_1.jpg (actual installed, not preview); backups radio-flow-* and radio-alias-* ignored. Preview server/tab stopped. Final worklog publication/checkpoint only remains; no main merge.

### V2.50 | 2026-10-16 | Codex

Actual1.14.0/build03:35:28Z app-only OTA144/C28C HTTP200, exact runtime MD5/profiles/calibration verified. Explicit upgrade/resume preserved own6F614ECF33PASS1FAIL and all unrelated details/rawanalog/Modbus/network/RTC/PCB/operator/reset metadata. Actual READ captured E70(915NW14S)_V1.0 response WMCFG query -> Mode:4 (twice). Root cause confirmed parser label alias, not absent hardware response. Read correctlyFAIL while7th field unknown, current32PASS2FAIL; both before/after backups ignored build/radio-flow-* and radio-alias-*.

Patch1.14.1 accepts Mode only for WMCFG with original0..4 bounds; negative assertions reject Mode for channel/outofrange. No commands/defaults/write/restart/profile/pin changes. Narrow upgrade only exact1.14.0/build03:35:28Z ->1.14.1; same-unit/profile/recipe/calibration/RTC revision checks retained. Node UI PASS. Final frozen build1.14.1/build2026-10-16T03:40:40Z running;03:40:02Z candidate cancelled before bridge completion, NEVER upload. Final compile/push/OTA/resume/rawread verification pending. M1 writeOFF respected; physical write not tested/no invented target; no peerRF test. Temporary fullQC CSV now serial.csv browser/server, Warranty unchanged.

### V2.48 | 2026-10-16 | Codex

Owner clarified Get uses actual stored module parameters, never CPU defaults; automatic form fill after completed fresh Get, polling preserves edits. Send+Get reviews only changed fields, requires ACK and matching readback; observed read/write transaction failure is FAIL, stale context/cancellation remains incomplete/unverified and never PASS. No peer/RF QC performed. Existing Radio configuration switch still respected (current M1 OFF), no automatic profile reset. Temporary QC CSV now serial-only filename in browser AND HTTP header; content/columns/PCB retained, Warranty unchanged. UI tests include auto-fill/draft retention and serial-only temporary CSV filename, PASS.

Owner reconnected144/C28C: installed1.13.1; reboot left profile unselected but own6F614ECF checkpoint intact. Explicit same-build resume HTTP200 restored current own session (never oldPCB0004/sessionB6361F85). Fresh pre-upload archive required. Final revised build1.14.0/build2026-10-16T03:33:19Z started, source frozen. Earlier03:21/03:29 artifacts superseded/NEVER upload. Final compile/push/OTA/resume/read/rawWMCFG diagnosis pending. No desired target values authorized: operator edits through web; do not invent channel10 write.

Radio V2.44: source1.14.0 simple read/edit/write/readback + raw strict parser, stable IDs. Build03:09:28Z pending/software PASS. Latest current144/C28C session6F614ECF/PCBempty/32PASS1FAIL1pending archived. Preserve old results; no actual radio writes until desired values known. See CURRENT/RADIO_WORKFLOW.md.

Warranty FAIL V2.43 COMPLETE1.13.1/build11:36:34Z OTA144 verified; actualFAIL export200 preservesFAIL, exact10/serial.csv, date/PIC guards. Original33PASS1FAIL/PCB0004/session/RTC1/raw evidence preserved. Source57448f8 pushed beforeOTA, final docs-only feature publication/no merge. See CURRENT.

Warranty FAIL V2.41: owner permits actualFAIL export with qcStatusFAIL, no forcedPASS. Source1.13.1/UI tests done, build11:36:34Z running; preserve current144/C28C/33PASS1FAIL/PCB0004. Next gated compile/push/OTA/explicit1.13.0 upgrade/live read-onlyFAIL CSV. See CURRENT.

Module reset V2.40 COMPLETE1.13.0/build11:18:57Z OTA144 verified. Summary PIC/error focus + direct allDI/DO/module reset, real Radio reset/readback and reboot recovery verified; own34PASS/PCB0004/RTCquick1/raw analog+Modbus retained. Source397858b published beforeOTA; final docs-only feature checkpoint/no merge. See CURRENT for limits.

Module reset V2.39 SOFTWARE COMPLETE1.13.0/build11:18:57Z; allDI/DO/module buttons, summary PIC and actionable errors tested. Actual144 offline/OTA NOT done; preserve25PASS/PCB0004/RTCquick1. Final feature publication and exact resume procedure in CURRENT.

Module reset V2.37: source1.13.0 per-module reviewed reset implemented; software PASS, frozen build09:28:18Z pending. Preserve actual25PASS/PCB0004/RTCquick revision1 on144; exact1.12->1.13 resume. See CURRENT/MODULE_RESET.md for pending compile/OTA/runtime/reboot/publication.

Warranty/PCB V2.36 COMPLETE firmware1.12.0/build08:59:41Z OTA144/newserialC28C verified; PCB/fullCSV/same-build reboot/resume PASS; temporaryPCB cleared, own1PASS/33pending/clean checkpoint. Warranty exact10/software success+live rejection verified, live PASS download/import pending actual production QC. Source d1ef3d2 published before OTA; final docs-only feature publication, no merge. See CURRENT.

RTC QUICK V2.32:1.11.0/build08:16:55Z installed OTA; actual Operator reviewed challenge/write/readback/restoration PASS,34PASS, original33/82CSV exact after reboot/resume. Daily/battery sampling remains separate, not PASS. ClockRevision2/no run/checkpoint clean; source provenance preserved. Current procedure/limitations in CURRENT V2.32 and RTC_PORTAL_CONTRACT.

RTC V2.28 COMPLETE firmware:1.10.6/build06:57:45Z installed OTA verified. Original33 and82 CSV analog/Modbus columns retained through two real software reboots, RTC set/readback/WIB/60s and persistent run lock PASS. No24h/physical battery PASS; portal integration separate owner. ClockRevision1, no active run, outputsOFF, clean checkpoint; NEVER reimport legacy/reset casually. Current detailed evidence and continuation in CURRENT V2.28.

RTC V2.23: source1.10.4/build06:42:50Z compile/software PASS. Device192.168.1.31 unreachable; last verified1.10.3. OTA/migration/RTC60s/reboot/native pending. Preserve original33; exact continuation in CURRENT. No24h/battery PASS.

# RTC durable QC1.10.0 - local implementation2026-10-15

COMPACT-RTC-1 contract04_Dokumen/RTC_PORTAL_CONTRACT.md: explicit RTCset/readback/OSF handling+offset metadata/clockRevision, monotonic60s tick, persistent portal run lock, bounded immutable external accuracy+retention summaries and RTC-only retest. RTC no longer PASS from clean OSF alone. Progress own NVS CRC/readback, explicit resume (same build/profile/recipe/calibration), completed evidence retained/labeled; outputs OFF and partial jobs not resumed. Autosave opt-in after save/bind/report, max1/min; explicit save before power-off. No erase.

Live1.9.1 user sessionEB4CDCB2 has33 existing PASS/RTCUNVERIFIED, not Codex physical acceptance. Original sanitized state+CSV archived ignored build/rtc-legacy-original-v1_9_1.json/.csv; JSON17866 bytes SHA25699fe1e83761fa93e1fb157afc51bb68080e26e8479b8e23c7548b5d98768a8fb. Original operator empty, do not fabricate PIC. One-time explicit1.9.1 CRC-verified import bridge preserves these33 results after first upgrade, origin build/code traced; never force ordinary cross-build resume. Do not upload until original evidence safely saved and bridge tested.

Build03:01:50Z is a compile CANDIDATE ONLY: source changed afterward for safety/legacy migration fixes. Never upload it. Need fresh final source-frozen Build-Firmware.ps1, metadata/legacy gates, API/actual60s tick and same-binary reboot/resume proof. Node prior regressions pass; additional RTC cases underway. No portal repo edits; portal integration/24h/physical battery acceptance pending. Read rules/version/current/task/readme/validation/handoff; feature baselinea20e067 clean/fetched, existing WiFi/pins/polarity/partition/calibration kept.

# ST01..07 SOFTWARE/OTA DONE1.9.0

Exact1.9.0/build00:17:08Z installed and MD5/serial verified. Recipes persisted across second same-binary OTA boot; old profiles/calibration retained. Node/Python14/portal2/QR independent decode, live recipe/report/laptop archive API PASS. Native browser/mobile/physical limitations and publication in CURRENT. No actual outputON/EEPROMWrite/PDrequest/radioApply.

# ST01..07 local1.9.0 -2026-10-15

Seven scoped follow-ups implemented; Node/Python14/portal2/independentQR decode PASS. Firmware build/OTA/runtime/browser acceptance pending. Existingdevice1.8.0 evidence historical below. CURRENT/04_Dokumen/QC_STATION_REVISION.md is continuation entry. No physicalON/EEPROMWrite/PDrequest/radioApply.

# WEB01..07 SOFTWARE/OTA DONE1.8.0

Final build14:54:18Z gates PASS, exact runtime MD5/serial and profile/calibration persistence verified. Report API/CSV/RTC runtime PASS, Node/Python10/portal2 PASS. Browser visual/layout/archive persistence is not claimed after policy interruption; current limitations/publication in CURRENT. Physical PD/WC/load acceptance remains separate.

# WEB-01..07 revision1.8.0 local -2026-10-14

Implemented operator workflow/confirmation/grouped profiles/report snapshots+browser archive and RTC OSF semantics. Node/Python10/portal2 PASS; preview/build/OTA pending. Device prior1.7.0. Scope04_Dokumen/OPERATOR_WEB_REVISION.md; next action/current publication in CURRENT. Older installed checkpoints below historical.

## PD-REV-06 DONE SOFTWARE / installed1.7.0

Final build13:42:34Z gates PASS, authorized OTA and exact runtime verified, saved profiles/calibration retained. Presets/slider/review/cancel/browser/software tests PASS. PD0x28 absent on M0; actual capabilities/voltage unverified, no accepted voltage command. Final M1 PD OFF. Feature-only publication recorded in CURRENT. Earlier in-progress entries are historical.

## PD-REV-06 -2026-10-14

Source1.7.0 presets/slider/explicit confirmation DONE SOFTWARE; tests PASS, browser preview PASS. Final build started2026-10-14T13:39:48Z, no firmware edits after start; build/OTA pending. Physical PD voltages remainUNVERIFIED. See CURRENT and04_Dokumen/PD_CONTROLS.md.

## Final continuation - v1.6.1 / 2026-10-14

Revisions REV-01..05 are tracked in04_Dokumen/QC_PROFILE_REVISION.md. Software implemented/build/tests PASS; authorized OTA192.168.1.31 installed1.6.1/build11:45:51Z, exact MD5/serial match, calibration/profile persistence verified. AHT now live valid. EEPROM Read fixed but Write remains rejected Wire4; M1 ignores EEPROM per user fallback. M1 also excludes Radio configuration/RS232/PD. Editable M2 example saved with both DO OFF; browser toggles/session/CSV verified. UI click queue/Stop/stable DOM and profile-transition fix included. No general serial-web work or automatic main/develop merge. See CURRENT for final publication state; physical output/accuracy/EEPROM-WC followup remains explicit.

## Resume audit - 2026-10-14 17:49 UTC+7

- Open project: Compact QC only. Clean local feature/qc-operator-ota was 12 commits behind; fetched and fast-forwarded to the exact remote checkpoint add83d971df4e2de5eb994beeae4a735058e1145. Remote main remains35733ba; no remote develop exists. No main/develop merge.
- Source truth: firmware_version.h1.5.1; relay_profile.h GPIO32/33 active HIGH (ON=HIGH/OFF=LOW), LED MCP active LOW. User reports failed relay as hardware; no fresh physical evidence collected.
- QC-02/04/05: v1.5.1 SOFTWARE PASS per checkpoint and current regression run; BENCH PENDING. Earlier table/status and device readings below are historical. The obsolete instruction not to invert DO is superseded by the user's both-HIGH correction. QC-16 remains BENCH FOLLOWUP; do not reopen QC-09 without new evidence.
- This continuation corrects contradictory operator/install polarity, version and serial-deferral text. Firmware source/version stays1.5.1. Node UI PASS, Python10 PASS, portal2 PASS. No new firmware compile, device contact or upload.
- Local build/compact_qc_tester.ino.bin is an older1120144-byte app, SHA256231291610a98957b526c86115ccc9534da0ec1e35febba30538ba2e87ddb55c5, with no firmware-manifest.json or build_metadata.h. It is not the gated v1.5.1 app and must not be relabelled or installed as such.
- Vault paths referenced by AGENTS are absent here; targeted searches under D:/Home Work, C:/Users/taufi/Documents and C:/Users/taufi/.codex found no Rules_Versioning.md or Rules_Change_Notifications.md. Resolve/read these before a fresh build. The checkpoint's v1.5.1 build evidence is retained as historical evidence, not reproduced locally.
- Next: locate the mandatory vault rules; if a fresh binary is needed, use Build-Firmware.ps1 and retain exact manifest/hash. Team verifies installed version/buildUTC, repaired relay, GPIO vs physical DO behavior and Stop/timeout; EEPROM/PD followup requires fresh errors/observations. Publication state is recorded in CURRENT/worklog and Git history.

## Reopened output report � 2026-10-14

QC-02/QC-04/QC-05 reopened for DO1 always physically OFF (user confirmed) and DO2 inverted on installed1.5.0. v1.5.1 fixes proven startup sequencing bug and corrects BOTH DO1/DO2 toHIGH; user identifies the failed relay as hardware. Final1.5.1 build/tests PASS; see latest branch worklog. Physical acceptance pending team install. Earlier DONE SOFTWARE does not certify physical output behavior.

# Backlog revisi Compact QC — 13 Oktober 2026

## Status saat handoff

- **Implementasi source1.5.0 selesai/diverifikasi software; physical acceptance belum dilakukan.** User melanjutkan setelah checkpoint13:52, tanpa deadline baru.
-14 dari15 tugas aktif DONE SOFTWARE. QC-16 DIAGNOSED/BENCH FOLLOWUP; ADS QC-09 CLOSED USER. Jangan menyamakan software DONE dengan hardware PASS.
- Branch feature/qc-operator-ota. Source checkpoints1.3.0 (468e781),1.4.0 (b36f3d6); final1.5.0 sudah dipush sebagai fa0624ea3d979f2a87ff3db118a46c4ad6ca0387 dan SHA remote diverifikasi pada2026-10-14. Tidak merge main/develop.
- Final build **2026-10-13T09:34:41Z /2026-10-13 16:34:41 UTC+7**, app1260416 bytes, metadata/partition gates PASS; rincian lengkap VALIDATION.md.
- Actual device192.168.1.55 masih1.2.0/build2026-10-12T21:08:39Z. No upload; tim pengguna menginstal firmware lalu menjalankan QC_OPERATOR_GUIDE.md.
- QC-16 fakta: EEPROM57 merespons tetapi belum ada identitas QC valid; slot kosong/foreign/corrupt belum diketahui pada firmware lama. PD28 tidak merespons scan pada M0; periksa populasi/power/bus atau pilihM1 jika memang unpopulated. Source menambahkan diagnosis/read raw/protocol retry; tidak mengarang keberhasilan tegangan.
- Next: team install, physical tests pada guide dan feedback untuk hardware followup. Tidak ada tugas source lain yang sengaja ditinggalkan tanpa implementasi.

## Checklist dengan ID tetap

Status `TODO` = belum diimplementasikan atau diverifikasi. Jangan mengubah status hanya karena fungsi serupa ada pada v1.2.0.

| ID | Status | Tugas | Kriteria selesai |
| --- | --- | --- | --- |
| QC-01 | DONE — SOFTWARE | Hasil QC jelas pada setiap menu, termasuk OLED dan semua modul komunikasi | Badge/teks PASS atau FAIL besar, warna dan alasan hasil terlihat di container modul, konsisten dengan Ringkasan QC dan report; jangan hanya menampilkan raw JSON. Belum diuji/berjalan/tidak berlaku dibedakan dari hasil final. |
| QC-02 | DONE — SOFTWARE | Perbaiki ON/OFF LED yang dilaporkan terbalik | Perintah ON menyalakan LED, OFF mematikan LED; verifikasi mapping/polaritas aktual. Pengguna menyatakan output module sudah benar: jangan membalik DO. |
| QC-03 | DONE — SOFTWARE | DO1 dan DO2 independen | ON/OFF DO1 tidak mengubah DO2, dan sebaliknya; keduanya dapat ON bersamaan. Kontrol/konfirmasi/QC per modul tetap terpisah. Stop semua, reset sesi dan OTA tetap eksplisit mematikan semua. |
| QC-04 | DONE — SOFTWARE | Highlight ON/OFF mengikuti status, default OFF | Biru berpindah ke OFF ketika OFF diterima, kembali ke ON ketika ON diterima; startup semua output OFF. Status UI berasal dari state firmware/perintah yang berhasil, bukan warna ON permanen atau optimisme setelah request gagal. |
| QC-05 | DONE — SOFTWARE | Perbaiki kegagalan komunikasi kontrol output | Cari penyebab request output sering gagal; tampilkan error sebenarnya dan state terakhir tanpa mengklaim berhasil. Verifikasi cepat berganti ON/OFF, polling, heartbeat/session, reconnect dan akses tiap baris. Penyebab saat ini belum terbukti. |
| QC-06 | DONE — SOFTWARE | Sederhanakan konfirmasi fisik output dan latch hasil | Hanya pilihan **Sesuai** / **Tidak sesuai**. Setelah hasil QC PASS/FAIL final, tidak perlu meminta konfirmasi lagi; pengguna tetap bisa ON/OFF tanpa menghapus hasil. Reset hanya melalui mekanisme tes/sesi ulang yang jelas. |
| QC-07 | DONE — SOFTWARE | AHT10 terdeteksi tetapi gagal membaca data | Diagnosis init/status busy/timing/read length/I2C; tampilkan temperature dan humidity valid atau alasan gagal yang jelas. ACK alamat saja tidak menghasilkan data/PASS. Tetap AHT10 sesuai konfirmasi pengguna. |
| QC-08 | DONE — SOFTWARE | Tambah konfirmasi QC I2C oleh pengguna | Setelah scan, daftar alamat tetap ada; sediakan tombol PASS/FAIL user dan hasil jelas dalam menu/report. Label hasil sebagai konfirmasi pengguna atas scan, bukan bukti identitas IC atau seluruh protokol. |
| QC-09 | CLOSED — USER | ADS1115 tidak terdeteksi | **Dibatalkan sebagai perbaikan firmware:** pengguna memastikan masalah hardware dan sudah OK. Jangan membuka kembali tanpa bukti baru. |
| QC-10 | DONE — SOFTWARE | QC akurasi 4–20 mA dengan lima titik | Container baru, lima target uji, nilai hasil pembacaan/capture dan konfirmasi Sesuai/Tidak sesuai per titik. Kelima titik sesuai -> PASS; titik tidak sesuai -> FAIL; titik belum selesai tidak PASS. Kalibrasi empat titik yang sudah berhasil tetap dipertahankan dan terpisah dari QC. |
| QC-11 | DONE — SOFTWARE | Modbus RS485 master dengan dua tag independen | Dua config terpisah, Request masing-masing, transaksi ke slave/register tertentu, data hasil tampil, konfirmasi Sesuai/Tidak sesuai menjadi QC. Config mengikuti rincian di bawah. Request sungguhan diperlukan; menyimpan config bukan PASS. |
| QC-12 | DONE — SOFTWARE | Skenario dan QC RS232 jelas saat mode dipilih | Jelaskan wiring laptop USB-RS232, default serial, langkah pengujian, pola paket, timeout/CRC dan kriteria PASS/FAIL. Saat memilih RS232 tampilkan UI QC yang relevan; jangan hanya form Modbus. Pertahankan integrasi penguji laptop lokal yang telah dibuat. |
| QC-13 | DONE — SOFTWARE | Pisahkan menu RS485/RS232, komunikasi dan peripheral SPI | Menu khusus RS485/RS232. Peripheral SPI memuat SD card dan pemeriksaan ETH melalui SPI. Komunikasi memuat jaringan ETH/Wi-Fi, MQTT serta tes/config radio. Bedakan pemeriksaan chip ETH dari konektivitas jaringan ETH. |
| QC-14 | DONE — SOFTWARE | QC jaringan Wi-Fi/ETH dan config MQTT | Setting jaringan/broker MQTT, IP aktual, status koneksi dan hasil QC yang jelas. Koneksi jaringan/IP dan keberhasilan koneksi MQTT harus terbukti, bukan sekadar config tersimpan atau PHY link ON. Secret tidak ikut report/log/push. |
| QC-15 | DONE — SOFTWARE | Hasil radio jelas, baca data dan konfigurasi E70 | Pengguna menyebut LoRa; hardware terkonfirmasi E70-915NW14S. Tampilkan hasil komunikasi dan konfigurasi dengan jelas, data terbaca dan hasil config/readback; jangan hanya raw JSON. Rincian write/volatile/NVM perlu ditentukan sebelum implementasi, tidak otomatis menulis NVM. |
| QC-16 | DIAGNOSED — BENCH FOLLOWUP | Diagnosis fitur identitas EEPROM / PD Sync masih error | Screenshot terakhir memuat keduanya; perlakukan keduanya sebagai perlu diagnosis sampai diketahui fitur mana yang gagal. Perbaiki Write+verify/Read EEPROM dan/atau Request PDO sesuai bukti. Tidak ada pesan error lengkap yang diberikan; jangan menganggap record kosong atau sourcePDO kosong sebagai akar masalah. |

## Rincian dan batas yang harus dibawa ke implementasi

### Output dan konfirmasi

- Fakta source baseline: README v1.2.0 mendokumentasikan hanya satu output ON sekaligus. Ini bertentangan dengan permintaan baru DO1/DO2 independen; revisi perlu mengubah model state, bukan sekadar tombol frontend.
- Keputusan terbaru pengguna2026-10-13: DO1/DO2 tetap aktif LOW; **LED MCP aktif LOW**, menggantikan jawaban HIGH sebelumnya. Source telah diperbaiki; bench acceptance belum dilakukan.
- Jangan menonaktifkan heartbeat, timeout, I2C fault lock atau stop OTA untuk menyembunyikan error komunikasi. Terapkan pengamanan pada state setiap output.
- Hasil QC final tidak boleh hilang karena perintah ON/OFF lanjutan. Tentukan reset tes secara eksplisit dan tampilkan kapan hasil lama berlaku.
- OLED perlu membedakan init berhasil dari hasil observasi user; tulisan `OLED init PASS` bukan pengganti hasil QC visual.

### Analog

- Pengguna menyatakan ADC dan kalibrasi empat titik **sudah berhasil**. Gain/offset aktif dari NVS tetap dipakai.
- Lima titik QC adalah uji setelah kalibrasi, bukan mengganti fit empat titik menjadi lima titik.
- Nilai default lima target, toleransi, cara capture dan reset hasil belum ditentukan pengguna. Pilih dan dokumentasikan saat `kerjakan`; jangan mengklaim telah disetujui sekarang.
- Simpan evidence tiap titik (target, nilai terbaca, konfirmasi, waktu/sesi dan kalibrasi yang digunakan) dalam report. Perubahan kalibrasi harus membatalkan evidence QC lama yang tidak lagi sesuai.

### RS485 dua tag

- Pengguna kini meminta transaksi Modbus, menggantikan penundaan Modbus sebelumnya; implementasi telah diizinkan hanya sampai deadline sesi; belum dikerjakan pada batch output ini.
- Config link: mode, baud, parity dan slave ID; contoh baseline 9600/N/slave1, bukan universal.
- Dua tag dikonfigurasi independen: nama tag (contoh UI max10 karakter), function **Read Coils (01), Read Discrete Inputs (02), Read Holding Registers (03), Read Input Registers (04)**, datatype (contoh Unsigned; opsi lain ditentukan dengan representasi yang didukung), alamat dan length (contoh max2).
- Sebutkan alamat register sebagai offset protokol atau notasi vendor, agar tidak ambigu 0 vs 40001. Bedakan bit coils/discrete dari register 16-bit. Endianness/word order untuk nilai multi-register belum diketahui.
- Device berperan master; arah TX/RX RS485 otomatis tanpa DE/RE sudah dikonfirmasi. Route MCP2A2 LOW untuk RS485 / HIGH untuk RS232.
- Request tiap tag menampilkan nilai mentah dan hasil interpretasi beserta error timeout/CRC/exception. Response invalid tidak boleh dikonfirmasi PASS sebagai data valid.
- Hasil per tag wajib jelas. Aturan agregasi QC RS485 (misalnya kedua tag selesai/sesuai) perlu dinyatakan saat implementasi.

### RS232 dan radio

- RS232 adalah interface listrik; tidak memiliki satu protokol/baud QC universal. Baseline custom binary laptop 9600/8N1/100 paket adalah skenario proyek, bukan sertifikasi standar RS232. Jangan menyamakan adapter USB-TTL atau echo lokal dengan pengujian transceiver.
- Pertahankan backend laptop lokal terpisah dan jelaskan proses Start/hasil device vs hasil laptop, wiring TX/RX/GND dan kecocokan adapter.
- E70 baseline terkonfirmasi CC1310/15.4 star, **bukan LoRa**. SF/BW/coding rate/CRC LoRa tetap tidak didukung; jangan menandainya PASS atau mengarang perintah.
- Pengguna kini meminta config radio, berbeda dari v1.2.0 read-only. Jenis perubahan yang dapat dilakukan, prosedur aman, target produksi dan persistensi belum dipastikan. Readback/konfigurasi saja tidak membuktikan RF link antarmodul.
- QC otomatis: skenario selesai dengan evidence valid -> PASS; kegagalan skenario yang terkonfirmasi -> FAIL. Belum diuji, dibatalkan, unsupported atau tanpa evidence jangan diubah menjadi PASS. Tampilkan alasan kegagalan/ketidaklengkapan di container masing-masing.

### Jaringan, I2C, EEPROM dan PD

- ETH memiliki dua lapisan: peripheral SPI/controller/link dan komunikasi jaringan/IP/MQTT. Hindari dua hasil PASS yang mengklaim hal sama tanpa tes berbeda.
- Wi-Fi lab default HuaweiJIN / AP12345678 tetap baseline. Setting jaringan baru/MQTT jangan membocorkan credential ke public repo; pertahankan AP pemulihan dan NVS lain.
- Granularitas tombol PASS/FAIL I2C (bus/scan keseluruhan vs tiap baris) belum dinyatakan. Permintaan saat ini adalah QC I2C oleh pengguna; scan tetap daftar alamat, tidak mengirim command model yang ditebak.
- EEPROM TOP `TMC_T_V2_R1` / BOTTOM `TMC_B_V3_R0`, C7559/M24128, QC region0x3F80–0x3FFF. Periksa write/readback, CRC dan WC; jangan menghapus foreign/production data untuk memaksa write berhasil.
- PD hanya M0. M1 STUSB4500 unpopulated tetap N-A. Periksa transaksi I2C, sink PDO/source capabilities/negosiasi dan adapter; target12V perlu compatible fixed PDO. Actual VBUS perlu pengukuran, bukan inferensi dari angka request. Tidak otomatis program NVM STUSB.

## Bukti dan unknowns

- Laporan dan screenshot berasal dari chat pengguna setelah push v1.2.0. Ambient browser menunjuk `http://127.0.0.1:8769/#comms`; ini **bukan bukti** screenshot berasal dari hardware atau localhost terhubung ke ESP.
- Sebelum diagnosis, bedakan preview mock, web laptop8770 dan live dashboard ESP; periksa URL/version/buildUTC/API/error yang sebenarnya. Jangan menyimpulkan AHT/PD/EEPROM rusak dari placeholder preview.
- Screenshot dipakai sebagai referensi tampilan/keluhan; tidak dimasukkan sebagai asset repo publik. Deskripsi dan kriteria di atas cukup untuk melanjutkan pekerjaan tanpa file Temp laptop.
- Unknowns: versi perangkat aktual, pesan error output/EEPROM/PD lengkap, model sensor fisik yang terbaca, broker MQTT/credential, target radio produksi, target lima titik QC, datatype/word order Modbus.

## Urutan kerja setelah izin memulai

1. Audit branch/status/remote dan reread aturan; baca CURRENT worklog serta dua update terbaru, kemudian backlog ini.
2. Bedakan mock vs device; kumpulkan error nyata. Mulai QC-01/02/03/04/05/06 karena fondasi state/perintah memengaruhi banyak tes.
3. QC-07/08/10/16: sensor, I2C, analog QC dan EEPROM/PD sesuai evidence. QC-09 tetap closed.
4. QC-11/12/13/14/15: Modbus dua tag, RS232, menu SPI/jaringan/MQTT/radio.
5. Integrasikan badge/menu/report/OLED, tes regresi, build dengan versi dan UTC baru; bedakan software PASS dari bench acceptance.
6. Perbarui checklist, changelog/handoff/worklog; commit/push feature branch. Tidak merge main/develop atau upload device otomatis.

Urutan ini usulan untuk kelanjutan, bukan bukti pekerjaan sudah dimulai. Keputusan baru pengguna dapat mengubah urutan/scope.

## Protokol checkpoint

- Saat mulai sebuah ID, ubah status ke `IN PROGRESS`, isi checkpoint dan CURRENT worklog.
- Saat berhenti, tulis file/baris/modul yang sudah berubah, tes yang dijalankan, error terakhir, commit yang sudah dipush, perubahan lokal yang belum dipush dan **langkah konkret berikutnya**.
- Saat selesai, checklist menjadi `[x]`/`DONE` hanya setelah kriteria diuji; tulis batas verifikasi hardware. Simpan timestamp berlabel UTC/UTC+7.
- Pertahankan ID task dan history; koreksi scope ditambahkan sebagai update, bukan menghapus permintaan yang belum ditangani.

## Checkpoint terbaru — 2026-10-13 13:31 UTC+7

- Yang dikerjakan: pencatatan backlog dan aturan resume; tidak ada source firmware/UI/backend yang diubah.
- Yang belum dikerjakan: semua15 ID aktif pada checklist.
- Versi firmware tetap1.2.0; tidak ada build, tes hardware atau upload baru.
- Baseline Git sudah diverifikasi lokal/remote `218f208`. Commit dokumen dapat dilihat dari history file ini dan branch worklog.
- Aturan dibaca: Firmware_AI_Vault_Home, Rules_Nama_dan_Folder_Workspace, Rules_Firmware_Standar, Rules_GitHub_Gitflow, Rules_Kredensial_WiFi, WiFi_HuaweiJIN, Rules_Versioning dan Rules_Change_Notifications. File lengkap berada di vault workspace, daftar path di README.
- Tahap dokumentasi tidak mengubah profil HuaweiJIN/AP12345678 atau NVS. Mekanisme lama tetap selective migration driver STA; NVS lain dipertahankan.
- Langkah berikutnya: **menunggu pengguna mengatakan `kerjakan`**.

## Checkpoint implementasi sesi13:36–13:52 UTC+7

QC-02/03/04/06/08: source selesai; tes UI lulus, compile dihentikan sebelum deadline; gate belum lulus. QC-01 badge lokal OLED/AHT/RTC/EEPROM/PD/ETH/SD/serial/radio ditambah, belum semua menu. QC-05 penyebab source terkonfirmasi: ON memanggil stop semua, heartbeat lama menolak perintah baru, poll tumpang tindih; diperbaiki tetapi kegagalan fisik/endurance belum terbukti selesai. QC-07/10/11/12/13/14/15/16 belum diimplementasikan dalam sesi ini. ADS tetap CLOSED.

Files source: app.cpp, config.h, firmware_version.h, qc_hardware.cpp, qc_math_tests.cpp, qc_session.cpp/.h, web_ui.h, output_state.h; test web-ui.test.cjs. No upload. Next: diagnosis AHT10 actual busy/init/read evidence, lalu QC lima titik dan master Modbus dua tag. Jangan menganggap data mock sebagai PASS hardware.

Final checkpoint: STOPPED before13:52 UTC+7 per user. Final1.3.0 build interrupted during library compilation; no new valid manifest/binary. Only documentation checkpoint committed/pushed;10 firmware/test files remain local (9 modified plus new output_state.h). Remote source remains1.2.0/218f208. Resume requires new user authorization and completing Build-Firmware.ps1 before source publication.

Resumed checkpoint:1.3.0 final compile/metadata/partition gates PASS (2026-10-13T08:16:19Z). QC-02/03/04/06/08 DONE SOFTWARE; physical acceptance still pending.5/15 software completed,2 partial,8 TODO. User continuation reauthorizes remaining work.

1.4.0 checkpoint: QC-07 and QC-10 software implementation/build PASS; physical acceptance pending.7/15 software DONE,2 partial,6 TODO. Read-only actual device192.168.1.55 still1.2.0; EEPROM no valid identity, STUSB28 no ACK. Next Modbus2 tags/menu/network/radio and diagnostics.

Active1.5.0 checkpoint: QC-01/05/11/12/13/14/15 implemented locally, final compile/browser gate pending. QC-16 diagnosis implemented: actual olddevice1.2.0 at192.168.1.55 has EEPROM ACK but no valid identity; PD0x28 absent. EEPROM raw-slot diagnostic and precise PD errors added; no physical write/PDO or upload. Node/Python tests PASS. Latest build PID38844; see worklog.

Final1.5.0 gate PASS. Operator guide/checkpoint documents all revised flows and remaining physical tests. No source TODO:14 software DONE,1 diagnosed hardware/bench followup,ADSclosed. Source push follows final verification; see branch history for commit.
