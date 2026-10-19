# Compact QC 1.17.3: completed 12-hour HTTP observation

Target http://192.168.1.79, serial TMC88F1559CA478, build 2026-10-17T04:23:32Z.
All captured identities match. Logger finished normally and PID26864 is absent.
Start 17 October 2026 11:28:25.236 WIB; finish 23:28:25.261 WIB:
elapsed 12 hours 0.025 seconds. Last sample 23:28:25.234 WIB.
Raw evidence remains in build/network-repair-v1_17_3; old network-overnight retained.

| Logger measurements | Health | State |
|---|---:|---:|
| Requests successful | 4278 | 1426 |
| Failed / timeout | 0 | 0 |
| Median latency, ms | 31 | 125 |
| P95 latency, ms (nearest lower rank) | 94 | 219 |
| Maximum latency, ms | 3141 | 610 |
| Successful responses above 2 seconds | 1 | 0 |

Health cadence 10 seconds plus request time; state every third health sample.
One isolated successful health response took 3141ms at 12:49:35.674 WIB.
No repeated slow responses, counter increase or restart accompanied it; its cause
is unproven. This prevents claiming uniformly fast responses or a guaranteed cure.

RSSI ranged -85 to -56dBm. BSSID 64:A2:8A:E0:CD:90/channel6 stayed unchanged;
disconnects0, gotIPs1, retryRequests0, empty disconnect history. No new restart
was observed; resetReason3 belongs to the deliberate OTA before capture.
Earlier manual power events and OTA/rollback restarts are outside this baseline.
Free heap first-hour median154500 / last-hour median154504 bytes, final154500;
minimum-ever heap91048 to76620 bytes, with no persistent current-heap decline.
HTTP rejectedHeaders remained1, writeTimeouts0; maxHandle644 to996ms,
maxLoop652 to1005ms. The rejection is historical, not a new failure.

Host logger failed once at11:59:21WIB due to Windows summary-file replacement
PermissionError. All185 raw samples survived and were replayed. Replacement
PID26864 resumed12:00:20WIB with original deadline. The raw sampling interval
across recovery is59.276 seconds; this is an observation gap, not measured device
downtime. No further interval exceeded20 seconds. No duplicate logger was started.

Compared with the previous 12-hour1.17.1 capture: health1426/1435 successful,
9timeouts, max4891ms and15successful responses above2s; state183/183 successful,
max5344ms and4above2s. Earlier capture recorded4disconnect events, not proof of
4distinct outages. Current capture has more frequent health/state sampling and
different traffic/environment; this is an observational comparison, not a
controlled causal experiment. Original RF/router cause remains unresolved;
owner knew of no intentional router change. Direct LAN HTTP does not test WAN.

The demonstrated incomplete-header program weakness was repaired and live
regression-tested in1.17.3 before capture. These results support improved HTTP
responsiveness under the observed idle/outputOFF conditions. They do not prove
all original faults share that cause, indefinite stability, physical hardware QC,
or output behavior under load. No hardware PASS is claimed. No additional OTA,
QC, output, radio, RTC, profile, reset, erase or merge was performed for completion.
Final bounded GET-only probe at23:29:35WIB passed2health/3state, health31ms,
state125..141ms, exact identity/build and outputOFF, disconnect0.

Supplementary bounded probes (separate from logger counts):146health and219state,
all successful, no response above2s. Health median32/max312ms;
state median125/max406ms, including the final post-capture probe.
