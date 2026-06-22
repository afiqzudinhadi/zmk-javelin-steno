#include "clock.h"

// TODO Phase 2: Replace with Zephyr kernel time functions.
// #include <zephyr/kernel.h>
// uint32_t Clock::GetMilliseconds() { return k_uptime_get_32(); }
// uint32_t Clock::GetMicroseconds() { return k_cyc_to_us_floor32(k_cycle_get_32()); }
// void Clock::Sleep(uint32_t ms) { k_msleep(ms); }

uint32_t Clock::GetMilliseconds() {
  return 0;
}

uint32_t Clock::GetMicroseconds() {
  return 0;
}

void Clock::Sleep(uint32_t milliseconds) {
  (void)milliseconds;
}
