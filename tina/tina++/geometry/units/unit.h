#ifndef TINAPP_UNITS_UNIT_H
#define TINAPP_UNITS_UNIT_H

#include <cstddef>
#include <type_traits>
#include <ratio>

#include "../../math.h"
#include "config.h"
#include <tina/tina.h>

namespace TURAG {
namespace Units {

typedef std::ratio<0> RationalNull;
typedef std::ratio<1> RationalOne;

template<typename Length, typename Angle, typename Time>
struct Dimension {
  typedef Length length;
  typedef Angle angle;
  typedef Time time;
};

typedef Dimension<RationalNull, RationalNull, RationalNull> DimensionlessDimension;
typedef Dimension<RationalOne,  RationalNull, RationalNull> LengthDimension;
typedef Dimension<RationalNull, RationalOne,  RationalNull> AngleDimension;
typedef Dimension<RationalNull, RationalNull, RationalOne > TimeDimension;

template<typename LhsDimension, typename RhsDimension>
struct dim_mul {
  typedef Dimension<typename std::ratio_add<typename LhsDimension::length, typename RhsDimension::length>::type,
                            typename std::ratio_add<typename LhsDimension::angle,  typename RhsDimension::angle>::type,
                            typename std::ratio_add<typename LhsDimension::time,   typename RhsDimension::time>::type>
          type;
};

template<typename LhsDimension, typename RhsDimension>
struct dim_div {
  typedef Dimension<typename std::ratio_subtract<typename LhsDimension::length, typename RhsDimension::length>::type,
                            typename std::ratio_subtract<typename LhsDimension::angle,  typename RhsDimension::angle>::type,
                            typename std::ratio_subtract<typename LhsDimension::time,   typename RhsDimension::time>::type>
          type;
};

template<typename Dim, typename N>
struct dim_root {
  typedef Dimension<typename std::ratio_divide<typename Dim::length, N>::type,
                            typename std::ratio_divide<typename Dim::angle,  N>::type,
                            typename std::ratio_divide<typename Dim::time,   N>::type>
          type;
};

template<typename Dim, typename N>
struct dim_pow {
  typedef Dimension<typename std::ratio_multiply<typename Dim::length, N>::type,
                            typename std::ratio_multiply<typename Dim::angle,  N>::type,
                            typename std::ratio_multiply<typename Dim::time,   N>::type>
          type;
};

template<typename Dim>
struct Unit {
public:
  constexpr explicit
  Unit() :
    factor(1.f)
  { }

  constexpr explicit
  Unit(UnitFactor fac) :
    factor(fac)
  { }

  const UnitFactor factor;
};

template<typename Dim1, typename Dim2> constexpr
Unit<typename dim_mul<Dim1, Dim2>::type>
operator * (Unit<Dim1> a, Unit<Dim2> b)
{
  return Unit<typename dim_mul<Dim1, Dim2>::type>(a.factor * b.factor);
}

template<typename Dim1, typename Dim2> constexpr
Unit<typename dim_div<Dim1, Dim2>::type>
operator / (Unit<Dim1> a, Unit<Dim2> b)
{
  return Unit<typename dim_div<Dim1, Dim2>::type>(a.factor / b.factor);
}

constexpr struct null_t {

} null = {};

// roundToInt
namespace detail {
  template<typename T, REQUIRES(std::is_floating_point<T>)> math_constexpr _always_inline
  int roundToInt(T value) {
#if GCC_VERSION < 40700 && !defined(ECOS)
    return (value < 0.0f) ? ceil(value - 0.5f) : floor(value + 0.5f);
#else
    return ::lround(value);
#endif
  }
  
  template<typename T, REQUIRES(!std::is_floating_point<T>)> constexpr
  int roundToInt(T value) {
    return value;
  }
}

// toValue
namespace detail {
  template<typename T, REQUIRES2(!std::is_floating_point<T>::value || std::is_floating_point<Value>::value)>
  constexpr
  Value toValue(T value) {
    return value;
  }
  
