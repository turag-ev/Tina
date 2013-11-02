#ifndef TINAPP_GEOMETRY_UNITS_UNITS_H
#define TINAPP_GEOMETRY_UNITS_UNITS_H

#include "config.h"
#include "unit.h"

namespace TURAG {
namespace Units {

constexpr Unit<LengthDimension> mm(FROM_SI_CONVERT_FACTOR_LENGTH / 1000.f);
constexpr Unit<LengthDimension> cm(FROM_SI_CONVERT_FACTOR_LENGTH / 100.f);
constexpr Unit<LengthDimension> dm(FROM_SI_CONVERT_FACTOR_LENGTH / 10.f);
constexpr Unit<LengthDimension>  m(FROM_SI_CONVERT_FACTOR_LENGTH);

constexpr Unit<AngleDimension> mrad(FROM_SI_CONVERT_FACTOR_ANGLE / 1000.f);
constexpr Unit<AngleDimension>  rad(FROM_SI_CONVERT_FACTOR_ANGLE);
constexpr Unit<AngleDimension>  deg(FROM_SI_CONVERT_FACTOR_ANGLE * M_PI/180.f);

constexpr Unit<TimeDimension>  s(FROM_SI_CONVERT_FACTOR_TIME);
constexpr Unit<TimeDimension> ms(FROM_SI_CONVERT_FACTOR_TIME / 1000.f);

#if GCC_VERSION > 40700 && defined(TINA_UNITS_LITERALS)
constexpr Length operator"" _mm(long double x) { return x * mm; }
constexpr Length operator"" _mm(long long unsigned x) { return x * mm; }
constexpr Length operator"" _cm(long double x) { return x * cm; }
constexpr Length operator"" _cm(long long unsigned x) { return x * cm; }
constexpr Length operator"" _dm(long double x) { return x * dm; }
constexpr Length operator"" _dm(long long unsigned x) { return x * dm; }
constexpr Length operator"" _m(long double x) { return x * m; }
constexpr Length operator"" _m(long long unsigned x) { return x * m; }

constexpr Angle operator"" _mrad(long double x) { return x * mrad; }
constexpr Angle operator"" _mrad(long long unsigned x) { return x * mrad; }
constexpr Angle operator"" _rad(long double x) { return x * rad; }
constexpr Angle operator"" _rad(long long unsigned x) { return x * rad; }
constexpr Angle operator"" _deg(long double x) { return x * deg; }
constexpr Angle operator"" _deg(long long unsigned x) { return x * deg; }
#endif

} // namespace Units
} // namespace TURAG

#endif // TINAPP_GEOMETRY_UNITS_UNITS_H
