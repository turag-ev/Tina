#ifndef TINAPP_UNITS_MECHANICS_H
#define TINAPP_UNITS_MECHANICS_H

#include "unit.h"
#include "units.h"

namespace TURAG {
namespace Units {

// Größe

/// Typ für Kraftvariablen
typedef Quantity< dim_div<EnergyDimension, LengthDimension> > Force;

/// Typ für Moment
typedef Quantity< dim_div<EnergyDimension, AngleDimension> > Torque;

/// Typ für Flächen
typedef Quantity< dim_mul<LengthDimension, LengthDimension> > Area;

/// Typ für Krümmung
typedef Quantity< dim_div<AngleDimension, LengthDimension> > Curvature;

/// Typen für Geschwindigkeit
typedef Quantity< dim_div<LengthDimension, TimeDimension> > Velocity;
typedef Quantity< dim_div<TimeDimension, LengthDimension> > InverseVelocity;

/// Typ für Winkelgeschwindigkeit
typedef Quantity< dim_div<AngleDimension, TimeDimension> > AngularVelocity;

/// Typen für Beschleunigung
typedef Quantity< dim_div<LengthDimension, QuadTimeDimension> > Acceleration;

/// Typ für Winkelbeschleunigung
typedef Quantity< dim_div<AngleDimension, QuadTimeDimension> > AngularAcceleration;

/// Typ für Motorkonstante
typedef decltype(Torque() / Current()) MotorConstant;

typedef decltype(Torque() / AngularAcceleration()) MomentOfInertia;

// Einheiten

/// Definition für Newton
constexpr Force::UnitType N(kg*m/(s*s));

/// Definition für Newtonmeter
constexpr Torque::UnitType Nm(N*m/rad);

/// Definition für RPM
constexpr AngularVelocity::UnitType rpm(round/minute);

} } // namespace TURAG::Units

#endif // TINAPP_UNITS_MECHANICS_H