  template<typename T, REQUIRES2(std::is_floating_point<T>::value && !std::is_floating_point<Value>::value)>
  constexpr
  Value toValue(T value) {
#if GCC_VERSION < 40700 && !defined(ECOS)
    return (value < 0.0) ? ceil(value - 0.5) : floor(value + 0.5);
#else
    return ::lroundf(value);
#endif
  }
}

template <typename Dim>
struct Quantity {
  typedef Dim dimension;
  typedef Unit<Dim> unit;

  template<typename T>
  constexpr explicit
  Quantity(T val) : value(detail::toValue(val)) { }

  constexpr
  Quantity() :
    value(0)
  { }

  constexpr
  Quantity(null_t) :
    value(0)
  { }

  constexpr
  Real to(Unit<Dim> u) const {
    return value / u.factor;
  }

  constexpr
  int toInt(Unit<Dim> u) const {
    return detail::roundToInt(value / u.factor);
  }

  const Quantity&
  operator += (Quantity arg) {
    value += arg.value;
    return *this;
  }

  const Quantity&
  operator -= (Quantity arg) {
    value -= arg.value;
    return *this;
  }
  
  Value value;
};

template <>
struct Quantity<DimensionlessDimension> {
  typedef DimensionlessDimension dimension;
  typedef Unit<DimensionlessDimension> unit;

  template<typename T>
  constexpr
  Quantity(T val) : value(detail::toValue(val)) { }

  constexpr
  Quantity() :
    value(0)
  { }

  constexpr
  Quantity(null_t) :
    value(0)
  { }

  constexpr
  Real to(Unit<DimensionlessDimension> u) const {
    return value / u.factor;
  }

  math_constexpr
  int toInt(Unit<DimensionlessDimension> u) const {
    return detail::roundToInt(value / u.factor);
  }

  constexpr
  operator Value () const {
    return value;
  }

  Quantity<DimensionlessDimension>
  operator += (Quantity<DimensionlessDimension> arg) {
    value += arg.value;
    return *this;
  }

  Quantity<DimensionlessDimension>
  operator -= (Quantity<DimensionlessDimension> arg) {
    value -= arg.value;
    return *this;
  }

