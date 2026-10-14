CURRENT | V2.4 | 2026-10-14 | Codex | Published source1.6.1 b39d447; device/build/MD5 verified; AHT/profiles/UI PASS; EEPROM ignored M1 | Next: optional hardware WC evidence in issue1; no source/upload work pending
READING RULE | Read the CURRENT line and the first two entries under Latest Updates before editing.

# Branch Worklog

- Branch: `feature/qc-operator-ota`
- Project: `compact-qc-tester-v2-r1`
- Purpose: checkpoint pekerjaan QC agar dapat dilanjutkan setelah interupsi.
- Task source: [Backlog revisi QC](../tasks/2026-10-13-qc-revision-backlog.md).
- Sequence V1.1 adalah versi worklog, bukan versi firmware. Firmware baseline tetap1.2.0.

## Latest Updates

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
