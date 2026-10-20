#pragma once
#include <cstdint>
constexpr uint32_t WIFI_RETRY_INTERVAL_MS = 5u * 60u * 1000u;
constexpr bool wifiRetryDue(uint32_t now, uint32_t lastAttempt, bool connected,
                            bool busy, bool pending) {
  return !connected && !busy && !pending &&
         uint32_t(now-lastAttempt)>=WIFI_RETRY_INTERVAL_MS;
}
static_assert(!wifiRetryDue(299999,0,false,false,false));
static_assert(wifiRetryDue(300000,0,false,false,false));
static_assert(!wifiRetryDue(300000,0,true,false,false));
static_assert(!wifiRetryDue(300000,0,false,true,false));
static_assert(!wifiRetryDue(300000,0,false,false,true));
static_assert(wifiRetryDue(299984,0xfffffff0u,false,false,false));
