#ifndef TINAPP_TIME_H
#define TINAPP_TIME_H

#include <tina/time.h>
#include <tina++/tina.h>
#include <tina++/units.h>

namespace TURAG {

/// \defgroup Time Zeit [C/C++]
/// Sachen mit Zeit
/// \{

/// \brief Zeitpunkt in Systemzeit
class SystemTime {
public:
    /// Systemzeit aus Systemticks erstellen
	constexpr explicit
    SystemTime(TuragSystemTicks ticks = 0) :
        value(ticks)
    { }
    
    /// explizit aus Einheitentyp erstellen
    constexpr explicit
    SystemTime(Units::Time time) :
        value(turag_ms_to_ticks(time.toInt(Units::ms)).value)
    { }

    /// implizit Systemzeit aus äquivalenten C-Typ erstellen
    constexpr
    SystemTime(TuragSystemTime time) :
        value(time.value)
    { }

    /// Kopierkonstruktor
    constexpr SystemTime(const SystemTime& time) = default;

    /// Kopierkonstruktor
    SystemTime& operator=(const SystemTime&) = default;

	SystemTime& operator+=(const SystemTime& other)
	{
		value += other.toTicks();
		return *this;
	}

    /// implizit in äquivalenten C-Typ umwandeln
    constexpr
    operator TuragSystemTime () const {
        return TuragSystemTime{value};
    }

    /// Systemzeit aus \p us Mikrosekunden erstellen
    constexpr
    static SystemTime fromUsec(unsigned us) {
        return turag_us_to_ticks(us);
    }

    /// Systemzeit aus \p ms Millisekunden erstellen
    constexpr
    static SystemTime fromMsec(unsigned ms) {
        return turag_ms_to_ticks(ms);
    }

    /// Systemzeit aus \p s Sekunden erstellen
    constexpr
    static SystemTime fromSec(unsigned s) {
        return turag_s_to_ticks(s);
    }

    /// Systemzeit aus \p s Sekunden in Double erstellen (wird in us umgewandelt und dann konvertiert)
    constexpr
    static SystemTime fromSecDouble(double s) {
        return turag_us_to_ticks((double)1e6*s);
    }
    
    /// Systemzeit aus Zeiteinheit \p time erstellen
    constexpr
    static SystemTime fromTime(Units::Time time) {
        return SystemTime(time);
    }

    /// gespeicherte Systemzeit auf \p us Mikrosekunden setzen
    void assignFromUsec(unsigned us) {
        *this = turag_us_to_ticks(us);
    }

    /// gespeicherte Systemzeit auf \p ms Millisekunden setzen
    void assignFromMsec(unsigned ms) {
        *this = turag_ms_to_ticks(ms);
    }

    /// gespeicherte Systemzeit auf \p s Sekunden setzen
    void assignFromSec(unsigned s) {
        *this = turag_s_to_ticks(s);
    }
    
    /// gespeicherte Systemzeit auf angegebene Zeit setzen
    void assignFromTime(Units::Time time) {
        assignFromMsec(time.toInt(Units::ms));
    }

    /// gespeicherte Systemzeit in Mikrosekunden
    constexpr
    unsigned toUsec() const {
        return turag_ticks_to_us(TuragSystemTime{value});
    }
    
    /// gespeicherte Systemzeit in Millisekunden
    constexpr
    unsigned toMsec() const {
        return turag_ticks_to_ms(TuragSystemTime{value});
    }

    /// gespeicherte Systemzeit in Sekunden
    constexpr
    unsigned toSec() const {
        return turag_ticks_to_s(TuragSystemTime{value});
    }

    /// gespeicherte Systemzeit in plattformabhängigen Ticks
    constexpr
    TuragSystemTicks toTicks() const {
        return value;
    }
    
    /// gespeicherte Systemzeit in Zeiteinheiten
    constexpr
    Units::Time toTime() const {
        return Units::Real(value) / Units::Real(frequency()) * Units::s;
    }

    /// aktuelle Systemzeit
    static SystemTime now() {
        return turag_get_current_tick();
    }

    /// keine Zeit (sofort) für Parameter
    const
    static SystemTime immediate() {
        return SystemTime(TURAG_TIME_IMMEDIATE_TICKS);
    }

    /// unendliche Zeit für Parameter
    constexpr
    static SystemTime infinite() {
        return SystemTime(TURAG_TIME_INFINITE_TICKS);
    }

