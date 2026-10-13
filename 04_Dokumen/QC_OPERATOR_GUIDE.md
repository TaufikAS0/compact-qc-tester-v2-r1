# Compact QC operator guide — source1.5.0

Software compilation is separate from physical acceptance. Team uploads using the legacy partition procedure in TEAM_HANDOFF; do not infer installed version from GitHub.

## Start

Select M0 (all populated) or M1 (PD unpopulated/excluded). New session resets results/counters; calibration and EEPROM stay. Check actual firmware + buildUTC in header/API/OLED. Use recovery AP password12345678 if needed; boot STA uses HuaweiJIN. Test-specific WiFi target and network/tag config are RAM only.

## Inputs / OLED / outputs

DI1–4 plus DMM/Normal/Webserver: two debounced complete HIGH–LOW–HIGH cycles automatically PASS. OLED: display text, then user visible/blank verdict. LED and DO are active LOW per latest user correction. Each output starts OFF and has independent timer; two DO can stay ON together. Blue follows firmware command. One Sesuai/Tidak sesuai observation finalizes each output result; ON/OFF remain usable without resetting it. New session repeats QC. Test heartbeat-loss3.5s,60s per-output expiration, Stop and OTA OFF physically. MCP RMW/restoration defers watchdog service inside the bounded transaction, preserving unrelated bits/TPL B7.

## Analog

Keep the working four-point calibration4/10/15/20mA and save gain/offset to qc-adc/cal-v1. Separate QC defaults4/8/12/16/20mA. Set signal, hold stable, Capture16 fresh samples, inspect captured target/ADC/current/error/tolerance and confirm. PASS requires all5 distinct points within captured tolerance and user agreement; any failed point FAIL. New calibration fingerprint invalidates prior QC captures. Reset QC retains active calibration. Use actual calibrator/reference; never feed current directly into GPIO.

## I2C / sensors / EEPROM / PD

Scan lists ACK addresses only. User PASS/FAIL confirms this list, not every IC identity. AHT10 asynchronously resets/E1-initializes, verifies status, waits while busy up to500ms and reads6 bytes. Protocol/data PASS does not prove absolute temperature/RH accuracy. RTC reads BCD time and temperature; OSF/date failure remains explicit.

Read EEPROM shows stored TOP/BOTTOM, CRC/generation plus QC slot classification/raw bytes at0x3F80–0x3FFF. Readable blank/foreign data does not mean a communication failure. Write only this reserved region, preserving foreign bytes and refusing overwrite. Read-valid CRC and write/readback can PASS. If write fails, check WC/address/power and the exact diagnostic; do not erase production EEPROM automatically.

STUSB4500 uses fixed5–20V PDOs/50mV and current10mA steps, not PPS. Adapter must advertise the requested voltage;12V is not available from every adapter. Requests are runtime RAM, not IC NVM. Matching source PDO/RDO/PE ready is protocol evidence, not measured VBUS. MCU attempts bounded Get_Source_Cap if capabilities were missed; measure voltage externally. Absent0x28 on M0 requires population/power/bus check; on M1 choose M1. Source1.2.0 device192.168.1.55 showed no0x28 ACK during read-only diagnosis; software cannot negotiate with an absent/unpowered IC.

## Serial menu

RS232: select mode (MCP2A2 HIGH), wire USB–RS232 adapter TX→deviceRX/RX→deviceTX/commonGND according to terminal markings; use real RS232 voltage levels, not USB–TTL. Run Start-Laptop-QC.ps1 and open localhost8770. Default9600/8N1/100 packets. Firmware and fixture exchange patterned CRC/run/sequence request/response/ACK frames. All rounds verified=>PASS, timeout/corruption/wrong sequence=>FAIL; Stop=>INCOMPLETE. Read COMMUNICATION_QC for full wiring/protocol. This is a functional communication test, not EIA electrical certification.

RS485: MCP2A2 LOW, automatic direction/no DE/RE. Two independent tag forms configure name(max10), baud/parity/slave, functions01/02/03/04, offset0–65535, count1–2, Unsigned/Signed/Float32 and word orderABCD/CDAB. Request saves the selected form then performs a real master read. FC01/02 are bits; FC03/04 are16-bit registers; Float32 needs2 registers. Response CRC/slave/function/length/exception checked; invalid response or timeout FAIL. Confirm each valid result against external device; both user-confirmed tags=>RS485 PASS. Config change/re-request clears that tag's evidence. Do not confuse vendor40001 notation with protocol offset0.

## SPI / network / radio

Peripheral SPI has SD unique temporary write/read/delete and Ethernet controller/PHY checks. W5100 UNKNOWN link is not automatic PASS. Communication/network menu separates actual WiFi/ETH/IP and MQTT from SPI presence. WiFi association+IP or Ethernet DHCP/PHY/IP can PASS their defined scenario. Static EthernetIP alone remains UNVERIFIED until a successful MQTT TCP/broker connection. MQTT3.1.1 TCP verifies accepted CONNACK; socket closes after QC. Broker credentials remain RAM and are excluded from status/exports. Network operations run in a dedicated worker, keeping main-loop TPL service alive; conflicting SPI/routing actions are blocked until result/Stop.

Radio is E70-915NW14S CC1310/15.4 star, not LoRa. Read & validate compares7 supported AT parameters with editable targets. Unsupported LoRa SF/BW/CR/CRC do not PASS. Explicit Apply config sends acknowledged AT parameter writes, then reads settings back. Some settings are persistent and need radio restart to become active; no automatic radio factory reset/restart. Partial write failure can leave prior settings changed; re-read/reapply before use. Restore only M0/M1 bits/direction, preserving TPL. PASS here is UART/settings evidence, not RF range or measured transmit power.

## Export / bench boundary

Each menu shows result badge/reason; summary/JSON/CSV keep the same QC records. CSV retains the Longhi ten-column prefix and Excel quoting/formula protection. QC1 is an evidence CRC, not a signature. Download before reboot/new session. Review negative tests: disconnected sensor/cable, wrong slave/register/CRC, wrong MQTT credentials/broker, Stop mid-test, stale sessions, concurrent controls and power recovery. Physical acceptance for new source remains pending until team installs and performs these tests.

Explicit WiFi QC always reapplies supplied credentials, including the same SSID, so a pre-existing connection cannot mask a wrong-password test. Blank password for approved HuaweiJIN selects the shared lab profile. MQTT password without username is rejected rather than silently testing anonymously. AHT recovery count resets only after a valid measurement, bounding consecutive failures across init and conversion.

MQTT state codes in error detail follow PubSubClient: -4 response timeout, -2 TCP/connect/DNS failure,1 bad protocol,2 client ID rejected,3 broker unavailable,4 bad credentials,5 unauthorized. See [library API](https://pubsubclient.knolleary.net/api).
