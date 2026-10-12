#pragma once
#include <stdint.h>
constexpr uint32_t fixedSinkPDO(uint16_t mv, uint16_t ma) {
  return (uint32_t)(mv / 50u) << 10 | ma / 10u;
}
constexpr uint8_t activeSinkCount(uint8_t original, uint16_t mv) {
  return (original & ~3u) | (mv == 5000 ? 1 : 2);
}
constexpr uint32_t sourceContractMV(uint32_t rdo, const uint32_t *source, unsigned count) {
  unsigned pos = (rdo >> 28) & 7;
  return pos && pos <= count && !(source[pos - 1] >> 30) ? ((source[pos - 1] >> 10) & 1023) * 50
                                                         : 0;
}
