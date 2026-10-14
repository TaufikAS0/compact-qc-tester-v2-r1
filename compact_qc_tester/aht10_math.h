#pragma once
#include <stdint.h>
namespace Aht10 {
constexpr uint32_t COMMAND_SETTLE_MS=350, MEASURE_TIMEOUT_MS=500;
constexpr bool calibrated(uint8_t status) { return (status & 0x88)==0x08; }
constexpr uint32_t humidityRaw(const uint8_t *b) { return uint32_t(b[1])<<12 | uint32_t(b[2])<<4 | b[3]>>4; }
constexpr uint32_t temperatureRaw(const uint8_t *b) { return uint32_t(b[3]&15)<<16 | uint32_t(b[4])<<8 | b[5]; }
constexpr float humidity(const uint8_t *b) { return humidityRaw(b)*100.0f/1048576; }
constexpr float temperature(const uint8_t *b) { return temperatureRaw(b)*200.0f/1048576-50; }
constexpr uint8_t reference[]={0x08,0x80,0x00,0x06,0x00,0x00};
static_assert(humidity(reference)==50 && temperature(reference)==25);
static_assert(calibrated(0x08) && !calibrated(0x88) && !calibrated(0));
}
