Current1.13.1 Warranty FAIL export: RTC semantics unchanged; current approved checkpoint upgrade ONLY exact1.13.0/build11:18:57Z->1.13.1 same unit/no active run/clock change. See CURRENT/WARRANTY_CSV.md; earlier exceptions below historical.

Current1.13.0 module reset: same checked RTC evidence-only reset and active-run/tick protection, exposed also via /api/qc/reset. Current approved upgrade exact1.12.0/build08:59:41Z->1.13.0 same unit; historical exceptions below are not a general bypass. Read MODULE_RESET.md/CURRENT.

Current1.12.0 exports/PCB revision: RTC quick/sampling semantics unchanged. Current approved upgrade exception is exact1.11.0/build08:16:55Z ->1.12.0, documented in WARRANTY_CSV.md; earlier1.10.6->1.11.0 text below is historical and is not a general cross-build bypass. New owner target192.168.1.44 has different serial; never transfer previous unit evidence.

# Compact RTC + durable QC integration (COMPACT-RTC-1)

Firmware owner: this Compact repository. Portal owner: separate Telemetric Hardware Portal repository. No portal source changes here. Source1.11.0; **check CURRENT/VALIDATION for exact installed build**, never infer deployment from this document.

## Invariants

- DS3231 at0x68. Existing SDA21/SCL22, outputs, WiFi, partitions and other NVS namespaces unchanged. New namespaces qc-rtc/meta-v1 and qc-progress/session-v1 only. No EEPROM identity writes or NVS erase/format.
- RTC POST bodies are application/x-www-form-urlencoded. The one-time legacy bridge also accepts multipart text fields; use that with a minimal original-evidence projection to avoid large URL-decoding allocations. Existing device token from GET/api/state remains required. This intentional lab token model is not a cryptographic signature or Internet authentication.
- UTC internally. RTC stores a local calendar; offset must be explicitly supplied. Calendar2000..2099 only. Firmware does not contact NTP; portal owns independent reference quality and network/quantization uncertainty.
- Owner decision2026-10-15/source1.11.0: production RTC may PASS a real deliberate Write/Readback challenge AND verified restoration to the reviewed time. OSF-clean/read-only/time-sync alone never grants quick PASS.60s/daily accuracy/retention are separate sampling evidence; quick PASS does not imply those passed. Calendar/transport/OSF faults and real sampling FAIL still block/override production PASS. Retention is operator-declared physical power removal, not inferred from reboot. Earlier1.10.6 used the strict combined sampling criterion; do not infer identical PASS scope across versions.
- A portal run binds the unit's current session/clockRevision, locks time setting and deliberate new-session/profile/recipe changes, and persists that lock across reboot. Finish or explicitly abort/release first. Never auto-sync RTC during a drift run.

## Read

`GET /api/rtc/sample` (Cache-Control:no-store) returns:

```
{serial,sessionId,profile,profileRevision,version,buildUTC,
 rtc:{schema:"COMPACT-RTC-1",bootId,sampleUptimeUs,sampleCompletedUptimeUs,
 snapshotResponseUptimeUs,sampleAgeUpperBoundMs,
 resolutionMs:1000,communicationValid,valid,timeValid,oscillatorStopped,
 time,rtcLocalEpochSeconds,utcSeconds,utcOffsetMinutes,timezoneConfigured,
 clockRevision,metadataReady,writeVerified,lastSetUtcSeconds,
 temperatureC,runningTest,runningTestPassed,runningElapsedSeconds,
 externalRunId,externalRunSessionId,externalEvidence,detail}}
```

Missing/invalid fields must not be coerced into time0 or valid measurements. `utcSeconds` exists only after explicit successful setting with persisted timezone metadata. `timeValid` means valid calendar/OSF clear, NOT independently correct absolute time. Uptime is monotonic microseconds within `bootId`, unrelated to UTC. Timestamp brackets the I2C read; no subsecond phase is supplied. Add at least1s measurement uncertainty, plus transport/reference uncertainty. Do not infer hardware battery voltage from any field.

`snapshotResponseUptimeUs` is sampled immediately before JSON serialization for GET/sample and GET/state. `sampleAgeUpperBoundMs` is rounded-up age from read START to that instant; it excludes subsequent JSON serialization/network transfer. Portal adds the full measured HTTP round-trip budget (which includes server processing/serialization) conservatively, plus reference uncertainty and at least1000ms quantization. A high RTT/gap invalidates precision claims, not permission to assume a cached sample is current. This is an acquisition-age bound under normal firmware execution, not an oscillator accuracy certificate.

`GET /api/state` also includes `hardware.rtc`, `qc.progress`, `qc.rtcEvidence` and each test's `restoredEvidence` flag. GET/sample itself performs no clock writes or automatic test acceptance.

