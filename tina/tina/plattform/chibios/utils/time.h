#ifndef CHIBIOS_TIME_H
#define CHIBIOS_TIME_H

#include <ch.h>

#include <utils/types.h>
#include <utils/extra/normalize.h>
#include "timetype.h"

struct SystemTime {
  SystemTicks value;

  constexpr explicit _always_inline
  SystemTime(SystemTicks ticks = 0) :
    value(ticks)
  { }
  
  constexpr _always_inline
  SystemTime(int t, SystemTicks ticks) :
    value((t == 0) ? TIME_IMMEDIATE : ((t < 0) ? TIME_INFINITE : ticks))
  { }
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

constexpr _always_inline SystemTime s_to_ticks(int s) {
  return SystemTime(s, S2ST(s));
}

constexpr _always_inline SystemTime ms_to_ticks(int ms) {
  return SystemTime(ms, MS2ST(ms));
}

constexpr _always_inline SystemTime us_to_ticks(int us) {
  return SystemTime(us, US2ST(us));
}

constexpr _always_inline unsigned ticks_to_s(SystemTime ticks) {
  return ticks.value / CH_FREQUENCY;
}

constexpr _always_inline unsigned ticks_to_ms(SystemTime ticks) {
  return ((ticks.value - 1L) * 1000L) / CH_FREQUENCY + 1L;
}

constexpr _always_inline unsigned ticks_to_us(SystemTime ticks) {
  return ((ticks.value - 1L) * 1000000L) / CH_FREQUENCY + 1L;
}

/// Get number of sys ticks since system start
/**
 * \return sys ticks
 */
_always_inline SystemTime get_current_tick() { // [tick]
  return SystemTime{chTimeNow()};
}

#endif // CHIBIOS_TIME_H
