# Network responsiveness repair 1.17.2

## Previous

1.17.1/build2026-10-16T12:07:18Z on192.168.1.79 / TMC88F1559CA478.
12h19:11:39WIB16Oct to07:11:39WIB17Oct:1435 health samples,
1426 success/9 timeouts, median31ms/max4891ms; six timeouts after19:40
baseline.183 state GETs successful, median110ms/max5344ms, four>2s.
RSSI-81..-62dBm; four STA disconnect events around04:24..04:25WIB,
channel11->6, recovered. Two earlier POWERON events confirmed manual/power.
No unexplained post-baseline restart. Free heap recovered to~158KB.
Historical logs remain in ignored build/network-overnight, never overwritten.

## Facts and unknowns

Direct GET to private LAN IP does not require internet WAN access. WAN speed
is not established as a cause. HTTP latency and WiFi association are different.
1.17.1 pinned Arduino3.3.10 WebServer enters synchronous readStringUntil on
partial headers. Actual idle-unit reproduction17Oct: one incomplete GET header
made another health GET exceed12s/timeout, with WiFi connected and outputsOFF.
This proves a software weakness, not the exact origin of every overnight timeout.

IDF reason36 is STA_LEAVING, not a measurement of low signal. The old fallback
WiFi.reconnect() disconnects before connecting; with two fallback requests in
the log it may have contributed extra disconnects. Initial loss cause remains
unknown; channel change alone does not prove router restart/roaming. No AP
configuration, WAN, antenna, power or radio hardware change is inferred.
Reference: https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32/api-guides/wifi.html

## Changes

QcWebServer retains stock handlers/Digest/OTA parsing but queues up to4 ingress
sockets. Non-consuming MSG_PEEK checks for complete CRLF headers before invoking
the parser. Normal request (headers+body) bounded4096bytes/deadline1500ms;
incomplete bodies, duplicate/invalid length and chunked requests are not dispatched.
Deliberate multipart OTA and reviewed legacy JSON import retain streaming parsing,
but admission is refused while any output is ON. No other task can start an output
while the main task parses a stream. Expired pending requests never dispatch.
Idle/partial clients no longer
occupy the only handler. One handler per main loop, no concurrent I2C tasks.
Response sends use nonblocking1460byte chunks, bounded1s without progress,
with main-task output timeout/heartbeat OFF and TPL service during backpressure.
Health exposes http.requests/rejectedHeaders/maxHandleMs/writeTimeouts.

Core WiFi AutoReconnect remains enabled.30s fallback now uses esp_wifi_connect
without forced disconnect, and waits30s from last disconnect event. Exposes last
retry result, current BSSID and last8 actual disconnect records (uptime/reason/RSSI).
Counters/history RAM only. Firmware source version1.17.2.

## Unchanged

HuaweiJIN STA + recovery AP, credentials, pin polarity, partitions, OTA auth,
QC criteria/profiles, RTC/radio/EEPROM, calibration and unrelated NVS retained.
Exact explicit checkpoint migration only from1.17.1/build12:07:18Z; all existing
serial/profile/recipe/calibration/clock guards retained. No forced QC PASS/reset.
Current RTC clock validity is separate from restored historical evidence.

## Tests

Node UI,14 Python regressions and cross-compiler11 ingress static assertions PASS
before final build04:07:34Z. Earlier candidates were never uploaded. Pinned firmware compile,
metadata/legacy partitions, source publication, app-only OTA and same-unit runtime
checks pending. Compile-time assertions cover partial/complete headers/bodies,
oversize/duplicate/negative Content-Length, chunked rejection and OTA/legacy stream.
tools/http-ingress-check.py exercises3 incomplete/idle sockets (including an
incomplete unauthorized heartbeat POST that cannot pass authentication),
valid fragmented GET and9 concurrent health GETs with identity/outputOFF guards.
No deliberate WiFi loss/physical output/radio/RTC tests required for this repair.

## Limitations

Admission is cooperative, not a replacement for the stock multipart/legacy-import
parser. Slow OTA/legacy uploads retain existing core limits; authenticated OTA
already stops outputs and services TPL. Slots/headers bounded; this is a LAN QC
server, not an internet-facing general HTTP server. Runtime validation and fresh
long observation are required; do not label a short test as overnight stability.

## Upgrade

Build only Build-Firmware.ps1. Verify serial/build/chip/outputOFF and archive current
state/checkpoint privately before app-only OTA. Preserve historical samples. Do
not reset profile/new-session or transfer evidence from another serial. Version,
actual generated buildUTC/hash and installation results go in CURRENT/handoff.
