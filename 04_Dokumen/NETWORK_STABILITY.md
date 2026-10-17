# Network responsiveness repair1.17.3

Host observation correction17Oct12:00WIB: PID24168 exited11:59:21WIB when
Windows denied atomic summary.tmp replacement. All185 raw samples survived;
summary was one sample behind. Device bounded five-GET probe still succeeded
(31..141ms), disconnect0/restart0. Host logger now retries transient rename locks,
retains pending snapshot on persistent lock, and supports explicit --resume by
replaying raw evidence with original deadline. Verified lock/replay tests PASS.
Single replacement PID26864 resumed12:00:20WIB with original23:28:25WIB end;
about59s observation gap is not device downtime. Backups/operator-events retained,
same automation updated. Firmware/build/checkpoint unchanged; no OTA required.
Later PID24168 references below describe the initial launch, not current ownership.

## Previous

Installed baseline1.17.1/build2026-10-16T12:07:18Z on192.168.1.79,
serialTMC88F1559CA478. Old12h19:11:39WIB16Oct to07:11:39WIB17Oct:
1435health samples/1426success/9timeouts (6 after19:40baseline), median31ms,
max4891ms.183stateGETs successful, median110ms/max5344ms,4over2s.
RSSI-81..-62dBm,4disconnect events around04:24..04:25/channel11->6.
Two earlyPOWERON events confirmed manual/power; none new afterbaseline.
These counters are events, not proof of four separate RF outages. Old logs
remain under ignored build/network-overnight and are never overwritten.

Actual idle/outputOFF1.17.1 reproduction: incomplete GET header blocked another
health GET beyond12s/timeout with STA connected. This proves a program weakness,
not the exact cause of every historical timeout. Direct LAN-IP HTTP does not
require WAN internet. Initial RF/router loss remains unknown. Reason36 is
STA_LEAVING, not a measurement of signal quality. Forced WiFi.reconnect fallback
may have added disconnects; channel change alone proves neither roaming nor
router restart. Reference: https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32/api-guides/wifi.html

Intermediate1.17.2 compiled/OTA successfully but FAILED actual valid fragmented
GET acceptance (reset/no response). It is REJECTED. Immediately rolled back the
retained exact1.17.1 app/hash/MD5, checkpoint unchanged. No active operator QC was
interrupted.1.17.3 replaces first-segment MSG_PEEK with cooperative header replay.

## Changes

QcWebServer queues4bounded clients, incrementally consumes headers across TCP
segments, then replays them through virtual NetworkClient reads to the PINNED
3.3.10 stock parser. Normal header+body total<=4096bytes/deadline1500ms; entire
normal body must be available before dispatch. Invalid/duplicate length and
chunked requests are rejected. Expired requests never dispatch afterward.
Multipart OTA and reviewed legacy JSON import retain stock streaming parsing,
admitted only with outputsOFF. All handlers remain on main task. This adapter
does not implement SSE/middleware; none is configured in this firmware.

Response sends use nonblocking1460byte chunks, bounded1s without progress, while
servicing main-task OFF timers/heartbeat and TPL. Health exposes HTTP attempts,
admission rejections, max handler time and write timeouts. Names are diagnostic,
not QC acceptance counters. Core AutoReconnect remains enabled;30s fallback uses
esp_wifi_connect without disconnecting an association already in flight, and
waits30s since last disconnect. WiFi diagnostics add retry result, BSSID and last8
disconnect records (uptime/reason/RSSI), RAM only.

## Unchanged

HuaweiJIN STA + recovery AP, approved credentials, output polarity/pins, partitions,
OTA Digest/nonce protection, QC criteria/profiles/recipes, EEPROM/RTC/radio,
calibration and unrelated NVS. DO32/33 HIGH, MCP LED LOW; TPL B7 preserved.
No radio/RTC writes, outputON, QC reset, forcedPASS, merge or erase this task.

## Tests

