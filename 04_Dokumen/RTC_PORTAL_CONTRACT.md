# Compact RTC + durable QC integration (COMPACT-RTC-1)

Firmware owner: this Compact repository. Portal owner: separate Telemetric Hardware Portal repository. No portal source changes here. Source1.10.0; **check CURRENT/VALIDATION for exact installed build**, never infer deployment from this document.

## Invariants

- DS3231 at0x68. Existing SDA21/SCL22, outputs, WiFi, partitions and other NVS namespaces unchanged. New namespaces qc-rtc/meta-v1 and qc-progress/session-v1 only. No EEPROM identity writes or NVS erase/format.
- All POST bodies are application/x-www-form-urlencoded. Existing device token from GET/api/state remains required. This intentional lab token model is not a cryptographic signature or Internet authentication.
- UTC internally. RTC stores a local calendar; offset must be explicitly supplied. Calendar2000..2099 only. Firmware does not contact NTP; portal owns independent reference quality and network/quantization uncertainty.
- An OSF-clean clock is not overall RTC PASS. RTC PASS requires the60s monotonic ticking test plus accepted accuracy and retention evidence. Retention is explicitly operator-declared physical power removal, not inferred from reboot. Other QC tests may continue during portal run.
- A portal run binds the unit's current session/clockRevision, locks time setting and deliberate new-session/profile/recipe changes, and persists that lock across reboot. Finish or explicitly abort/release first. Never auto-sync RTC during a drift run.

## Read

`GET /api/rtc/sample` (Cache-Control:no-store) returns:

```
{serial,sessionId,profile,profileRevision,version,buildUTC,
 rtc:{schema:"COMPACT-RTC-1",bootId,sampleUptimeUs,sampleCompletedUptimeUs,
 resolutionMs:1000,communicationValid,valid,timeValid,oscillatorStopped,
 time,rtcLocalEpochSeconds,utcSeconds,utcOffsetMinutes,timezoneConfigured,
 clockRevision,metadataReady,writeVerified,lastSetUtcSeconds,
 temperatureC,runningTest,runningTestPassed,runningElapsedSeconds,
 externalRunId,externalRunSessionId,externalEvidence,detail}}
```

Missing/invalid fields must not be coerced into time0 or valid measurements. `utcSeconds` exists only after explicit successful setting with persisted timezone metadata. `timeValid` means valid calendar/OSF clear, NOT independently correct absolute time. Uptime is monotonic microseconds within `bootId`, unrelated to UTC. Timestamp brackets the I2C read; no subsecond phase is supplied. Add at least1s measurement uncertainty, plus transport/reference uncertainty. Do not infer hardware battery voltage from any field.

`GET /api/state` also includes `hardware.rtc`, `qc.progress`, `qc.rtcEvidence` and each test's `restoredEvidence` flag. GET/sample itself performs no clock writes or automatic test acceptance.

## Common POST context

RTC mutation fields: `token`, `confirm=1`, `sessionId`, `profileRevision`, `clockRevision` must match fresh state. RTC must be enabled in active QC profile. Stale contexts409, missing confirmation400, auth403, unavailable persistence507 (some metadata failures409). Existing serial/radio/network/PD/capture/OTA ownership guards still apply. Inspect response detail, never treat HTTP200 alone as physical acceptance.

### Set time

`POST /api/rtc/time`: common fields plus `utcSeconds` (Unix seconds), `utcOffsetMinutes`(-720..840), `source`(1..64 bytes).

Explicitly review time+zone before calling. Send a fresh reference close to request dispatch; account for latency. Firmware persists unverified intent/new clockRevision before I2C writes, writes calendar/clears EOSC preserving other control bits, reads back, then clears OSF preserving EN32kHz/alarm flags and re-reads. Failure never claims writeVerified. Metadata/readback errors require operator action, never reset unrelated settings. Response `{ok,rtc}`. Setting changes RTC evidence only, not other QC results. Failed attempts also consume clockRevision, so portal must reread after errors.

###60s test / RTC-only retest

- `POST /api/rtc/tick`: common fields. Returns immediately; firmware compares RTC advancement against esp_timer over60s, tolerance2s. Poll for result. Browser may close; reboot interrupts an unfinished test. This is not24h accuracy.
- `POST /api/rtc/reset`: common fields. No active run/tick. Clears only RTC evidence, increments clockRevision, saves checkpoint; leaves actual clock, timezone and other QC results unchanged. Download original report before retest; portal must preserve its own prior run history.

### Bind / abort a portal run

`POST /api/rtc/run`: common fields plus `action=bind|abort`, `runId`(1..64 ASCII alphanumeric/underscore/dash/dot).

