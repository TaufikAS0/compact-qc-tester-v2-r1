# Operator web revision - v1.8.0

Authorized2026-10-14: push checkpoint first, then implement all six review recommendations. Baseline feature HEAD e320474, firmware/device1.7.0. No main/develop merge; separate serial-web project untouched.

| ID | Scope | Acceptance |
| --- | --- | --- |
| WEB-01 | Active profile versus draft | Active ID/revision persistent; unapplied selection labelled; reset/replacement preview and explicit confirm; draft edit/save preview; session/context changes expire confirmations. |
| WEB-02 | Operator/Technician views | Operator default with next-step/navigation, optional technical details; configuration/calibration/OTA in Technician view, explicit switch available. View mode is presentation, not authentication. Stop always accessible. |
| WEB-03 | Guided summary | Applicable tests only for progress/next action; pending/fail/all filters; links to relevant page; never send hardware actions automatically. All-OFF remainsINCOMPLETE. |
| WEB-04 | Consistent cards/performance | Instructions/actions/results/details, Indonesian labels, readable touch controls and reduced repeated startup panel; retained input/output DOM, change-only text/raw rendering. Heartbeat/Stop guards retained. |
| WEB-05 | Profiles | Four grouped categories, clone profile to a new ID, enabled/disabled diff, save confirmation; stale revision enforced server-side; no automatic exclusion ofFAIL. |
| WEB-06 | Final reports | Required operator/date; server authoritative hardware/QC snapshot and CSV from same snapshot, session/revision guard, no active worker; outputs stop first. Snapshot archive in laptop browser storage plus JSON/CSV downloads. Quota/corruption errors visible; never silently delete history. Reports not written to device NVS/EEPROM. INCOMPLETE/FAIL saveable with reasons; no fabricatedPASS. |
| WEB-07 | RTC meaning | Communication/BCD validity versus time validity separate. OSF means timeUNVERIFIED and RTC notPASS; no RTC writes/time setting added. ClearingOSF alone does not prove accuracy/battery retention. |

Browser archive is local to browser/origin and survives reload/device reboot, but can be removed by browser data cleanup. Downloaded files are the durable handoff; max50 records, reject a full archive rather than silently removing old records. No background hardware actions, outputON, EEPROM writes, PD requests or radioApply during UI validation.

Keep36 stable profile IDs/16-profile layout, qc-adc/qc-profiles/EEPROM regions, pins/polarity, TPL, WiFi, core and legacy partitions unchanged. Firmware minor1.7.0 ->1.8.0; final build/OTA/push evidence recorded in CURRENT/VALIDATION. Test real browser layout at laptop/mobile widths and mock adverse cases; physical acceptance remains separate.
