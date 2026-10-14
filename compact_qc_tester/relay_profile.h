#pragma once

constexpr int RELAY_PINS[] = {32, 33};
// User confirms BOTH DO1/DO2 physically invert ON/OFF on v1.5.0.
// Updated active HIGH profile, 2026-10-14; failed relay is a hardware issue.
constexpr bool RELAY_ACTIVE_HIGH[] = {true, true};
constexpr int relayLevel(int index, bool on) {
  return on == RELAY_ACTIVE_HIGH[index] ? 1 : 0;
}

// Preload the hardware latch before enabling the driver, then explicitly write
// again. Arduino-ESP32 3.x digitalWrite before pinMode is silently ineffective.
template <typename Preload, typename Configure, typename Write>
constexpr bool initializeRelay(int index, Preload preload, Configure configure, Write write) {
  const int off = relayLevel(index, false);
  if (!preload(RELAY_PINS[index], off)) return false;
  configure(RELAY_PINS[index]);
  return write(RELAY_PINS[index], off);
}
