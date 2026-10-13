#pragma once
#include <stdint.h>
constexpr uint8_t mcpMaskedByte(uint8_t current, uint8_t mask, uint8_t requested) {
  return (current & uint8_t(~mask)) | (requested & mask);
}
