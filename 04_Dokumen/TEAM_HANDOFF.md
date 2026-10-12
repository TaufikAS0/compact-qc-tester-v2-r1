# Team handoff: v0.5.1

## New standard portal install/reset

Owner now authorizes standard merged USB flashing and optional Full erase + write.
Use the validated TMC_ESP32_16M_v0.5.1_merged.bin through portal USB/COM, offset 0.
It resets NVS and boot selection even in Fast write; Full erase also clears
storage and every other flash region. Back up anything needed before starting.
Bootloader/partitions/app are assembled at 0x1000/0x8000/0x20000, not Arduino
default offsets. Factory boot selection uses erased otadata, per
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/ota.html.
The legacy preserve-data procedure below remains for operators who specifically
need to keep NVS. It does not apply to the merged reset/install image.

## Initial install via USB

User assigns physical flashing to team. Verify actual USB port, MCU classic ESP32, MAC/16MB and current partition table before upload. The last verified board was COM13 CH340, ESP32-D0WD-V3 rev3.1 / 16MB; a reconnect can change port. No upload of v0.5.0 was performed by Codex.

Owner decision 2026-10-12 permits distribution with the intentional public LAB default admin / CompactLab050!. Build using Build-Firmware.ps1 -PublicLabRelease; private ota_secrets.h overrides are refused. Do not distribute private override builds. Keep OTA on the isolated LAB LAN. Portal distribution is app-only for device /update, not automatic portal OTA or merged USB. Both USB and OTA still require matching lab/pin/partition profiles.

Legacy layout from partitions.csv:

| Name | Offset | Size |
| --- | --- | --- |
| nvs | 0x9000 | 0x8000 |
| otadata | 0x17000 | 0x2000 |
| phy_init | 0x19000 | 0x1000 |
| factory | 0x20000 | 0x200000 |
| ota_0 | 0x220000 | 0x200000 |
| ota_1 | 0x420000 | 0x200000 |
| storage | 0x620000 | 0xF0000 |

Back up current partition table and NVS first. On this layout write build bootloader at **0x1000**, partitions at **0x8000**, application compact_qc_tester.ino.bin at **0x20000**. If boot selection must be reset to factory, erase **only** otadata 0x17000 size 0x2000. Preserve NVS/storage. Verify each write/readback hash. Do not write boot_app0.bin at Arduino default 0xE000, merged.bin, or erase whole flash. Arduino's default uploader/offset cannot be assumed correct. Use esptool explicit offsets after verifying current device layout. No command here auto-flashes a guessed COM port.

## Later OTA

Open http://device-IP/update (AP 192.168.4.1), login admin + LAB default (private password only for a private build), select application .ino.bin only. Keep power stable. Bootloader/partitions cannot be changed via this application OTA. On success device restarts; reconnect, verify firmware v0.5.0/next version + MD5 and serial. A successful compile does not prove OTA works electrically with watchdog timing; exercise the acceptance cases below on a bench.

## Bench acceptance checklist

- Confirm Wi-Fi HuaweiJIN and AP 12345678; reboot/profile migration preserves unrelated NVS. Update settings namespace qc-settings preserves production data.
- Measure TPL DONE waveform at MCP1B7, confirm intervals/high width under OLED/I2C/SD/Ethernet/OTA load, deliberately stop firmware service and verify TPL reset. Software pulse counter alone is not a pass.
- Verify relay/LED polarity in config.h, output fixture, MCP BANK0/SEQOP0. Unlock two flags only after verification. Check timeout/stop both states physically.
- Verify SSD1306 geometry/address; rotate through every item, including OLED; confirm only after visual check. A broken OLED must remain visible as failing in web.
- For each DI/mode/button give actual ON/OFF stimuli and use corresponding level selection; check bouncing, stuck pins, GPIO39 external bias and MCP1B3 input. Confirming before stable expected signal must be rejected. Reset/EN check after reboot with external observation; report first because RAM clears.
- Confirm ADS fitted model (not established by address), shunt/amplifier/offset/VDD and safe 4-20mA wiring; configure, check 4/12/20mA with calibrator/reference, both wrong-value and disconnected cases. Profile is RAM; configure each boot. Firmware cannot automatically detect a disconnected loop whose ADC value accidentally matches stimulus.
- EEPROM at 0x57: approve/reserve final 128 bytes region, save TOP/BOTTOM, reboot/read CRC. Reject foreign contents and WC high. Test interruption with old valid slot preserved; do not reuse QC region in production without migration.
- Confirm AHT20 model before init; compare measurement to reference. RTC exact model and battery retention test remain operator/fixture checks. ATtiny protocol not provided; no invented command. Unknown I2C ACK is only detection.
- USB-C source must offer target fixed PDO. Test 5/9/12/15/20V only with rated hardware/fixture; inspect RDO/state and measure actual output. Test unsupported PDO fallback. Runtime PDOs reset when IC resets; no NVM provisioning is performed.
- Ethernet fitted chip/link/reset circuit; check link with known cable. W5100 can report UNKNOWN link; this requires manual fixture verification and is not automatically classified as hardware failure. SPI/PHY test does not validate network packets. Extend with fixture LAN test if production QC requires packet/data verification.
- SD unique write/read/delete and cleanup failure; shared SPI CS, pin straps during reset.
- RS485/RS232 **deferred**. Check MCP2A2 levels only; do not mark comms PASS. Later fixture requires slave ID/register map/read function/baud/parity + external RS232 protocol. No UART transmissions implemented now.
- OTA login rejected without password, expired/wrong nonce, valid image, wrong image, over-2MB image, dropped upload, power interruption; active app preserved on rejected/incomplete upload; successful reboot uses correct version. No secure-boot/signed-image production claim.
- Report fresh results, deferred/incomplete states, all current points, stored identities, serial format and no control token. Reload must reflect server session; reboot creates a fresh session, never inherit PASS from old board.

## Unknowns and assumptions

Confirmed: pinlist V2_R1 applies both PCB labels; MCP23017-E/ML; STUSB4500QTR; EEPROM C7559. Assumptions requiring bench checks: SSD1306 128x64, fitted Ethernet compatible with Ethernet library; ADC/AHT model selected explicitly in web. Unknown: ADC conditioning values, RTC exact chip, ATtiny protocol, relay/LED polarity, physical reset/OTA timing acceptance, Ethernet reset polarity and packet test. RS comms were explicitly removed from immediate test scope. Full board remains INCOMPLETE until all applicable tests have evidence; deferred items prevent overall PASS.
