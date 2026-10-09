# Validation — 2026-10-09

- Arduino CLI compile `esp32:esp32:esp32`, ESP32 core 3.3.10, ArduinoJson 7.4.3: PASS.
- Sketch storage: 1,035,018 / 1,310,720 bytes (78%). Global RAM: 48,972 / 327,680 bytes (14%).
- Node UI tests: PASS for output locks, rejecting unattempted LED confirmation, required report identity, missing-test status, FAIL propagation, rejecting overall PASS for partial evidence, and clearing results for a new inspection.
- Browser preview loaded and layout inspected. Preview is explicitly labelled disconnected from hardware and output actions are disabled.
- Configuration used: provisional classic ESP32 profile; OUTPUT_PROFILE_VERIFIED=false; MCP23017_PROFILE_VERIFIED=false.
- NOT VERIFIED: MCU physically fitted, boot straps, power levels, relay/LED polarity, AP on-device, I2C, SD, physical outputs, timing/disconnect on-device, watchdog behaviour, electrical performance or post-power-cycle operation.
- No firmware was flashed and no existing firmware project was edited.
