# Team handoff — v1.0.0

Source baseline v0.5.0 plus integrated remote v0.5.1 packaging changes to v1.0.0. Codex prepares/builds/tests/pushes; **team uploads hardware**. Exact build UTC/hash and publication evidence are recorded in [VALIDATION](../VALIDATION.md). Read [README](../README.md) and [CHANGELOG](../CHANGELOG.md) for API changes. User confirmed final polarity: DO1/DO2 active LOW, LED MCP active HIGH. Source output/MCP profile flags are enabled; runtime I2C faults still lock commands.

## Build and install

1. Verify actual port, classic ESP32 chip, MAC/16MB and current partition table. Last historical port was COM13 CH340; do not assume current port. Back up NVS/partition table. No Codex upload of v1.0.0 is performed.
2. Check `config.h`: LED MCP1 active HIGH, DO GPIO32/33 active LOW, as corrected by user. Both output/MCP verification flags are true; BANK0/SEQOP0 is checked at runtime. Use a suitable isolated bench fixture and verify actual OFF before attaching loads.
3. Install pinned dependencies from README, Python3 and Node. Run `./Build-Firmware.ps1`; this generates UTC identity before compile and validates it in binary. Without a private override, use the intentional public LAB default admin / CompactLab050! from lab_ota_defaults.h (owner decision 2026-10-12). Private override builds stay private; -PublicLabRelease refuses them and packages only committed source.
4. Keep app and `build/firmware-manifest.json` together internally. Confirm manifest version/buildUTC/SHA-256 matches the exact `.ino.bin` to install. Build timestamp is build start, not device clock/upload time.
5. For initial USB installation use verified legacy offsets below; never Arduino default offsets; validated portal merged is allowed for deliberate LAB reset. For an installed OTA-capable firmware, `/update` uses application `.ino.bin` only.

| Partition | Offset | Size |
| --- | --- | --- |
| nvs | 0x9000 | 0x8000 |
| otadata | 0x17000 | 0x2000 |
| phy_init | 0x19000 | 0x1000 |
| factory | 0x20000 | 0x200000 |
| ota_0 | 0x220000 | 0x200000 |
| ota_1 | 0x420000 | 0x200000 |
| storage | 0x620000 | 0xF0000 |

USB component offsets: bootloader **0x1000**, partitions **0x8000**, application **0x20000**. If boot selection must be reset to factory, erase **only** otadata0x17000/0x2000. Preserve NVS/storage. Verify write hashes. Do not write Arduino boot_app0.bin at0xE000. Validated portal merged at offset0 is an alternative for deliberate LAB reset: even fast write resets NVS/settings; Full erase + write additionally deletes storage. Build this package with -PublicLabRelease in a clean checkout without a private override. Component USB is the data-preserving option. These instructions depend on matching the device's current layout first.

## Bench procedure

