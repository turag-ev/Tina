#ifndef TINA_UNITS_UNIT_H
#define TINA_UNITS_UNIT_H

#include <cstddef>
#include <type_traits>
#include <cmath>

#include "config.h"
#include <tina/tina.h>

namespace TURAG {
namespace Units {

namespace detail {

// größer gemeinsamer Teiler
constexpr
unsigned gcd(unsigned n, unsigned m) {
  return (m == 0) ? n : gcd(m, n % m);
}

constexpr
unsigned abs(int a) {
  return (a < 0) ? -a : a;
}

template<int Z, unsigned N>
struct Rational {
  constexpr static int z() { return Z; }
  constexpr static unsigned n() { return N; }

  static_assert(N != 0, "interner Fehler: Nenner darf nicht Null sein!");
  static_assert(N > 0, "interner Fehler: Nenner darf nicht kleiner Null sein!");
};

template<typename Rtnl>
struct rational_cancel_down {
  constexpr static int gcd_ = gcd(abs(Rtnl::z()), Rtnl::n());
  typedef Rational<Rtnl::z() / gcd_, Rtnl::n() / gcd_> type;
};

typedef Rational<0,1> Null;
typedef Rational<1,1> One;

template<typename Rtnl, int n>
struct rational_mul {
  typedef typename rational_cancel_down<Rational<Rtnl::z() * n, Rtnl::n()> >::type
          type;
};

template<typename Rtnl, int n>
struct rational_div {
  typedef typename rational_cancel_down<Rational<Rtnl::z(), Rtnl::n() * n> >::type
          type;
};

template<typename LhsRtnl, typename RhsRtnl>
struct rational_add {
  typedef typename rational_cancel_down<Rational<LhsRtnl::z()*RhsRtnl::n() + RhsRtnl::z()*LhsRtnl::n(), LhsRtnl::n()*RhsRtnl::n()> >::type
          type;
};

template<typename LhsRtnl, typename RhsRtnl>
struct rational_sub {
  typedef typename rational_cancel_down<Rational<LhsRtnl::z()*RhsRtnl::n() - RhsRtnl::z()*LhsRtnl::n(), LhsRtnl::n()*RhsRtnl::n()> >::type
          type;
};

/*
template<typename LhsRtnl, typename RhsRtnl>
struct rational_mul {
  typedef typename rational_cancel_down<Rational<LhsRtnl::z * RhsRtnl::z, LhsRtnl::n * RhsRtnl::n> >::type
          type;
};

template<typename LhsRtnl, typename RhsRtnl>
struct rational_div {
  typedef typename rational_cancel_down<Rational<LhsRtnl::z / RhsRtnl::z, LhsRtnl::n / RhsRtnl::n> >::type
          type;
};*/

template<typename Length, typename Angle, typename Time>
struct Dimension {
  typedef Length length;
  typedef Angle angle;
  typedef Time time;
};

} // namespace detail

typedef detail::Dimension<detail::Null, detail::Null, detail::Null> DimensionlessDimension;
typedef detail::Dimension<detail::One,  detail::Null, detail::Null> LengthDimension;
typedef detail::Dimension<detail::Null, detail::One,  detail::Null> AngleDimension;
typedef detail::Dimension<detail::Null, detail::Null, detail::One > TimeDimension;

template<typename LhsDimension, typename RhsDimension>
struct dim_mul {
  typedef detail::Dimension<typename detail::rational_add<typename LhsDimension::length, typename RhsDimension::length>::type,
                            typename detail::rational_add<typename LhsDimension::angle,  typename RhsDimension::angle>::type,
                            typename detail::rational_add<typename LhsDimension::time,   typename RhsDimension::time>::type>
          type;
};

template<typename LhsDimension, typename RhsDimension>
struct dim_div {
  typedef detail::Dimension<typename detail::rational_sub<typename LhsDimension::length, typename RhsDimension::length>::type,
                            typename detail::rational_sub<typename LhsDimension::angle,  typename RhsDimension::angle>::type,
                            typename detail::rational_sub<typename LhsDimension::time,   typename RhsDimension::time>::type>
          type;
};

template<typename Dim, int n>
struct dim_root {
  typedef detail::Dimension<typename detail::rational_div<typename Dim::length, n>::type,
                            typename detail::rational_div<typename Dim::angle,  n>::type,
                            typename detail::rational_div<typename Dim::time,   n>::type>
          type;
};

template<typename Dim, int n>
struct dim_pow {
  typedef detail::Dimension<typename detail::rational_mul<typename Dim::length, n>::type,
                            typename detail::rational_mul<typename Dim::angle,  n>::type,
                            typename detail::rational_mul<typename Dim::time,   n>::type>
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
  template<typename T, REQUIRES(std::is_floating_point<T>)> constexpr
  int roundToInt(T value) {
#if GCC_VERSION < 40700 || defined(ECOS)
    return (value < 0.0) ? ceil(value - 0.5) : floor(value + 0.5);
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
#if GCC_VERSION < 40700 || defined(ECOS)
    return (value < 0.0) ? ceil(value - 0.5) : floor(value + 0.5);
#else
    return ::lround(value);
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

  constexpr
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

#endif // TINA_UNITS_UNIT_H
