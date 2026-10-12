#pragma once
#include <stdint.h>
#include <stddef.h>

namespace SerialQc {
constexpr size_t HEADER = 12, MAX_PAYLOAD = 128, MAX_FRAME = HEADER + MAX_PAYLOAD + 2;
constexpr uint8_t VERSION = 1, REQUEST = 1, RESPONSE = 2, ACK = 3, ACK_REPLY = 4;
constexpr uint16_t crc16(const uint8_t *p, size_t n) {
  uint16_t crc = 0xffff;
  while (n--) {
    crc ^= static_cast<uint16_t>(*p++) << 8;
    for (int i = 0; i < 8; ++i)
      crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
  }
  return crc;
}
constexpr uint16_t get16(const uint8_t *p) { return p[0] | (uint16_t(p[1]) << 8); }
constexpr uint32_t get32(const uint8_t *p) {
  return uint32_t(get16(p)) | (uint32_t(get16(p + 2)) << 16);
}
constexpr void put16(uint8_t *p, uint16_t v) { p[0] = v; p[1] = v >> 8; }
constexpr void put32(uint8_t *p, uint32_t v) { put16(p, v); put16(p + 2, v >> 16); }
constexpr size_t payloadSize(uint16_t seq) { return size_t(16) << (seq % 4); }
constexpr uint8_t pattern(uint32_t run, uint16_t seq, size_t index) {
  const uint8_t fixed[] = {0x00, 0xff, 0x55, 0xaa};
  if (index < 4) return fixed[index];
  return uint8_t((run >> ((index % 4) * 8)) ^ (seq * 37u) ^ (index * 73u));
}
constexpr size_t frame(uint8_t *out, uint8_t type, uint32_t run, uint16_t seq) {
  const size_t n = type == REQUEST || type == RESPONSE ? payloadSize(seq) : 0;
  out[0] = 'Q'; out[1] = 'C'; out[2] = VERSION; out[3] = type;
  put32(out + 4, run); put16(out + 8, seq); put16(out + 10, n);
  for (size_t i = 0; i < n; ++i)
    out[HEADER + i] = pattern(run, seq, i) ^ (type == RESPONSE ? 0xa5 : 0);
  put16(out + HEADER + n, crc16(out, HEADER + n));
  return HEADER + n + 2;
}
constexpr bool valid(const uint8_t *p, size_t n) {
  return n >= HEADER + 2 && p[0] == 'Q' && p[1] == 'C' && p[2] == VERSION &&
         get16(p + 10) <= MAX_PAYLOAD && n == HEADER + get16(p + 10) + 2 &&
         crc16(p, n - 2) == get16(p + n - 2);
}
} // namespace SerialQc
