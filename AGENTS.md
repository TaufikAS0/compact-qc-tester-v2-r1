# Compact QC firmware

Read README.md, VALIDATION.md and 04_Dokumen/TEAM_HANDOFF.md before editing/build/upload.
Use feature branches from develop; never push directly to main or merge without review.
Version truth: compact_qc_tester/config.h::FW_VERSION. Target classic ESP32 only, 16MB.
Keep sketch thin; app, hardware QC, session, Wi-Fi, watchdog and OTA remain separate.
Do not equate ACK, output command, software pulse count, PHY link or operator click alone with automatic functional acceptance.
RS485/RS232 external-device tests are deferred by user; do not silently activate them.
Preserve legacy partitions.csv and all unrelated NVS/storage. No merged/default Arduino USB upload on existing hardware.
Private OTA secrets, binaries and machine backups must never be committed. Owner decision 2026-10-12 permits lab_ota_defaults.h public LAB defaults in source/release BINs, never a private ota_secrets.h override build. Shared HuaweiJIN lab profile is authorized; AP recovery 12345678. Customer credentials require explicit profile.
Output flags remain locked until polarity/fixture verified. Preserve MCP unrelated bits, particularly TPL MCP1B7.
EEPROM identity region 0x3F80-0x3FFF is reserved for QC; never overwrite foreign bytes. STUSB PDO runtime only, no NVM write.
Run firmware compile and Node UI tests; hardware acceptance must be reported separately. Do not simulate a physical PASS.
