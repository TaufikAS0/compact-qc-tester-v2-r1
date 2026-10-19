# AP response repair - 1.18.1

## Previous

1.18.0 build2026-10-19T08:55:24Z installed by app-only OTA via
192.168.1.20 on exact TMC88F1559CA478. Source49133ae pushed before OTA.
Saved Wi-Fi and identical same-app OTA verified persistent configuration;
entire historical checkpoint, profiles, saved recipes, calibration and identity
unchanged. No hardware QC/output ON/radio/RTC tests performed.

## Changes

Increase cooperative HTTP response inactivity allowance from1000ms to5000ms.
On AP4.1 a GET of151789-byte HTML stopped after12822 bytes and incremented
writeTimeouts; the same page via STA1.20 completed in484ms. This proves the
existing send limit can truncate the page in the observed AP conditions.
It does not establish why the network paused. The receive/header bounds and
authentication are unchanged. OFF timers and TPL safety service still execute
on every send iteration, with a scheduler yield. A stalled response remains
bounded by five seconds without progress; this is not an unlimited send wait.

## Unchanged

All1.18.0 Auto QC/Wi-Fi/OLED features, GPIO polarity, classicESP32/16MB legacy
partitions, NVS/checkpoint preservation, profile guards and radio PLAN ONLY.
See UNIFIED_AUTO_QC.md for the complete1.18 feature behavior and limitations.

## Tests

1.18.0 compile/EEPROM five compile-time fault cases, Node UI, Python14,
portal2 and local browser preview passed. Actual1.18.0 AP checks did NOT pass:
one bounded checker overlapped deliberate Wi-Fi apply/reconnection and had
three failures; a separate stable AP HTML fetch was truncated with one server
write timeout, while health/state and STA HTML succeeded. These are retained
as failed evidence, not hidden by later retries.1.18.1 build2026-10-19T09:23:20Z app1611120 SHA256f1b0584e0ce06e91730070b8f3cde679311ebec59f5445b18ccd4d2a2ef19abd MD5055e377b5a2e9147562553595e5fccb3.
1.18.1 compile/partition/frozen72 inputs/Node UI/Python14 PASS; deploy/runtime
acceptance is recorded in CURRENT/VALIDATION after actual verification.

## Limitations

The original router disconnection cause is unknown.1.17.3 reconnected to
HuaweiJIN before any new firmware installed; recovery cannot be attributed to
1.18.0. Longer send allowance trades slower rejection of stalled receivers
for tolerating temporary backpressure. It cannot repair RF/router/laptop
failure. One-device software acceptance does not establish hardware PASS or
long-term network stability. New Wi-Fi password entry requires the owner;
testing retained the existing password for the same configured SSID.

## Upgrade

Only exact CA478 idle/output OFF, app-only OTA after source push and backup.
Retain both1.17.3 rollback and1.18.0 packages/manifests/evidence. No USB port
identified: owner explicitly said COM11 is not this Compact; COM11/COM5 were
not opened. Verify exact build/hash and historical data after restart.