From1.11.0, RTC includes `productionCriterion:"WRITE_READBACK"`, `quick` and `samplingComplete`. Quick evidence schema `QC-RTC-QUICK-1` stores scope, result, sessionId, clockRevision, challengeUtcSeconds/challengeReadbackUtcSeconds, challengeVerified, restorationVerified, requestedUtcSeconds/readbackUtcSeconds, offset/source and completedUptimeMs. Null/absent quick means not tested; metadata writeVerified alone is not quick PASS. Quick evidence is also in qc.rtcEvidence and CSV rtcEvidence.json. `samplingComplete` still requires60s ticking plus accepted accuracy AND retention PASS.

## Common POST context

RTC mutation fields: `token`, `confirm=1`, `sessionId`, `profileRevision`, `clockRevision` must match fresh state. RTC must be enabled in active QC profile. Stale contexts409, missing confirmation400, auth403, unavailable persistence507 (some metadata failures409). Existing serial/radio/network/PD/capture/OTA ownership guards still apply. Inspect response detail, never treat HTTP200 alone as physical acceptance.

### Set time

`POST /api/rtc/time`: common fields plus `utcSeconds` (Unix seconds), `utcOffsetMinutes`(-720..840), `source`(1..64 bytes).

Optional `qcQuick=1` explicitly requests production quick QC. Any other supplied qcQuick value400. Firmware selects a calendar challenge120/240s away from the requested time and different from the previous actual read, writes and verifies it, then ALWAYS attempts to restore the requested real time and verifies again. Both readbacks must be within0..1s, calendar valid and final OSF clear. Old RTC evidence is invalidated/new revision consumed; other tests unchanged. If the challenge fails but restoration succeeds, quick still FAIL; do not coerce a successful final write into PASS. Failure/power loss can leave an untrusted/test time; metadata intent is persisted before writes and operator must retry explicitly. No writes while any bound run or capture is active. Success immediately saves/readbacks the QC checkpoint;507 means not durably saved, even if hardware write was verified. No60s wait is required for this production criterion. Normal time setting without qcQuick does not create quick evidence.

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

One immutable normalized attachment per kind per QC session. Same payload is idempotent; changed attachment409. Use RTC-only reset/new portal run for a retest. Attachment ends/releases that run and forces checkpoint; response `{ok,result,detail}`. If507, portal retains its report and can retry same payload/checkpoint. Accuracy and retention can use different run IDs within one session. Both accepted attachments and ticking PASS are required for samplingComplete. From1.11.0 real quick Write/Readback may independently satisfy production RTC; overall QC still depends on ALL enabled tests and actual sampling failures override quick PASS.

## Durable progress / reboot / reports

- `POST /api/qc/checkpoint`: token,confirm=1,sessionId. Stops outputs, requires idle capture/PD and selected session. Saves own checksummed single NVS blob with readback (logical JSON max16KB; dictionary+MessagePack max9KB, to respect the existing32KB NVS partition). No partition resize. One checkpoint per unit, not unlimited report history. Explicit success is required before cutting power. NVS full/unavailable507; never format or erase other data.
- After first save/bind/report, autosave only dirty progress, debounced2s and at most once/minute. Up to60s unflushed changes may be lost on sudden power loss. `qc.progress.dirty/detail/autoSave` exposes this. Portal still archives every observation independently.
- `GET /api/qc/checkpoint`: bounded saved JSON (no auth tokens/passwords). Archive it on server. Do not expose it publicly just because the device LAN API is public lab.
- On reboot, get fresh token and sample. If `qc.progress.available`, before selecting a NEW profile call `POST /api/qc/resume` with token,confirm=1. Requires same serial, exact build identity, profile revision/mask, preset identity and calibration ID. Never automatically merge a changed firmware/profile/calibration. A rejected checkpoint remains downloadable.
- Resume restores same session ID, completed PASS/FAIL evidence and completed input counts. Outputs remain physically commanded OFF; partial sequences/captures/serial jobs restart explicitly. No old worker resumes. Read-current hardware and retained historical evidence are separately labeled; RTC is re-read, OSF still authoritative. Retained tests may be repeated explicitly without resetting others (finalized outputs keep existing policy/new session).
- JSON/CSV reports carry RTC evidence and retained checkpoint evidence plus restoredEvidence flags. QC code changes with new RTC evidence; earlier downloaded reports remain immutable. Portal must NOT edit an old report/CRC to insert PASS.
- Any firmware upgrade during a bound run invalidates exact-build restoration. Abort/archive first. Power-off battery test should use SAME firmware/build and successful checkpoint, then explicit resume. Pure reboot is not a battery test.