- On boot verify serial `TMC` +12 uppercase STA MAC hex digits, source v1.0.0 and matching buildUTC on serial, `/api/state`, dashboard and rotating OLED screen. Wi-Fi STA HuaweiJIN; recovery AP`COMPACT-QC-xxxx`, password12345678 at192.168.4.1. Existing driver STA profile is updated only if different; unrelated NVS preserved.
- New Session resets RAM results/input counters and capture points, not active calibration or EEPROM. Operator name optional. Browser reload preserves firmware counters/results; device reboot resets RAM. Download report before reset/reboot. No input operator confirmation exists.
- DI1–4 and each DMM/Normal/Webserver: provide `HIGH LOW HIGH LOW HIGH`, holding each state≥50ms. Verify counter0→1→2 and latched automatic PASS; stuck HIGH/LOW/bounce must not pass. Normal is GPIO2 LOW AND GPIO39 LOW; DMM/WS HIGH contacts confirmed. GPIO39 needs external bias.
- OLED: inspect actual text and choose Teks terlihat or Blank. Verify every module status rotates, including OLED's own result and build UTC. Visual PASS rejected if initialization failed.
- LEDs: each of11 rows ON→observe→Menyala, OFF→observe→Mati. DO1 and DO2 separate containers use same flow. Requested ON confirmation marks visual PASS; OFF confirmation strengthens record. Wrong-state/unattempted click rejected. Check one output ON, explicit stop,60s bound,3.5s disconnected heartbeat, OTA stop. Verify physical OFF, especially I2C failure. Firmware cannot prove contact/LED state without user observation.
- ADS1115 confirmed, AIN0 conditioning/VDD still must be verified. Never feed current directly into GPIO. Before calibration conversion100Ω/gain1/offset0 is provisional. At4/10/15/20mA (or edited references), capture16 new stable readings. Firmware rejects samples≥3.3V, invalid/stale/saturated and SD>30count. Save fit with max residual(default.10mA), R²≥.995, distinct points and broad span. Confirm gain/offset/CRC persist reboot. Reset points keeps active calibration. Test independent currents such as7/12/18mA with a reference and compare accuracy; do not treat fitted residual as independent QC accuracy. Interrupted NVS write must retain a valid previous/new blob, never affect production namespaces. Calibration lives in `qc-adc/cal-v1`, not PCB EEPROM. ADC/OLED acquisition pauses for up to5s while PD negotiation is prioritized; capture and PD request cannot overlap.
- I2C: Scan lists ACK addresses0x08–0x77 only. No scan-triggered protocol/measurement/init. Expected labels are pinlist guesses; unknown/ATtiny gets no invented command. AHT10 automatically initializes, six-byte data, no AHT20 CRC; separate temp/RH card, compare reference. DS3231 clock/temp read-only, OSF warning; verify ticks/date/12h/century and battery retention physically. Model confirmed by user, not proved by address.
- EEPROM: Write TOP`TMC_T_V2_R1` /BOTTOM`TMC_B_V3_R0`, click Read and check stored labels/generation/CRC, then reboot/read. Failed reads clear cached labels. Reserve final128bytes0x3F80–0x3FFF; foreign data rejected. Test WC, missing chip and interrupted inactive-slot write with previous valid record preserved. No manufacturing bytes elsewhere are altered.
- PD: use adapter that offers desired **fixed PDO**, rated cable/load/voltmeter. Request5/9/12/15/20V where supported. Check sink active count1/2, PDO1=5V, PDO2/3=target (PDO3 inactive); source capabilities, source RDO position and PE ready. Soft reset is queued in main loop after HTTP response to reduce lost Source_Capabilities. ADC/OLED work pauses while negotiating; TPL service continues. If no source message captured, UNVERIFIED is correct; retry, do not infer voltage from sink index. Physically measure VBUS and test unsupported voltage/current fallback. Reproduce reported12V→20V behavior. The root cause was not proved by source inspection; patch prevents stale PDO3 selection and adds evidence, but still needs hardware validation. IC reset/power loss reloads NVM defaults; runtime requests do not permanently provision NVM.
- Ethernet: SPI controller/PHY link with known cable, static documentation test IP192.0.2.2, no DHCP/packet acceptance. W5100 UNKNOWN link needs separate bench verification. Reset MCP2A1 untouched(unknown polarity). SD unique temporary write/read/delete + shared CS tests; no production file overwrite.
- RS485/RS232 external tests remain deferred. Modbus config stored namespace`qc-settings`; apply route selects MCP2A2 HIGH RS232/LOW RS485. No UART data or comms PASS. Reapply routing after reboot.
- Measure TPL5010 DONE MCP1B7 waveform/interval/high width under load/OTA; force main-loop failure and observe hardware reset. Software pulse count alone is insufficient. Keep MCP B3–B7 preserved by LED operations.
- OTA `/update`: Digest admin + public LAB default or private build override, one-use nonce2min, inactive2MB slot. Test wrong image, expired nonce, dropped/slow upload, power interruption, successful reboot/version/buildUTC; old app survives rejected/incomplete update. Watchdog/flash timing requires bench test. No bootloader/partition change via app OTA.

## Evidence boundaries

Confirmed by user: pinmap, MCP23017-E/ML, STUSB4500QTR, M24128/C7559, AHT10, ADS1115, DS3231 Normal logic and corrected output polarity. Unknown: conditioning transfer, physical fixture acceptance, Ethernet reset circuit, ATtiny protocol, actual PD source PDO availability. Software regression/compile/browser tests do not substitute for waveform/current/voltage/physical upload acceptance. Overall intentionally stays INCOMPLETE while deferred/data-only modules lack applicable acceptance evidence.