Final1.17.3/build2026-10-17T04:23:32Z, app1582800bytes,
SHA256bc6b0b4f652482319d1813e5ddd68644f3733696bebca98ccb884ffa5ac6996c,
MD5919879256bbd1156f9f3d836f91058d9. Sketch1582645/2097152, staticRAM58096.
Pinned Build-Firmware compile, metadata and legacy partition gates PASS.
Node UI,14existing Python regressions,2portal tests,13ingress static assertions
and clean-source paired artifact packaging PASS. No firmware edits after build
start. Source7fdb5d3 pushed BEFORE actual app-only OTA.

Live1.17.3 ingress regression PASS on SAME unit/outputOFF:3incomplete/idle clients
(one incomplete invalid-token heartbeat body) allowed competing health47ms and
state140ms; valid fragmented GET OK, fragmented POST completed/auth403 correctly;
9concurrent healthGETs31..78ms. Deliberate unread large-page client bounded:
competing health984ms, handler1039ms/loop1041ms, no STA disconnect/restart.
Intentional stimulus caused reject4/writeTimeout1; it is NOT an unexplained fault.
Normal valid-token heartbeat POST200 also verified with outputsOFF, no QC change.

Uploaded IDENTICAL1.17.3 app again through NEW parser/Digest/multipart OTA HTTP200,
then verified same unit/version/build/exactMD5 again. Profiles/calibration/identity
and ENTIRE saved checkpoint EB4CDCB2 unchanged after both uploads. Runtime remains
UNSELECTED, no automatic restoration or test restart. Actual browser header/build,
RSSI, sidebar navigation and1.17.3 checkpoint-review modal verified; Batal only.
Post-final-OTA bounded check3/3state94..125ms, no timeout/disconnect. These are
momentary software/HTTP results, not overnight stability or hardwareQC PASS.

## Limitations

Initial RF/router loss still unknown. Stock multipart/legacy stream parser retains
its core body timeouts; streaming is refused while outputON. TPL service during
actual flash is retained. Normal requests over4096 are rejected, response size is
not capped by this ingress limit. Protected parser adapter is pinned to3.3.10;
revalidate on any core change. Physical output/watchdog timing was NOT measured.
Restored historical RTC evidence never proves current OSF/time/battery validity.

## Upgrade

Build only Build-Firmware.ps1, app-only OTA only. Exact explicit checkpoint bridge
only from1.17.1/build2026-10-16T12:07:18Z, same serial/profile/recipe/calibration/
clock-revision guards. User can choose Tinjau pemulihan sesi then confirm; new
session would discard RAM results, so do not use it merely to resume history.
Version/time/hash and previous/rejected candidates remain recorded in worklog.

## Follow-up observation

Existing automation pantau-compact-qc-semalaman UPDATED (not duplicated), named
Pantau perbaikan Compact QC, ACTIVE10min quiet unless meaningful change.
Single logger PID24168 started17Oct2026 11:28:25WIB,12h to~23:28:25WIB today,
health10s/stateevery3samples(~30s). Logs build/network-repair-v1_17_3;
baseline1.17.3/disconnect0/gotIPs1/retry0/channel6/reject1/writeTimeout0,
maxHandle644ms/maxLoop652ms/freeHeap~154KB/min~91KB. Historical reject1 at baseline
is not a new fault. Stress counters above were reset by deliberate final same-app
OTA before this baseline. Old logger14988 finished normally; no duplicate logger.

Compare health AND state latency/errors, uptime, RSSI/BSSID/channel, WiFi events/
reason/retries, HTTP diagnostics and heap before attributing faults. Routine
monitoring GET-only. At end report actual counts/duration/limits then pause same
automation; if a proven program regression repeats, owner authorized repair/OTA
under AGENTS gates and only idle/same unit/outputOFF. Never fabricate RF/physical
acceptance or hide timeout with forcedPASS. Laptop awake/onLAN and unitON required.