Only approved cross-build exception: source1.11.0 `POST /api/qc/resume` may explicitly include `upgradeFromBuild=QC_BUILD=1.10.6|2026-10-15T06:57:45Z`. Requires the NVS checkpoint to match that EXACT build, same serial/calibration/profile/preset/clock revision and no bound run. Source build provenance is retained under checkpointEvidence.upgradeFromBuild, new header is saved/read back before restoring. It never creates quick evidence or changes the RTC. Missing/wrong source retains strict rejection; arbitrary edited JSON imports are not accepted by this route. Ordinary1.11.0 reboot resumes same build without the parameter. Archive original progress before this deliberate upgrade.

### One-time legacy1.9.1 bridge

The real owner had33 PASS in RAM on1.9.1/sessionEB4CDCB2 before this feature. The original sanitized GET/state+CSV were archived BEFORE upgrade. `POST /api/qc/import-legacy` (token,confirm=1,snapshot=<original JSON>, parser limit50KB, actual heap/transport capacity lower) is allowed only while unselected and no checkpoint exists. A failed migration may be retried over its own checkpoint only if original session/operator/source QC code and every non-RTC result still match, clockRevision remains0, and no RTC tick/external evidence/run exists. It verifies original1.9.1 QC1 CRC, serial, profile/preset revisions, calibration coefficients within original JSON rounding precision and exact captured calibration IDs. It stores a bounded checkpoint with original build/QCcode provenance and explicitly resumes completed evidence, never RTC acceptance or ON state. This is a narrow migration bridge, not a general user-edited PASS import; CRC is integrity, not authentication. Operator field stays empty if the original was empty. Downloaded original reports are not rewritten. Ordinary cross-build resume remains forbidden. Do not call this route with simulated or reconstructed physical results.

### Portal compatibility handoff

Read-only portal docs observed2026-10-15: `docs/RTC_FIRMWARE_CONTRACT.md` still describes1.9.1 and proposes future fields/routes. Its existing GET/state adapter remains compatible with the retained identity/RTC fields and uptime. This COMPACT-RTC-1 document is the implemented firmware contract for new features; proposed portal field/route spellings are not silently treated as supported. Portal owner should map the microsecond timing fields, UTC/offset/boot/revision guards and form-encoded routes explicitly, keep its immutable reports/segments and distinguish observed tests from deployments. No portal repository edits by this firmware task.

## Acceptance boundary

Read CURRENT for compile/API/browser/OTA evidence. Tests shorter than24h cannot be described as an actual24h test. No automatic physical PASS, supply removal, EEPROM write or radio/PD output operation is authorized merely to exercise the integration. Portal and firmware must each report their own deployed SHA/build and pending evidence.

## Installed firmware acceptance2026-10-15

1.10.6/build06:57:45Z installed by app-only OTA, runtime hash verified. Real set/readback/WIB,60s ticking, negative auth/context/bounds, run guards/abort, checkpoint/header/readback and two same-binary software reboot/resume cycles passed. An administrative bound run survived reboot and was aborted cleanly. Original33 historical PASS and82 analog/Modbus CSV columns remained exact. Live UI shows running-test LULUS while accuracy/retention remain pending. These do not prove24h accuracy, reference certification, physical power removal or battery health. No external positive evidence was fabricated or submitted. Portal still needs owner integration against this contract.

For the legacy bridge, project only original root version/buildUTC/buildIdentity; hardware serial/calibration/currentQc; qc variant/profileSelected/sessionId/profileRevision/profileMask/operator/overall/qcCode/recipe/tests/outputs/inputs/modbus/network/serial/radio. Preserve their values and every CRC-covered test unchanged. Multipart projection11420 bytes was verified; full old encoded report failed under ESP32 memory pressure. Keep the immutable full original archive independently. Do not retry legacy now: installed clockRevision1 contains new RTC evidence.

## Installed quick-QC acceptance2026-10-15

1.11.0/build08:16:55Z installed and runtime hash verified. Actual Operator button/review/confirmation executed a different-time challenge, readback, real-time restoration/readback in WIB. Receipt revision2/quickPASS persisted through an identical-binary software reboot+explicit resume.34 production tests PASS; original33 results and82 analog/Modbus CSV fields remain exact. Report quick receipt matches saved NVS checkpoint. SamplingComplete false; no sampling results or reference/battery acceptance invented. Auth/context/bounds/run-lock negative cases reject mutations; source1.10.6 compatible migration retains historical provenance. Read CURRENT/VALIDATION for exact artifacts. Quick test may temporarily alter RTC by120/240s; never run during a portal observation. Save progress again after live state re-evaluation if dirty is shown.