  Value value;
};

// conversion from standard value to value with unit
template<typename Dim> constexpr
Quantity<Dim> operator * (Value value, Unit<Dim> unit)
{
  return Quantity<Dim>(value * unit.factor);
}

template<typename Dim> constexpr
Quantity<Dim> operator * (Unit<Dim> unit, Value value)
{
  return Quantity<Dim>(value * unit.factor);
}

template<typename Dim> constexpr
Quantity<typename dim_div<DimensionlessDimension, Dim>::type>
operator / (Value value, Unit<Dim> unit)
{
  return Quantity<typename dim_div<DimensionlessDimension, Dim>::type>
      (value / unit.factor);
}

template<typename Dim> constexpr
Quantity<Dim> operator / (Unit<Dim> unit, Value value)
{
  return Quantity<Dim> (unit.factor / value);
}

//

template<typename Dim> constexpr
Quantity<Dim> operator - (Quantity<Dim> arg)
{
  return Quantity<Dim> (-arg.value);
}

template<typename Dim> constexpr
Quantity<Dim> operator + (Quantity<Dim> arg)
{
  return arg;
}

template<typename Dim> constexpr
Quantity<Dim> operator + (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return Quantity<Dim> (lhs.value + rhs.value);
}

template<typename Dim> constexpr
Quantity<Dim> operator - (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return Quantity<Dim> (lhs.value - rhs.value);
}

template<typename Dim1, typename Dim2> constexpr
Quantity<typename dim_mul<Dim1, Dim2>::type>
operator * (Quantity<Dim1> lhs, Quantity<Dim2> rhs)
{
  return Quantity<typename dim_mul<Dim1, Dim2>::type> (lhs.value * rhs.value);
}

template<typename Dim1, typename Dim2> constexpr
Quantity<typename dim_div<Dim1, Dim2>::type>
operator / (Quantity<Dim1> lhs, Quantity<Dim2> rhs)
{
  return Quantity<typename dim_div<Dim1, Dim2>::type>(lhs.value / rhs.value);
}

template<typename Dim> constexpr
bool operator <= (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value <= rhs.value;
}

template<typename Dim> constexpr
bool operator >= (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value >= rhs.value;
}

template<typename Dim> constexpr
bool operator == (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value == rhs.value;
}

template<typename Dim> constexpr
bool operator != (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value != rhs.value;
}

template<typename Dim> constexpr
bool operator <  (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value < rhs.value;
}

template<typename Dim> constexpr
bool operator >  (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value > rhs.value;
}

template<typename Dim> constexpr bool operator <= (Quantity<Dim> lhs, null_t) { return lhs.value <= 0.f; }
template<typename Dim> constexpr bool operator <= (null_t, Quantity<Dim> rhs) { return 0.f <= rhs.value; }
template<typename Dim> constexpr bool operator >= (Quantity<Dim> lhs, null_t) { return lhs.value >= 0.f; }
template<typename Dim> constexpr bool operator >= (null_t, Quantity<Dim> rhs) { return 0.f >= rhs.value; }
template<typename Dim> constexpr bool operator == (Quantity<Dim> lhs, null_t) { return lhs.value == 0.f; }
template<typename Dim> constexpr bool operator == (null_t, Quantity<Dim> rhs) { return 0.f == rhs.value; }
template<typename Dim> constexpr bool operator != (Quantity<Dim> lhs, null_t) { return lhs.value != 0.f; }
template<typename Dim> constexpr bool operator != (null_t, Quantity<Dim> rhs) { return 0.f != rhs.value; }
template<typename Dim> constexpr bool operator <  (Quantity<Dim> lhs, null_t) { return lhs.value < 0.f; }
template<typename Dim> constexpr bool operator <  (null_t, Quantity<Dim> rhs) { return 0.f < rhs.value; }
template<typename Dim> constexpr bool operator >  (Quantity<Dim> lhs, null_t) { return lhs.value > 0.f; }
template<typename Dim> constexpr bool operator >  (null_t, Quantity<Dim> rhs) { return 0.f > rhs.value; }

template<typename Dim> constexpr
Quantity<Dim> operator * (Value lhs, Quantity<Dim> rhs)
{
  return Quantity<Dim>(lhs * rhs.value);
}

template<typename Dim> constexpr
Quantity<Dim> operator * (Quantity<Dim> lhs, Value rhs)
{
  return Quantity<Dim>(lhs.value * rhs);
}

template<typename Dim> constexpr
Quantity<typename dim_div<DimensionlessDimension, Dim>::type>
operator / (Value lhs, Quantity<Dim> rhs)
{
  return Quantity<typename dim_div<DimensionlessDimension, Dim>::type>
      (lhs / rhs.value);
}

template<typename Dim> constexpr
Quantity<Dim> operator / (Quantity<Dim> lhs, Value rhs)
{
  return Quantity<Dim>(lhs.value / rhs);
}

////////////////////////////////////////////////////////////////////////////////
// Vordefinierte Dimensionen

typedef Quantity<DimensionlessDimension> Dimensionless;
typedef Quantity<LengthDimension> Length;
typedef Quantity<AngleDimension> Angle;
typedef Quantity<TimeDimension> Time;

typedef Quantity< typename dim_mul<LengthDimension, LengthDimension>::type > Area;
typedef Quantity< typename dim_div<LengthDimension, TimeDimension>::type > Velocity;
typedef Quantity< typename dim_div<AngleDimension, TimeDimension>::type > AngularVelocity;

} // namespace Units
} // namespace TURAG

#endif // TINAPP_UNITS_UNIT_H
