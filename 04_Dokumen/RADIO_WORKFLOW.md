Current acceptance:1.15.1/build2026-10-16T04:18:24Z installed144/C28C; actual GET->SEND/OK->GET PASS and checkpoint reboot/resume verified. Final fresh GET deliberately MENUNGGU SEND with SEND enabled for owner; unchanged currentchannel0 resend tested, changed-value/power-cycle/RF untested. See CURRENT V2.55.

# Radio QC - GET -> SEND -> GET (1.15.1)

Owner correction 2026-10-16 supersedes historical separate read-PASS/write-PASS semantics.

## One QC cycle

1. GET reads all seven actual stored module parameters. Form fills automatically; CPU supplies no factory values. Result is MENUNGGU SEND, never QC PASS.
2. Edit any available field, then Send + Get and confirm the reviewed values. Only edited keys are written. If unchanged, review resends the actual channel from GET to test ACK/readback without altering final settings.
3. Firmware rechecks baseline, performs actual AT writes, requires exact OK or +OK, reads all seven fields again and compares sent keys. Only complete success -> QC PASS. Transport/ACK/readback failure -> FAIL; cancellation/stale context -> incomplete, never PASS.

Read and write use one existing profile scope. Stable IDs Radio communication/radio_comm and Radio configuration/radio_config are retained for report/profile compatibility. radio_comm now means the full cycle, and permits SEND even with legacy radio_config OFF. Either radio switch enables the cycle; both OFF blocks commands at server and UI. No active profile save/reset, no NVS schema/mask migration. Legacy config row is not a second visible result in the radio card. If both switches enabled, both legacy report rows use the same cycle verdict. No RF/peer communication is tested or claimed.

## Evidence and recovery

QC-RADIO-3 carries criterion GET_SEND_GET_V1, getStatus, cycleStatus, selected write/ACK masks, actual read values and bounded raw TX/RX diagnostics. Fresh readId, same session/profile revision, <=5min age, explicit confirm and baseline preflight remain required. A new GET invalidates previous runtime write proof and starts a new cycle. GET must be repeated after reboot before any further SEND.

Old read-only radioPASS is invalidated on checkpoint recovery unless the stored receipt proves the new criterion + actual writeAttempted + writeVerified + nonzero writeMask fully acknowledged. Unrelated module results, session, PCB, operator, calibration, profiles and RTC evidence remain intact. Valid full-cycle PASS persists as historical checkpoint evidence; outputs never resume ON. Actual radio FAIL is retained. Before upgrade archive old evidence locally.

Only exact1.15.0/build2026-10-16T04:05:15Z ->1.15.1 upgrade is permitted, with existing same-unit/profile/recipe/calibration/RTC revision guards. No generic cross-build resume.

## Hardware constraints

E70-915NW14S is CC1310/15.4 star. UART2 GPIO16/17, fixed configuration UART1152008N1; temporary MCP1B4/B5 mode bits restored, TPL B7 preserved. Actual E70_V1.0 replies Channel:0 and Mode:4 to CNCFG/WMCFG queries; bounded matching aliases required. No factory reset, automatic restart, RF payload or power-cycle persistence certification. Re-sending an unchanged channel tests ACK/readback, not changed-value persistence; operator can edit channel/PAN ID/etc for that separate observation.

## Validation

Compiled policy assertions test no PASS from GET-only, missing GET, ACK failure and mismatch; legacy PASS receipt rejected. Node UI tests include M1 combined gate, bothOFF block, actual-value resend, auto-fill/preserved drafts, explicit review, stale read/context/draft and Stop cancellation. See CURRENT/VALIDATION for exact build and live OTA acceptance; never infer hardware acceptance from software tests.

Actual E70_V1.0 write reply captured on2026-10-16 is bare OK. Accept exact trimmed OK or +OK in write-ACK stage only, with matching readback still required. Never accept NOT OK, substrings, AT command echoes, or use OK as a parameter value.
