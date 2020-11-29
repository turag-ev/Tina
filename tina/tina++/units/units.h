#ifndef TINAPP_UNITS_UNITS_H
#define TINAPP_UNITS_UNITS_H

#include "config.h"
#include "unit.h"

namespace TURAG {
namespace Units {

/// \defgroup UnitsDefs Einheitendefinitionen
/// \ingroup Units
/// \brief SI-Einheiten für die konkrette Anwendung
///
/// \{

/// Definition für Millimeter
constexpr Unit<LengthDimension> mm(FROM_SI_CONVERT_FACTOR_LENGTH / UnitFactor(1000));

/// Definition für Centimeter
constexpr Unit<LengthDimension> cm(FROM_SI_CONVERT_FACTOR_LENGTH / UnitFactor(100));

/// Definition für Dezimeter
constexpr Unit<LengthDimension> dm(FROM_SI_CONVERT_FACTOR_LENGTH / UnitFactor(10));

/// Definition für Meter
constexpr Unit<LengthDimension> m(FROM_SI_CONVERT_FACTOR_LENGTH);

/// Definition für Milliradiand
constexpr Unit<AngleDimension> mrad(FROM_SI_CONVERT_FACTOR_ANGLE / UnitFactor(1000));

/// Definition für Radiand (Pi rad = 180°)
constexpr Unit<AngleDimension> rad(FROM_SI_CONVERT_FACTOR_ANGLE);

/// Definition für Grad (360° = 2 * Pi rad)
constexpr Unit<AngleDimension> deg(FROM_SI_CONVERT_FACTOR_ANGLE* UnitFactor(M_PI) / UnitFactor(180));

/// Definition für Umdrehung (360°)
constexpr Unit<AngleDimension> round(FROM_SI_CONVERT_FACTOR_ANGLE* UnitFactor(2) * UnitFactor(M_PI));

/// Definition für Minuten
constexpr Unit<TimeDimension> minute(FROM_SI_CONVERT_FACTOR_TIME* UnitFactor(60));

/// Definition für Sekunden
constexpr Unit<TimeDimension> s(FROM_SI_CONVERT_FACTOR_TIME);

/// Definition für Millisekunden
constexpr Unit<TimeDimension> ms(FROM_SI_CONVERT_FACTOR_TIME / UnitFactor(1000));

/// Definition für Mikrosekunden
constexpr Unit<TimeDimension> us(FROM_SI_CONVERT_FACTOR_TIME / UnitFactor(1000000));

/// Definition für Gramm
constexpr Mass::UnitType g(FROM_SI_CONVERT_FACTOR_MASS / UnitFactor(1000));

/// Definition für Kilogramm
constexpr Mass::UnitType kg(FROM_SI_CONVERT_FACTOR_MASS);

/// Definition für Ampere
constexpr Current::UnitType A(FROM_SI_CONVERT_FACTOR_CURRENT);

/// Definition für Milliampere
constexpr Current::UnitType mA(FROM_SI_CONVERT_FACTOR_CURRENT / UnitFactor(1000));

/// \}

} // namespace Units

inline namespace Literals {
inline namespace UnitsLiterals {

/// \ingroup UnitsDefs
/// \{

constexpr Units::Length operator""_mm(long double x) {
    return x * Units::mm;
}
constexpr Units::Length operator""_mm(long long unsigned x) {
    return x * Units::mm;
}
constexpr Units::Length operator""_cm(long double x) {
    return x * Units::cm;
}
constexpr Units::Length operator""_cm(long long unsigned x) {
    return x * Units::cm;
}
constexpr Units::Length operator""_dm(long double x) {
    return x * Units::dm;
}
constexpr Units::Length operator""_dm(long long unsigned x) {
    return x * Units::dm;
}
constexpr Units::Length operator""_m(long double x) {
    return x * Units::m;
}
constexpr Units::Length operator""_m(long long unsigned x) {
    return x * Units::m;
}

constexpr Units::Angle operator""_mrad(long double x) {
    return x * Units::mrad;
}
constexpr Units::Angle operator""_mrad(long long unsigned x) {
    return x * Units::mrad;
}
constexpr Units::Angle operator""_rad(long double x) {
    return x * Units::rad;
}
constexpr Units::Angle operator""_rad(long long unsigned x) {
    return x * Units::rad;
}
constexpr Units::Angle operator""_deg(long double x) {
    return x * Units::deg;
}
constexpr Units::Angle operator""_deg(long long unsigned x) {
    return x * Units::deg;
}

constexpr Units::Mass operator""_g(long double x) {
    return x * Units::g;
}
constexpr Units::Mass operator""_kg(long long unsigned x) {
    return x * Units::kg;
}
constexpr Units::Current operator""_A(long double x) {
    return x * Units::A;
}
constexpr Units::Current operator""_mA(long long unsigned x) {
    return x * Units::mA;
}
/// \}

} // namespace UnitsLiterals
} // namespace Literals

namespace Units {
using namespace Literals::UnitsLiterals;
} // namespace Units

} // namespace TURAG

#endif // TINAPP_UNITS_UNITS_H