    /// Frequenz der plattformabhängigen Ticks
    constexpr
    static unsigned frequency() {
        return turag_get_systick_frequency();
    }

private:
    TuragSystemTicks value;
};

///
/// \}

/// \relates SystemTime
/// \brief Systemzeit vergleichen
constexpr SystemTime operator + (SystemTime arg) {
    return arg;
}
/// \relates SystemTime
/// \brief Systemzeit vergleichen
constexpr SystemTime operator + (SystemTime lhs, SystemTime rhs) {
    return SystemTime{lhs.toTicks() + rhs.toTicks()};
}

/// \relates SystemTime
/// \brief Differenz zwischen zwei Systemzeitpunkten bilden
constexpr SystemTime operator - (SystemTime lhs, SystemTime rhs) {
    return SystemTime{lhs.toTicks() - rhs.toTicks()};
}

/// \relates SystemTime
/// \brief Systemzeit multiplizieren
constexpr SystemTime operator * (SystemTime lhs, unsigned rhs) {
    return SystemTime(lhs.toTicks() * rhs);
}
/// \relates SystemTime
/// \brief Systemzeit multiplizieren
constexpr SystemTime operator * (unsigned lhs, SystemTime rhs) {
    return SystemTime(lhs * rhs.toTicks());
}

/// \relates SystemTime
/// \brief Systemzeit vergleichen
constexpr bool operator <= (SystemTime lhs, SystemTime rhs) { return lhs.toTicks() <= rhs.toTicks(); }
/// \relates SystemTime
/// \brief Systemzeit vergleichen
constexpr bool operator >= (SystemTime lhs, SystemTime rhs) { return lhs.toTicks() >= rhs.toTicks(); }
/// \relates SystemTime
/// \brief Systemzeit vergleichen
constexpr bool operator == (SystemTime lhs, SystemTime rhs) { return lhs.toTicks() == rhs.toTicks(); }
/// \relates SystemTime
/// \brief Systemzeit vergleichen
constexpr bool operator != (SystemTime lhs, SystemTime rhs) { return lhs.toTicks() != rhs.toTicks(); }
/// \relates SystemTime
/// \brief Systemzeit vergleichen
constexpr bool operator <  (SystemTime lhs, SystemTime rhs) { return lhs.toTicks() <  rhs.toTicks();  }
/// \relates SystemTime
/// \brief Systemzeit vergleichen
constexpr bool operator >  (SystemTime lhs, SystemTime rhs) { return lhs.toTicks() >  rhs.toTicks();  }

#if GCC_VERSION >= 40700 || defined(__DOXYGEN__)

inline namespace Literals {
inline namespace SystemTimeLiterals {

/// \relates SystemTime
/// \brief Systemzeit aus Sekundenangabe erstellen
constexpr SystemTime operator"" _s(long double a) { return SystemTime::fromMsec(a / 1000.0L); }
/// \relates SystemTime
/// \brief Systemzeit aus Sekundenangabe erstellen
constexpr SystemTime operator"" _s(unsigned long long a) { return SystemTime::fromSec(a); }

/// \relates SystemTime
/// \brief Systemzeit aus Millisekundenangabe erstellen
constexpr SystemTime operator"" _ms(unsigned long long a) { return SystemTime::fromMsec(a); }

} // inline namespace SystemTimeLiterals
} // inline namespace Literals

#endif

// alte Versionen:

/// \relates SystemTime
/// Systemzeit aus \p ms Millisekunden erstellen
/// \deprecated benutze SystemTime::fromMsec
constexpr _always_inline
static SystemTime ms_to_ticks(unsigned ms) {
	return turag_ms_to_ticks(ms);
}

/// Systemzeit aus \p s Sekunden erstellen
/// \relates SystemTime
/// \deprecated benutze SystemTime::fromSec
constexpr _always_inline
static SystemTime s_to_ticks(unsigned s) {
    return turag_s_to_ticks(s);
}

/// gespeicherte Systemzeit in Millisekunden
/// \relates SystemTime
/// \deprecated benutze SystemTime::toMsec
constexpr _always_inline
unsigned ticks_to_ms(SystemTime time) {
    return turag_ticks_to_ms(time);
}

/// gespeicherte Systemzeit in Sekunden
/// \relates SystemTime
/// \deprecated benutze SystemTime::toMsec
constexpr _always_inline
unsigned ticks_to_s(SystemTime time) {
    return turag_ticks_to_s(time);
}

} // namespace TURAG

#endif // TINAPP_SIM_TIME_H
