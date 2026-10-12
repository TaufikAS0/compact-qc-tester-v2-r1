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
User reopened serial QC on 2026-10-12: RS232/RS485 laptop binary QC now authorized. RS485 automatic TX/RX with no DE/RE confirmed. Modbus transactions remain deferred. Read 04_Dokumen/COMMUNICATION_QC.md. UART1 GPIO4/5, UART2 E70 GPIO16/17.
Preserve legacy partitions.csv. Owner decision 2026-10-12 permits validated portal merged USB / full erase for deliberate LAB reset: merged fast write resets NVS, full erase also deletes storage. Component USB and app-only OTA preserve unrelated NVS/storage. Never Arduino default 0x10000/boot_app0 offsets.
Private OTA secrets, binaries and machine backups must never be committed. Owner decision 2026-10-12 permits public lab_ota_defaults.h in source/release assets; PublicLabRelease refuses private overrides. Shared HuaweiJIN lab profile is authorized; AP recovery 12345678. Customer credentials require explicit profile.
User corrected outputs 2026-10-12: DO1/DO2 active LOW, LED MCP active HIGH; profile flags enabled after confirmation. Verify runtime BANK/SEQOP and bench fixture. Preserve MCP unrelated bits, particularly TPL MCP1B7.
EEPROM identity region 0x3F80-0x3FFF is reserved for QC; never overwrite foreign bytes. STUSB PDO runtime only, no NVM write.
Run firmware compile and Node UI tests; hardware acceptance must be reported separately. Do not simulate a physical PASS.

E70-915NW14S uses CC1310/15.4 star, not LoRa. Seven documented AT readbacks are compared against editable factory reference targets, not assumed production configuration. Missing readbacks are UNVERIFIED; unsupported SF/BW/CR/LoRaCRC are never PASS. No radio parameter/NVM writes. Restore only MCP1B4/B5 mode bits/direction, preserve TPL B7, including Stop/OTA. Serial/radio tests block conflicting mutations. Local laptop backend binds loopback, opens adapters only on explicit Start, excludes tokens from JSON/CSV and keeps reports ignored. Run Python protocol/worker tests too.

QC variant is mandatory before mutations: M0 full / M1 STUSB4500 unpopulated N-A, no PD register I2C; generic ACK scan allowed. Reject PD on M1 server-side; changing profile resets session and waits for active PD. Preserve N-A applicability in overall/counters/CSV. QC1 code is evidence CRC, not security signature or PASS; never coerce INCOMPLETE/deferred/data-only to PASS. CSV keeps first ten Longhi columns, UTF8 BOM/RFC4180 and Excel formula protection.
