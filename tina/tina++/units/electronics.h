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

/// Typ für Wiederstände
typedef decltype(Voltage() / Current()) Resistance;

/// Typ für magmetische Feldstärke
typedef decltype(Mass() / (Current() * Time() * Time())) MagneticField;

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

/// Tesla
constexpr MagneticField::UnitType T(kg / (A * s*s));

// TODO: mehr Einheiten ...

} } // namespace TURAG::Units

#endif // TINAPP_UNITS_ELECTRONICS_H

