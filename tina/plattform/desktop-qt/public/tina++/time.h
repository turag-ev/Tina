#ifndef TINAPP_SIM_TIME_H
#define TINAPP_SIM_TIME_H

#include <limits>

#include <tina++/tina.h>

#include <tina/time.h>

namespace TURAG {

struct SystemTime {
  TuragSystemTicks value;

#undef INFINITE
  static constexpr TuragSystemTicks INFINITE = std::numeric_limits<TuragSystemTicks>::max();

  constexpr explicit
  SystemTime(TuragSystemTicks ticks = 0) :
    value(ticks)
  { }

  constexpr
  SystemTime(TuragSystemTime time) :
    value(time.value)
  { }
};

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

constexpr _always_inline SystemTime s_to_ticks(unsigned s) {
  return SystemTime{1000 * s};
}

constexpr _always_inline SystemTime ms_to_ticks(unsigned ms) {
  return SystemTime{ms};
}

constexpr _always_inline SystemTime us_to_ticks(unsigned us) {
  return SystemTime{us / 1000};
}

constexpr _always_inline unsigned ticks_to_s(SystemTime ticks) {
  return ticks.value / 1000;
}

constexpr _always_inline unsigned ticks_to_ms(SystemTime ticks) {
  return ticks.value;
}

constexpr _always_inline unsigned ticks_to_us(SystemTime ticks) {
  return ticks.value * 1000;
}

#if GCC_VERSION > 40700
constexpr SystemTime operator"" _s(long double a) { return s_to_ticks(a); }
constexpr SystemTime operator"" _ms(long double a) { return ms_to_ticks(a); }
constexpr SystemTime operator"" _s(unsigned long long a) { return s_to_ticks(a); }
constexpr SystemTime operator"" _ms(unsigned long long a) { return ms_to_ticks(a); }
#endif


extern uint64_t turag_plattform_dependent_get_tick(void);

/// Get number of sys ticks since system start
/**
 * \return sys ticks
 */
_always_inline SystemTime get_current_tick() { // [tick]
  return SystemTime{turag_plattform_dependent_get_tick()};
}

} // namespace TURAG

#endif // TINAPP_SIM_TIME_H
