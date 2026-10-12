# Communication QC v1.2.0

## Laptop workflow

Run `./Start-Laptop-QC.ps1`, then open http://127.0.0.1:8770/. First launch installs pySerial3.5 in tools/laptop-qc/.venv. Keep the console running for repeated devices; Ctrl+C stops backend. No adapter is opened on startup. Choose M0/M1 on ESP dashboard first; laptop never automatically resets QC session. Team installs firmware1.2.0 before actual testing.

Connect genuine USB-RS232 to RS232 terminals, or USB-RS485 to A/B and reference using verified bench wiring. Programming USB/TTL bypasses transceiver and is not valid acceptance. Do not feed RS232 voltage into GPIO. Select adapter COM, device LAN/AP URL, transport, baud(default9600),100packets,2000ms,8N1/no flow control. At1200baud use>=3000ms. Firmware bounds10-1000packets/500-10000ms. User confirmed RS485 automatic direction, no DE/RE; adapter must support automatic turnaround too. Disable local adapter echo.

Start opens adapter before arming `/api/serial/start`. UART1 RX4/TX5, MCP2A2 HIGH RS232/LOW RS485 readback verified. Other QC mutations blocked during serial/radio tests. Device waits30s for first byte then bounded per-phase timeout. Explicit cancellation/OTA/session reset is INCOMPLETE. UART loop work is bounded; TPL retains main-loop service. Modbus transactions remain deferred; raw binary transport test is separate.

PASS requires every laptop request/response/ACK and matching final ESP run/session result. Echo, CRC, pattern/sequence mismatch, timeout or device reboot fails. No retries hide intermittent errors. FAIL identifies a failed test, not which adapter/cable/config/device component is faulty. Export JSON/CSV; automatic reports saved under ignored tools/laptop-qc/reports. Control token never exported. ESP session results are RAM, export before reset. Laptop receipt is authoritative: ESP can accept final ACK while laptop detects final-reply/network failure. More packets/baud combinations do not certify signal voltage or long-term reliability.

## Binary protocol1

Little-endian header12bytes: magicQC, version1, type8bit, random runID32bit, sequence16bit starting0, length16bit<=128. Payload + CRC16-CCITT-FALSE over header/payload(poly0x1021/init0xFFFF/no reflection/no xorout). Request1/response2/ACK3/ACKreply4, zero-payload ACK. Only HTTP arm starts test; serial cannot arm itself. Payload lengths16/32/64/128 alternate. First4bytes00 FF55 AA; byte i afterwards=(runID>>((i%4)*8)) XOR(sequence*37) XOR(i*73), truncated8bits. Response XORs payload with0xA5, rejecting plain echo. Firmware checks request pattern/run/sequence; laptop checks complete response before ACK. Device increments after queuing ACKreply and drains final reply for3s nonblocking before PASS. This is a QC protocol, not Modbus or authentication.

## E70 tests

[Manufacturer product](https://www.cdebyte.com/products/E70-915NW14S/7) specifies TI CC1310/15.4 star, not LoRa. [Vendor-authored manual2024](https://cdn.ozdisan.com/public/product/assets/E70-915NW14S.pdf) pp17-20 describes readbacks, pp13-14 configuration/wakeup. Pinlist V2_R1 rows5-11 confirms UART2 RX16/TX17 and radio M0 MCP1B4/M1 MCP1B5. ACK34/AUX35/LINK36 displayed as signals, never sole PASS evidence. Manual table/prose disagree on normal mode bits; QC uses unambiguous configuration11 and restores prior bits without guessing production role.

Targets are editable comparison values, not setters. Initial factory references: channel0, workingmode4(pin control), airmode1(standard50kbps), PAN65535(any network), baudcode7(115200), parity0(none), powercode0(ultra-high). These are not approved production settings. Pick actual network/channel/node-role targets before production acceptance; do not rewrite modules merely to force PASS.

Save just B4/B5 latch/direction, temporary pins11, UART1152008N1, wakebyte,120ms settling, `+++` without newline; expect Enter AT Mode. Read-only CNCFG/WMCFG/TMCFG/PIDCFG/UBCFG/UPCFG/PWCFG queries `AT+<name>=?` with CRLF. AT+VER raw response optional. Only labelled decimal values in documented range count; echo/OK/missing fields never count. AT+EXIT and masked restoration/readback preserve TPL B7. Stop/OTA restore pins immediately with best-effort exit and INCOMPLETE result. No NVM writes, defaults/reset/PAN changes or RF payload test.

Communication PASS requires AT entry and channel readback. Configuration PASS requires all seven fields readable and matching target; readable mismatch FAIL, unreadable UNVERIFIED. Unconfirmed AT exit UNVERIFIED; pin restoration failure FAIL. SF/bandwidth/codingrate/LoRaCRC are NOT SUPPORTED, not PASS. Power enum is not measured dBm. Frequency for documented airmode0/1 is908.2+0.2*channelMHz; high-rate mapping not guessed.

Bench validation remains necessary for E70 command/response formatting, wake/exit/restore, TPL waveform, disconnected UART, independent RS232/RS485 fixtures and RF link with second radio if later required. Software simulation is not physical acceptance. No new NVS/EEPROM keys; targets/results remain RAM. Wi-Fi HuaweiJIN/AP12345678 and existing selective driver profile migration unchanged.
