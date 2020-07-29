#ifndef TINAPP_UNITS_ELECTRONICS_H
#define TINAPP_UNITS_ELECTRONICS_H

#include "unit.h"
#include "units.h"
#include "mechanics.h"

namespace TURAG {
namespace Units {

// Größen

/// Typ für Spannungsvariablen
typedef Quantity<VoltageDimension> Voltage;

/// Typ für Leistung
typedef decltype(Current() * Voltage()) Power;

/// Typ für Widerstände
typedef decltype(Voltage() / Current()) Resistance;

/// Typ für magnetische Feldstärke
typedef decltype(Mass() / (Current() * Time() * Time())) MagneticField;

/// Typ für Frequenzen
typedef decltype(Real() / Time()) Frequency;

// TODO: mehr Größen ...

// Einheiten

/// Definition für Volt
constexpr Voltage::UnitType V(N*m/(A*s));

/// Definition für Millivolt
constexpr Voltage::UnitType mV(V.factor / UnitFactor(1000));

/// Definition für Watt
constexpr Power::UnitType W(A * V);

/// Definition für Milliwatt
constexpr Power::UnitType mW(mA * V);

/// Definition für Ohm
constexpr Resistance::UnitType Ohm(V / A);

/// Definition für Milliohm
constexpr Resistance::UnitType mOhm(Ohm.factor / UnitFactor(1000));

/// Definition für Tesla
constexpr MagneticField::UnitType T(kg / (A * s*s));

/// Hertz
constexpr Frequency::UnitType Hz(UnitFactor(1) / s.factor);

/// Kilohertz
constexpr Frequency::UnitType kHz(UnitFactor(1000) * Hz.factor);

/// Megahertz
constexpr Frequency::UnitType MHz(UnitFactor(1000) * kHz.factor);

// TODO: mehr Einheiten ...

} // namespace Units

#if __cplusplus >= 201103L

inline namespace Literals {
inline namespace UnitsLiterals {

/// \ingroup UnitsDefs
/// \{

constexpr Units::Voltage operator"" _V(long double x) { return x * Units::V; }
constexpr Units::Voltage operator"" _V(long long unsigned x) { return x * Units::V; }
constexpr Units::Voltage operator"" _mV(long double x) { return x * Units::mV; }
constexpr Units::Voltage operator"" _mV(long long unsigned x) { return x * Units::mV; }

constexpr Units::Power operator"" _W(long double x) { return x * Units::W; }
constexpr Units::Power operator"" _W(long long unsigned x) { return x * Units::W; }
constexpr Units::Power operator"" _mW(long double x) { return x * Units::mW; }
constexpr Units::Power operator"" _mW(long long unsigned x) { return x * Units::mW; }

constexpr Units::Resistance operator"" _Ohm(long double x) { return x * Units::Ohm; }
constexpr Units::Resistance operator"" _Ohm(long long unsigned x) { return x * Units::Ohm; }
constexpr Units::Resistance operator"" _mOhm(long double x) { return x * Units::mOhm; }
constexpr Units::Resistance operator"" _mOhm(long long unsigned x) { return x * Units::mOhm; }

constexpr Units::MagneticField operator"" _T(long double x) { return x * Units::T; }
constexpr Units::MagneticField operator"" _T(long long unsigned x) { return x * Units::T; }

constexpr Units::Frequency operator"" _Hz(long double x) { return x * Units::Hz; }
constexpr Units::Frequency operator"" _Hz(long long unsigned x) { return x * Units::Hz; }
constexpr Units::Frequency operator"" _kHz(long double x) { return x * Units::kHz; }
constexpr Units::Frequency operator"" _kHz(long long unsigned x) { return x * Units::kHz; }
constexpr Units::Frequency operator"" _MHz(long double x) { return x * Units::MHz; }
constexpr Units::Frequency operator"" _MHz(long long unsigned x) { return x * Units::MHz; }

/// \}

} // inline namespace UnitsLiterals
} // inline namespace Literals

namespace Units {
    using namespace Literals::UnitsLiterals;
} // namespace Units

#endif

} // namespace TURAG

#endif // TINAPP_UNITS_ELECTRONICS_H

