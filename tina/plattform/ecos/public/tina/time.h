#ifndef ECOS_TIME_H
#define ECOS_TIME_H

#include <cyg/kernel/kapi.h>

#include <tina/tina.h>
#include "timetype.h"
#include "ctime.h"

namespace TURAG {

// use type from c interface
typedef TuragSystemTicks SystemTicks; 

struct SystemTime {
  SystemTicks value;

  constexpr explicit _always_inline
  SystemTime(SystemTicks ticks = 0) :
    value(ticks)
  { }
  
  constexpr operator TuragSystemTime() const {
    return TuragSystemTime{value};
  }  
};

constexpr SystemTime operator - (SystemTime arg) { return SystemTime{-arg.value}; }
constexpr SystemTime operator + (SystemTime arg) { return arg; }
constexpr SystemTime operator + (SystemTime lhs, SystemTime rhs) { return SystemTime{lhs.value + rhs.value}; }
constexpr SystemTime operator - (SystemTime lhs, SystemTime rhs) { return SystemTime{lhs.value - rhs.value}; }
constexpr bool operator <= (SystemTime lhs, SystemTime rhs) { return lhs.value <= rhs.value; }
constexpr bool operator >= (SystemTime lhs, SystemTime rhs) { return lhs.value >= rhs.value; }
constexpr bool operator == (SystemTime lhs, SystemTime rhs) { return lhs.value == rhs.value; }
constexpr bool operator != (SystemTime lhs, SystemTime rhs) { return lhs.value != rhs.value; }
constexpr bool operator <  (SystemTime lhs, SystemTime rhs) { return lhs.value < rhs.value;  }
constexpr bool operator >  (SystemTime lhs, SystemTime rhs) { return lhs.value > rhs.value;  }

constexpr SystemTime operator * (SystemTime lhs, unsigned rhs) { return SystemTime{lhs.value * rhs}; }
constexpr SystemTime operator * (unsigned lhs, SystemTime rhs) { return SystemTime{lhs * rhs.value}; }

////////////////////////////////////////////////////////////////////////////////
// time functions

constexpr _always_inline SystemTime s_to_ticks(int s) {
  return SystemTime((s * 1000000000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR)
              / static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR)));
}

constexpr _always_inline SystemTime ms_to_ticks(int ms) {
  return SystemTime((ms * 1000000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR)
               / static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR)));
}

constexpr _always_inline SystemTime us_to_ticks(int us) {
  return SystemTime((us * 1000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR)
               / static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR)));
}

constexpr _always_inline unsigned ticks_to_s(SystemTime ticks) {
  return (ticks.value * static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR))
      / (1000000000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR));
}

constexpr _always_inline unsigned ticks_to_ms(SystemTime ticks) {
  return (ticks.value * static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR))
      / (1000000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR));
}

constexpr _always_inline unsigned ticks_to_us(SystemTime ticks) {
  return (ticks.value * static_cast<uint64_t>(CYGNUM_HAL_RTC_NUMERATOR))
      / (1000ULL * static_cast<uint64_t>(CYGNUM_HAL_RTC_DENOMINATOR));
}

/// Get number of ecos ticks since system start
/**
 * \return ecos ticks
 */
_always_inline SystemTime get_current_tick() { // [tick]
  return SystemTime(cyg_current_time());
}

} // namespace TURAG

#endif // ECOS_TIME_H
