#pragma once
#include <stdint.h>

// PROVISIONAL profile: ESP32 classic, NOT ESP32-S3. Confirm fitted MCU first.
constexpr char FW_VERSION[] = "0.5.0";
// Shared internal lab profile authorized by the firmware workspace rules.
constexpr char STA_SSID[] = "HuaweiJIN";
constexpr char STA_PASSWORD[] = "jayaabadi100";
constexpr char AP_PASSWORD[] = "12345678";
constexpr int SDA_PIN = 21, SCL_PIN = 22;
constexpr int SPI_SCK = 18, SPI_MISO = 19, SPI_MOSI = 23;
constexpr int SD_CS = 12, ETH_CS = 15;
constexpr int INPUT_PINS[] = {13, 14, 25, 26, 2, 39, 34, 35, 36};
constexpr char const *INPUT_NAMES[] = {
    "DI1", "DI2", "DI3", "DI4", "DMM mode", "Webserver mode", "LoRa ACK", "LoRa AUX", "LoRa LINK"};
constexpr int RELAY_PINS[] = {32, 33};

// Enable ONLY after checking schematic, chip identity, polarity and bench wiring.
// With false, firmware never configures relays or LED outputs.
// The independent TPL watchdog may configure MCP1 B7 only.
constexpr bool OUTPUT_PROFILE_VERIFIED = false;
constexpr bool RELAY_ACTIVE_HIGH = true; // Unknown until verified.
constexpr bool LED_ACTIVE_HIGH = true;   // Unknown until verified.
constexpr unsigned OUTPUT_PULSE_MS = 1200;
constexpr unsigned SESSION_TIMEOUT_MS = 3500;

// Only valid if MCP1 is confirmed as MCP23017 with BANK=0, SEQOP=0.
constexpr bool MCP23017_PROFILE_VERIFIED = false;
constexpr uint8_t MCP1_ADDR = 0x20;
constexpr uint8_t LED_BITS[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
constexpr char const *LED_NAMES[] = {"LED2 WiFi",        "LED3 Critical",    "LED4 Radio",
                                     "LED11 Config DI1", "LED12 Config DI2", "LED13 Config DI3",
                                     "LED14 Config DI4", "LED15 Config DO1", "LED16 Config DO2",
                                     "LED17 Serial RX",  "LED18 Serial TX"};
constexpr uint16_t LED_MASK = 0x07ff; // Preserve B3-B7: button, LoRa, watchdog.

// V2_R1!L60:M60 and G72: MCP1B7 -> TPL5010 DONE.
// User confirmed MCP23017-E/ML on 2026-10-09; no LED/relay unlock is implied.
constexpr bool TPL_MCP23017_CONFIRMED = true;
constexpr uint8_t TPL_DONE_MASK = 0x80;
constexpr uint32_t TPL_DONE_INTERVAL_MS = 25;
constexpr uint32_t TPL_DONE_HIGH_MS = 1;

// Pinlist V2_R1 confirmed unchanged for TOP V2_R1 + BOTTOM V3_R0.
constexpr uint8_t MCP2_ADDR = 0x21;
constexpr int SERIAL_RX = 4, SERIAL_TX = 5;
constexpr char HW_TOP[] = "TMC_T_V2_R1", HW_BOTTOM[] = "TMC_B_V3_R0";
constexpr uint8_t EEPROM_ADDR = 0x57;
constexpr uint16_t EEPROM_BYTES = 16384, IDENTITY_BASE = EEPROM_BYTES - 128;
constexpr int OLED_WIDTH = 128, OLED_HEIGHT = 64;
// ADS model/transfer function require bench confirmation in web, never infer from address.
constexpr uint32_t SAMPLE_MAX_AGE_MS = 1500;
