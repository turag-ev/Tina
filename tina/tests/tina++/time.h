#ifndef TINAPP_SIM_TIME_H
#define TINAPP_SIM_TIME_H

#include <limits>

#include <tina++/tina.h>

namespace TURAG {

struct SystemTime {
  uint64_t value;

#undef INFINITE
  static constexpr uint64_t INFINITE = std::numeric_limits<uint64_t>::max();

  constexpr explicit
  SystemTime(uint64_t ticks = 0) :
    value(ticks)
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

} // namespace TURAG

#endif // TINAPP_SIM_TIME_H
