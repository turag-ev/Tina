#ifndef TINAPP_GEOMETRY_UNITS_UNITS_H
#define TINAPP_GEOMETRY_UNITS_UNITS_H

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
constexpr Unit<LengthDimension> mm(FROM_SI_CONVERT_FACTOR_LENGTH / 1000.f);

/// Definition für Centimeter
constexpr Unit<LengthDimension> cm(FROM_SI_CONVERT_FACTOR_LENGTH / 100.f);

/// Definition für Dezimeter
constexpr Unit<LengthDimension> dm(FROM_SI_CONVERT_FACTOR_LENGTH / 10.f);

/// Definition für Meter
constexpr Unit<LengthDimension>  m(FROM_SI_CONVERT_FACTOR_LENGTH);

/// Definition für Milliradiand
constexpr Unit<AngleDimension> mrad(FROM_SI_CONVERT_FACTOR_ANGLE / 1000.f);

/// Definition für Radiand (Pi rad = 180°)
constexpr Unit<AngleDimension>  rad(FROM_SI_CONVERT_FACTOR_ANGLE);

/// Definition für Grad (360° = 2 * Pi rad)
constexpr Unit<AngleDimension>  deg(FROM_SI_CONVERT_FACTOR_ANGLE * float(M_PI)/180.f);

/// Definition für Umdrehung (360°)
constexpr Unit<AngleDimension> round(FROM_SI_CONVERT_FACTOR_ANGLE * 2.f * float(M_PI));

/// Definition für Minuten
constexpr Unit<TimeDimension> minute(FROM_SI_CONVERT_FACTOR_TIME * 60.f);

/// Definition für Sekunden
constexpr Unit<TimeDimension>  s(FROM_SI_CONVERT_FACTOR_TIME);

/// Definition für Millisekunden
constexpr Unit<TimeDimension> ms(FROM_SI_CONVERT_FACTOR_TIME / 1000.f);

/// Definition für Mikrosekunden
constexpr Unit<TimeDimension> us(FROM_SI_CONVERT_FACTOR_TIME / 1000000.f);

/// Definition für Gramm
constexpr Mass::UnitType g(FROM_SI_CONVERT_FACTOR_MASS / 1000.f);

/// Definition für Kilogramm
constexpr Mass::UnitType kg(FROM_SI_CONVERT_FACTOR_MASS);

/// Definition für Ampere
constexpr Current::UnitType A(FROM_SI_CONVERT_FACTOR_CURRENT);

/// Definition für Milliampere
constexpr Current::UnitType mA(FROM_SI_CONVERT_FACTOR_CURRENT / 1000.f);

/// \}

} // namespace Units

#if __cplusplus >= 201103L

inline namespace Literals {
inline namespace UnitsLiterals {

/// \ingroup UnitsDefs
/// \{

constexpr Units::Length operator"" _mm(long double x) { return x * Units::mm; }
constexpr Units::Length operator"" _mm(long long unsigned x) { return x * Units::mm; }
constexpr Units::Length operator"" _cm(long double x) { return x * Units::cm; }
constexpr Units::Length operator"" _cm(long long unsigned x) { return x * Units::cm; }
constexpr Units::Length operator"" _dm(long double x) { return x * Units::dm; }
constexpr Units::Length operator"" _dm(long long unsigned x) { return x * Units::dm; }
constexpr Units::Length operator"" _m(long double x) { return x * Units::m; }
constexpr Units::Length operator"" _m(long long unsigned x) { return x * Units::m; }

constexpr Units::Angle operator"" _mrad(long double x) { return x * Units::mrad; }
constexpr Units::Angle operator"" _mrad(long long unsigned x) { return x * Units::mrad; }
constexpr Units::Angle operator"" _rad(long double x) { return x * Units::rad; }
constexpr Units::Angle operator"" _rad(long long unsigned x) { return x * Units::rad; }
constexpr Units::Angle operator"" _deg(long double x) { return x * Units::deg; }
constexpr Units::Angle operator"" _deg(long long unsigned x) { return x * Units::deg; }

/// \}

} // inline namespace UnitsLiterals
} // inline namespace Literals

namespace Units {
using namespace Literals::UnitsLiterals;
} // namespace Units

#endif

} // namespace TURAG

#endif // TINAPP_GEOMETRY_UNITS_UNITS_H
