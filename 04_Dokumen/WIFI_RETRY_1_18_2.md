# Remembered Wi-Fi retry - 1.18.2

## Previous
1.18.1 build2026-10-19T09:23:20Z. Recovery AP stays active while STA retries
every30seconds, in addition to Arduino core automatic reconnect.

## Changes
When disconnected, the module retries its configured remembered SSID every
300000ms (five minutes). Disable core automatic reconnect so the application
owns retry timing. Initial boot and explicitly saved settings still connect
immediately; a newly observed connection loss starts a five-minute interval.
Each attempt uses esp_wifi_connect without forcing disconnect or stopping AP.
Network QC and pending settings defer the scheduled attempt; no catch-up burst.
Health/state adds retryIntervalMs and retryInMs; retry result remains exposed.
This reconnects only the configured network, not a scan of arbitrary networks.

## Unchanged
Recovery AP credentials/IP, saved Wi-Fi blob, profiles, results, calibration,
outputs, OTA partitions and all1.18.0 QC features. No password changes, NVS
erase, radio testing or hardware QC triggered by this revision.

## Tests
Compile-time timing cases cover before deadline, exact deadline, connected,
busy, pending settings and millis rollover. Build/software and installed
acceptance are recorded in CURRENT after actual verification.

## Limitations
One connection attempt can contain driver association/authentication exchanges;
five minutes controls application attempts, not each RF packet. Busy network
QC can delay a retry. This policy does not prove why HuaweiJIN was unavailable
or guarantee that credentials/router/RF will permit association. A live
five-minute cadence requires a disconnected-device observation across deadlines.

## Upgrade
Build-Firmware.ps1 only; source push before app-only OTA. Verify exact CA478,
old build/hash and idle/output OFF, backup current state/checkpoint before OTA,
then verify new build/hash and preserved data. No unidentified USB ports.

## Actual software validation
1.18.2 build2026-10-20T01:23:08Z app1611376bytes SHA2565b7341f5836af1a508cfd7073a3146d012f1bd1031a77ebb948e04fed3332fb1 MD5de3e52c8f29904d6800169f03832703a.
Build/partition/frozen73 compiler inputs/six timing assertions/Node UI PASS.
No OTA:20Oct4.1 returned404 for both identity/status APIs. Current address
and exactCA478 identity required before upload. Physical five-minute retry
and AP continuity remain untested on this release.
