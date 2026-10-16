# Wi-Fi diagnostics and automatic QC sequence (1.17.1)

User target for this deployment: http://192.168.1.79, serialTMC88F1559CA478.

## Network evidence

Same SSID plus blank password checks the existing association and IP without disconnecting. Explicit password/new SSID still applies credentials and can change IP. This is association/IP QC, not internet throughput acceptance. Recovery AP and public lab profile retained. Driver retries use RAM; manager does not compete with network worker. Wi-Fi power saving disabled to reduce receive latency; this is not proof that RF/power/router faults are fixed.

Header shows STA RSSI in dBm and1..4 bars: >=-55/ -65/ -75 dBm thresholds. STA loss is separate from browser HTTP timeout. Since-boot counters include disconnect reason, IP events and reconnect requests. Reason is the numeric ESP-IDF disconnect code; deliberate credential changes also count. Read-only `/api/health` excludes tokens/passwords and includes unit/build/uptime/heap/reset reason/maxloop and Wi-Fi telemetry. Maxloop includes request handling; a long loop is not proof of RF disconnection. Counters reset on reboot, no periodic NVS writes.

Idle browser no longer posts heartbeat each second. Active outputs/sequence still receive heartbeat every second; firmware3.5s expiry and maximum ON remain mandatory.

## Operator flow

Open Auto sequence QC -> review pending safe tests -> confirm. Final PASS/FAIL and disabled profile tests are skipped. Firmware determines AHT10, SD temporary-file roundtrip, Ethernet controller/link, current Wi-Fi SSID/IP, Ethernet DHCP and optional configured MQTT CONNACK outcomes. Unknown results/request errors are not fabricated hardware FAIL. Stop/context change/tab hidden/disconnect cancels; no automatic resume. Reopen only after reviewing state.

Radio and RTC are opt-in automatic steps after global reviewed approval: radio reads all seven parameters, waits at least2.5s after GET before AT reentry, resends only the actual current channel, requires ACK and matched GET; no draft/default target. RTC challenge/restoration uses the reviewed laptop time zone and checks clock revision before writing. Neither is armed by default; final results are still skipped. EEPROM identity, PD, analog reference/Modbus expected values and external serial fixtures use their existing reviewed workflows. DI/switches automatically evaluate two real debounced cycles after physical stimulus. OLED/I2C observation is human. These cannot all honestly PASS unattended.

LED & Output -> fixture safe checkbox on Kesiapan -> choose Otomatis + konfirmasi popup -> review -> confirm. Only one channel is commanded ON. Popup requires observed ON, then automatic OFF and observed OFF; only both observations finalize. Then next channel. ON maximum30s, firmware accepts only bounded1..60s sequence commands with session/profile context; manual preset1..10s unchanged. Firmware3.5s heartbeat still applies. Any FAIL stops the sequence. Timeout, Stop, changed context, hidden tab, lost link or another active channel prevents advancement. Opening page/refresh never starts outputs. Existing completed output results skipped. Manual controls disabled during sequence; global Stop accessible in popup.

Correction: stop -> Reset QC on the mistaken channel -> review/confirm -> retest ON/OFF. No direct forced PASS and no clearing unrelated results. Profiles/presets/calibration/pin polarity unchanged. DO GPIO32/33 active HIGH, MCP LEDs active LOW.

## Overnight observation

`tools/device-monitor.py --url http://192.168.1.79 --serial TMC88F1559CA478 --hours 12 --interval 30 --out build/network-overnight`

Only HTTP GET; no output/test/clock mutations. Bounded12h default, maximum24h, unit identity checked. Whitelisted health records and summary only (no control token/credentials). Before deployment health404 falls back to state and discards private fields. Laptop must stay awake/on same network. A timeout alone cannot identify device versus laptop/router failure. Review RSSI, disconnect counters/reason, uptime/reset/heap and HTTP latency together. `rebootsObserved` uses uptime decrease (millis rollover also possible after49days). Logs ignored under build/. Follow-up scheduler reads logs and alerts only meaningful changes, not every unchanged sample.

Compile/software and simulated UI tests do not establish overnight stability or physical LED/relay PASS.
