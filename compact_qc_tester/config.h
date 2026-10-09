#pragma once

// PROVISIONAL profile: ESP32 classic, NOT ESP32-S3. Confirm fitted MCU first.
constexpr char FW_VERSION[] = "0.1.0";
constexpr char AP_PASSWORD[] = "compactqc123"; // Local bench only; change before use.
constexpr int SDA_PIN = 21, SCL_PIN = 22;
constexpr int SPI_SCK = 18, SPI_MISO = 19, SPI_MOSI = 23;
constexpr int SD_CS = 12, ETH_CS = 15;
constexpr int INPUT_PINS[] = {13, 14, 25, 26, 2, 39, 34, 35, 36};
constexpr char const *INPUT_NAMES[] = {"DI1", "DI2", "DI3", "DI4", "Switch kiri", "Switch kanan", "LoRa ACK", "LoRa AUX", "LoRa LINK"};
constexpr int RELAY_PINS[] = {32, 33};

// Enable ONLY after checking schematic, chip identity, polarity and bench wiring.
// With false, firmware never configures relay pins or writes MCP registers.
constexpr bool OUTPUT_PROFILE_VERIFIED = false;
constexpr bool RELAY_ACTIVE_HIGH = true; // Unknown until verified.
constexpr bool LED_ACTIVE_HIGH = true;   // Unknown until verified.
constexpr unsigned OUTPUT_PULSE_MS = 1200;
constexpr unsigned SESSION_TIMEOUT_MS = 3500;

// Only valid if MCP1 is confirmed as MCP23017 with BANK=0, SEQOP=0.
constexpr bool MCP23017_PROFILE_VERIFIED = false;
constexpr uint8_t MCP1_ADDR = 0x20;
constexpr uint8_t LED_BITS[] = {0,1,2,3,4,5,6,7,8,9,10};
constexpr char const *LED_NAMES[] = {"LED2 WiFi", "LED3 Critical", "LED4 Radio", "LED11 Config DI1", "LED12 Config DI2", "LED13 Config DI3", "LED14 Config DI4", "LED15 Config DO1", "LED16 Config DO2", "LED17 Serial RX", "LED18 Serial TX"};
constexpr uint16_t LED_MASK = 0x07ff; // Preserve B3-B7: button, LoRa, watchdog.
