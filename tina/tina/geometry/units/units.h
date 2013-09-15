#ifndef TINA_UNITS_UNITS_H
#define TINA_UNITS_UNITS_H

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

} // namespace Units
} // namespace TURAG

#endif // TINA_UNITS_UNITS_H
