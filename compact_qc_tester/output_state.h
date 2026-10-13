#pragma once
#include <stdint.h>

// Logical command/timer state only; physical output acceptance is user evidence.
struct OutputState {
  static constexpr int COUNT = 13;
  bool on[COUNT] = {};
  uint32_t started[COUNT] = {};
  constexpr void set(int index, bool value, uint32_t now) {
    if (index < 0 || index >= COUNT) return;
    on[index] = value;
    if (value) started[index] = now;
  }
  constexpr bool any() const {
    for (bool value : on) if (value) return true;
    return false;
  }
  constexpr bool expired(int index, uint32_t now, uint32_t limit) const {
    return on[index] && uint32_t(now - started[index]) >= limit;
  }
  constexpr void clear() { for (auto &value : on) value = false; }
};
