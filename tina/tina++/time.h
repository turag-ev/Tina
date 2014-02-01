#ifndef TINAPP_TIME_H
#define TINAPP_TIME_H

#include <tina/time.h>
#include <tina++/tina.h>

namespace TURAG {

class SystemTime {
public:
    TuragSystemTicks value;

    constexpr explicit
    SystemTime(TuragSystemTicks ticks = 0) :
        value(ticks)
    { }

    constexpr
    SystemTime(TuragSystemTime time) :
        value(time.value)
    { }

    constexpr
    operator TuragSystemTime () {
        return TuragSystemTime{value};
    }

    constexpr
    static SystemTime from_ms(unsigned ms) {
        return turag_ms_to_ticks(ms);
    }

    constexpr
    static SystemTime from_s(unsigned s) {
        return turag_s_to_ticks(s);
    }

    void assign_from_ms(unsigned ms) {
        *this = turag_ms_to_ticks(ms);
    }

    void assign_from_s(unsigned s) {
        *this = turag_s_to_ticks(s);
    }

    constexpr
    unsigned to_ms() const {
        return turag_ticks_to_ms(TuragSystemTime{value});
    }

    constexpr
    unsigned to_s() const {
        return turag_ticks_to_s(TuragSystemTime{value});
    }

    TuragSystemTicks getNativeValue() const {
        return value;
    }

    static SystemTime now() {
        return turag_get_current_tick();
    }

    constexpr
    static SystemTime infinite() {
        return SystemTime(TURAG_TIME_INFINITE_TICKS);
    }
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

constexpr SystemTime operator * (SystemTime lhs, unsigned rhs) { return SystemTime(lhs.value * rhs); }
constexpr SystemTime operator * (unsigned lhs, SystemTime rhs) { return SystemTime(lhs * rhs.value); }

constexpr _always_inline SystemTime s_to_ticks(unsigned s) {
    return SystemTime::from_s(s);
}

constexpr _always_inline SystemTime ms_to_ticks(unsigned ms) {
    return SystemTime::from_ms(ms);
}

constexpr _always_inline unsigned ticks_to_s(SystemTime ticks) {
    return ticks.to_s();
}

constexpr _always_inline unsigned ticks_to_ms(SystemTime ticks) {
    return ticks.to_ms();
}

#if GCC_VERSION >= 40700
constexpr SystemTime operator"" _s(long double a) { return SystemTime::from_s(a); }
constexpr SystemTime operator"" _ms(long double a) { return SystemTime::from_ms(a); }
constexpr SystemTime operator"" _s(unsigned long long a) { return SystemTime::from_s(a); }
constexpr SystemTime operator"" _ms(unsigned long long a) { return SystemTime::from_ms(a); }
#endif

/// Get number of ecos ticks since system start
/**
 * \return ecos ticks
 */
_always_inline SystemTime get_current_tick() { // [tick]
    return SystemTime::now();
}

} // namespace TURAG

#endif // TINAPP_SIM_TIME_H
