#ifndef TINAPP_UNITS_MATH_H
#define TINAPP_UNITS_MATH_H

#include "../../math.h"
#include "units.h"

namespace TURAG {

namespace Units {

constexpr Dimensionless pi = M_PI;
constexpr Angle angle_pi = M_PI * rad;

} // namespace Units

template<typename Dim> constexpr _always_inline
Units::Quantity< typename Units::dim_pow<Dim, 2>::type >
sqr(Units::Quantity<Dim> arg) {
  return Units::Quantity< typename Units::dim_pow<Dim, 2>::type >
      (arg.value * arg.value);
}

template<typename Dim> math_constexpr _always_inline
Units::Quantity< typename Units::dim_root<Dim, 2>::type >
sqrt(Units::Quantity<Dim> arg) {
  return Units::Quantity< typename Units::dim_root<Dim, 2>::type >
      (std::sqrt(arg.value));
}

template<typename Dim> constexpr _always_inline
Units::Quantity<Dim>
abs(Units::Quantity<Dim> arg) {
  return Units::Quantity<Dim>(std::fabs(arg.value));
}

math_constexpr _always_inline
Units::Real sin(Units::Angle arg) {
  return std::sin(arg.to(Units::rad));
}

math_constexpr _always_inline
Units::Real cos(Units::Angle arg) {
  return std::cos(arg.to(Units::rad));
}

math_constexpr _always_inline
Units::Angle atan2(Units::Length y, Units::Length x) {
  return std::atan2(y.value, x.value) * Units::rad;
}

math_constexpr _always_inline
Units::Length hypot(Units::Length x, Units::Length y) {
  return Units::Length(::hypot(x.value, y.value));
}

} // namespace TURAG

#endif // TINAPP_UNITS_MATH_H

