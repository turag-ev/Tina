#ifndef TINA_UNITS_MATH_H
#define TINA_UNITS_MATH_H

#include <cmath>

#include "units.h"

namespace Tina {

namespace Units {

constexpr Dimensionless pi = M_PI;
constexpr Angle angle_pi = M_PI * rad;

} // namespace Units

template<typename Dim> constexpr
Units::Quantity< typename Units::dim_pow<Dim, 2>::type >
sqr(Units::Quantity<Dim> arg) {
  return Units::Quantity< typename Units::dim_pow<Dim, 2>::type >
      (arg.value * arg.value);
}

template<typename Dim> constexpr
Units::Quantity< typename Units::dim_root<Dim, 2>::type >
sqrt(Units::Quantity<Dim> arg) {
  return Units::Quantity< typename Units::dim_root<Dim, 2>::type >
      (sqrt(arg.value));
}

template<typename Dim> constexpr
Units::Quantity<Dim>
abs(Units::Quantity<Dim> arg) {
  return Units::Quantity<Dim>(std::abs(arg.value));
}

constexpr
Units::Real sin(Units::Angle arg) {
  return Units::Dimensionless(__builtin_sinf(arg.to(Units::rad)));
}

constexpr
Units::Real cos(Units::Angle arg) {
  return Units::Dimensionless(__builtin_cosf(arg.to(Units::rad)));
}

constexpr
Units::Angle atan2(Units::Length y, Units::Length x) {
  return __builtin_atan2f(y.value, x.value) * Units::rad;
}

constexpr
Units::Length hypot(Units::Length x, Units::Length y) {
  return Units::Length(__builtin_hypotf(x.value, y.value));
}

} // namespace Tina

#endif // TINA_UNITS_MATH_H