Bind requires established UTC and no other RTC run/tick, and successfully saves a durable QC checkpoint before persisting the lock. A binding is not a test PASS. Abort requires same run/session. Portal owns real run duration/sampling/reference.

For an orphan run (e.g. deliberately changed firmware prevented session restoration): `POST /api/rtc/run/release` accepts token,confirm=1,runId, **original** sessionId and clockRevision from externalRunSessionId. May be used before selecting a profile. It releases the lock as aborted; does not erase the stored checkpoint or manufacture results.

### Attach finalized external evidence

`POST /api/rtc/evidence`: common fields plus `report` JSON string(max2200 bytes):

```
{"schema":"QC-RTC-EVIDENCE-1","kind":"accuracy|retention",
 "runId":"portal-run-uuid","operator":"PIC","source":"reference + portal ID",
 "sessionId":"8HEX","clockRevision":1,"elapsedSeconds":86400,
 "driftSeconds":0.2,"uncertaintySeconds":1.2,"toleranceSeconds":3,
 "referenceVerified":true,"physicalPowerRemoved":false}
```

For accuracy: drift is CHANGE of RTC-minus-reference offset since baseline, not absolute initial offset. Require real elapsed>=86400s for PASS consideration. For retention: drift is RTC elapsed minus trustworthy reference elapsed across declared physical power removal; elapsed>=60s and physicalPowerRemoved=true required. Full samples, source quality checks, reboot/gap/clock-change handling remain in immutable portal report. Source/run ID is traceability, not proof against forged input; firmware cannot independently verify portal/reference/physical declarations.

Finite numeric bounds: elapsed60..31536000s, uncertainty1..3600s, tolerance>0..3600s, abs drift<=31536000s. Firmware computes result; caller cannot choose PASS. Poor reference/current calendar/OSF, insufficient duration, uncertainty>=tolerance =>INCOMPLETE. Otherwise abs(drift)+uncertainty<=tolerance=>PASS; abs(drift)-uncertainty>tolerance=>FAIL; overlap=>INCOMPLETE. This cannot certify DS3231±2ppm from1s HTTP data in24h.

One immutable normalized attachment per kind per QC session. Same payload is idempotent; changed attachment409. Use RTC-only reset/new portal run for a retest. Attachment ends/releases that run and forces checkpoint; response `{ok,result,detail}`. If507, portal retains its report and can retry same payload/checkpoint. Accuracy and retention can use different run IDs within one session. For final RTC PASS, both accepted attachments and ticking PASS are required; overall QC still depends on ALL enabled tests.

## Durable progress / reboot / reports

- `POST /api/qc/checkpoint`: token,confirm=1,sessionId. Stops outputs, requires idle capture/PD and selected session. Saves own checksummed single NVS blob with readback (max16KB). One checkpoint per unit, not unlimited report history. Explicit success is required before cutting power. NVS full/unavailable507; never format or erase other data.
- After first save/bind/report, autosave only dirty progress, debounced2s and at most once/minute. Up to60s unflushed changes may be lost on sudden power loss. `qc.progress.dirty/detail/autoSave` exposes this. Portal still archives every observation independently.
- `GET /api/qc/checkpoint`: bounded saved JSON (no auth tokens/passwords). Archive it on server. Do not expose it publicly just because the device LAN API is public lab.
- On reboot, get fresh token and sample. If `qc.progress.available`, before selecting a NEW profile call `POST /api/qc/resume` with token,confirm=1. Requires same serial, exact build identity, profile revision/mask, preset identity and calibration ID. Never automatically merge a changed firmware/profile/calibration. A rejected checkpoint remains downloadable.
- Resume restores same session ID, completed PASS/FAIL evidence and completed input counts. Outputs remain physically commanded OFF; partial sequences/captures/serial jobs restart explicitly. No old worker resumes. Read-current hardware and retained historical evidence are separately labeled; RTC is re-read, OSF still authoritative. Retained tests may be repeated explicitly without resetting others (finalized outputs keep existing policy/new session).
- JSON/CSV reports carry RTC evidence and retained checkpoint evidence plus restoredEvidence flags. QC code changes with new RTC evidence; earlier downloaded reports remain immutable. Portal must NOT edit an old report/CRC to insert PASS.
- Any firmware upgrade during a bound run invalidates exact-build restoration. Abort/archive first. Power-off battery test should use SAME firmware/build and successful checkpoint, then explicit resume. Pure reboot is not a battery test.

## Acceptance boundary

Read CURRENT for compile/API/browser/OTA evidence. Tests shorter than24h cannot be described as an actual24h test. No automatic physical PASS, supply removal, EEPROM write or radio/PD output operation is authorized merely to exercise the integration. Portal and firmware must each report their own deployed SHA/build and pending evidence.
