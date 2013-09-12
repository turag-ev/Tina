#ifndef ECOS_TIME_H
#define ECOS_TIME_H

#include <cyg/kernel/kapi.h>

#include <utils/types.h>
#include <utils/extra/normalize.h>
#include "timetype.h"

////////////////////////////////////////////////////////////////////////////////
// time functions

constexpr _always_inline Time s_to_ticks(uint64_t s) {
  return (s * 1000000000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR)
              / static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR));
}

constexpr _always_inline Time ms_to_ticks(uint64_t ms) {
  return (ms * 1000000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR)
               / static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR));
}

constexpr _always_inline Time us_to_ticks(uint64_t us) {
  return (us * 1000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR)
               / static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR));
}

constexpr _always_inline Time ticks_to_s(uint64_t ticks) {
  return (ticks * static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR))
      / (1000000000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR));
}

constexpr _always_inline Time ticks_to_ms(uint64_t ticks) {
  return (ticks * static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR))
      / (1000000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR));
}

constexpr _always_inline Time ticks_to_us(uint64_t ticks) {
  return (ticks * static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR))
      / (1000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR));
}

/// Get number of ecos ticks since system start
/**
 * \return ecos ticks
 */
_always_inline Time get_current_tick() { // [tick]
  return cyg_current_time();
}

#endif // ECOS_TIME_H
