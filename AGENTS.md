# Compact QC firmware

Read README.md, VALIDATION.md and 04_Dokumen/TEAM_HANDOFF.md before editing/build/upload.
Use feature branches from develop; never push directly to main or merge without review.
Read workspace vault `01_Rules/Rules_Versioning.md` and `01_Rules/Rules_Change_Notifications.md` fully before firmware/build/push. Links are in `../2026 Vault Firmware Rules/00_Start_Here/Firmware_AI_Vault_Home.md`.
Version truth: compact_qc_tester/firmware_version.h::FW_VERSION. Target classic ESP32 only, 16MB.
Build ONLY through Build-Firmware.ps1: generate ignored UTC metadata and gate final binary/manifest. Never manually label a stale binary with a fresh timestamp. Record changes in CHANGELOG.md and handoff; report version before/after, actual build UTC, tests and upload status.
Keep sketch thin; app, hardware QC, session, Wi-Fi, watchdog and OTA remain separate.
User-defined QC: DI1-4 and DMM/Normal/Webserver auto PASS after two debounced HIGH-LOW-HIGH cycles. Normal means GPIO2 LOW AND GPIO39 LOW (user confirmed). No operator confirmation for inputs. User visual confirmations accept OLED/LED/DO observations; label evidence accordingly.
Do not equate ACK, output command, software pulse count, PHY link, regression residual or PD contract voltage with measured electrical acceptance.
User confirmed ADS1115, DS3231 and AHT10. Do not send AHT20 initialization/CRC commands to AHT10.
Calibration namespace qc-adc/cal-v1 is independent of production namespaces and PCB EEPROM. No erase/clear of unrelated keys.
RS485/RS232 external-device tests are deferred by user; do not silently activate them.
Preserve legacy partitions.csv and all unrelated NVS/storage. No merged/default Arduino USB upload on existing hardware.
OTA secrets, binaries and machine backups must never be committed. Shared HuaweiJIN lab profile is authorized; AP recovery 12345678. Customer credentials require explicit profile.
User corrected outputs 2026-10-12: DO1/DO2 active LOW, LED MCP active HIGH; profile flags enabled after confirmation. Verify runtime BANK/SEQOP and bench fixture. Preserve MCP unrelated bits, particularly TPL MCP1B7.
EEPROM identity region 0x3F80-0x3FFF is reserved for QC; never overwrite foreign bytes. STUSB PDO runtime only, no NVM write.
Run firmware compile and Node UI tests; hardware acceptance must be reported separately. Do not simulate a physical PASS.
