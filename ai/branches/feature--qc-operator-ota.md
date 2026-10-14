CURRENT | V1.7 | 2026-10-14 | Codex | Firmware1.5.0 published fa0624e; local/remote verified clean | Next: team upload and physical acceptance
READING RULE | Read the CURRENT line and the first two entries under Latest Updates before editing.

# Branch Worklog

- Branch: `feature/qc-operator-ota`
- Project: `compact-qc-tester-v2-r1`
- Purpose: checkpoint pekerjaan QC agar dapat dilanjutkan setelah interupsi.
- Task source: [Backlog revisi QC](../tasks/2026-10-13-qc-revision-backlog.md).
- Sequence V1.1 adalah versi worklog, bukan versi firmware. Firmware baseline tetap1.2.0.

## Latest Updates

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
