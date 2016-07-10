#ifndef TINAPP_UNITS_UNIT_H
#define TINAPP_UNITS_UNIT_H

#include <type_traits>
#include <ratio>

#include "../../math.h"
#include "config.h"
#include <tina/tina.h>

namespace TURAG {
namespace Units {

/// \addtogroup Units
/// \{

#ifndef __DOXYGEN__

typedef std::ratio<0> RationalNull;
typedef std::ratio<1> RationalOne;
typedef std::ratio<2> RationalTwo;

template<typename Length, typename Angle, typename Time, typename Mass, typename Current>
struct Dimension {
  typedef Length length;
  typedef Angle angle;
  typedef Time time;
  typedef Mass mass;
  typedef Current ampere;
};

typedef Dimension<RationalNull, RationalNull, RationalNull, RationalNull, RationalNull> DimensionlessDimension;
typedef Dimension<RationalOne,  RationalNull, RationalNull, RationalNull, RationalNull> LengthDimension;
typedef Dimension<RationalNull, RationalOne,  RationalNull, RationalNull, RationalNull> AngleDimension;
typedef Dimension<RationalNull, RationalNull, RationalOne , RationalNull, RationalNull> TimeDimension;
typedef Dimension<RationalNull, RationalNull, RationalTwo , RationalNull, RationalNull> QuadTimeDimension;
typedef Dimension<RationalNull, RationalNull, RationalNull, RationalOne , RationalNull> MassDimension;
typedef Dimension<RationalNull, RationalNull, RationalNull, RationalNull, RationalOne > CurrentDimension;
typedef Dimension<RationalOne, RationalNull, std::ratio<-2>, RationalOne, RationalNull> ForceDimension;
typedef Dimension<RationalTwo, RationalNull, std::ratio<-3>, RationalOne, std::ratio<-1>> VoltageDimension;

template<typename LhsDimension, typename RhsDimension>
using dim_mul =
    Dimension<
  		typename std::ratio_add<typename LhsDimension::length, typename RhsDimension::length>::type,
        typename std::ratio_add<typename LhsDimension::angle,  typename RhsDimension::angle>::type,
        typename std::ratio_add<typename LhsDimension::time,   typename RhsDimension::time>::type,
        typename std::ratio_add<typename LhsDimension::mass,   typename RhsDimension::mass>::type,
        typename std::ratio_add<typename LhsDimension::ampere, typename RhsDimension::ampere>::type
    >;

template<typename LhsDimension, typename RhsDimension>
using dim_div =
    Dimension<
  		typename std::ratio_subtract<typename LhsDimension::length, typename RhsDimension::length>::type,
        typename std::ratio_subtract<typename LhsDimension::angle,  typename RhsDimension::angle>::type,
        typename std::ratio_subtract<typename LhsDimension::time,   typename RhsDimension::time>::type,
        typename std::ratio_subtract<typename LhsDimension::mass,   typename RhsDimension::mass>::type,
        typename std::ratio_subtract<typename LhsDimension::ampere, typename RhsDimension::ampere>::type
    >;

template<typename Dim, typename N>
using dim_root =
    Dimension<
  		typename std::ratio_divide<typename Dim::length, N>::type,
        typename std::ratio_divide<typename Dim::angle,  N>::type,
        typename std::ratio_divide<typename Dim::time,   N>::type,
        typename std::ratio_divide<typename Dim::mass,   N>::type,
        typename std::ratio_divide<typename Dim::ampere, N>::type
    >;

template<typename Dim, typename N>
using dim_pow =
    Dimension<
  		typename std::ratio_multiply<typename Dim::length, N>::type,
        typename std::ratio_multiply<typename Dim::angle,  N>::type,
        typename std::ratio_multiply<typename Dim::time,   N>::type,
        typename std::ratio_multiply<typename Dim::mass,   N>::type,
        typename std::ratio_multiply<typename Dim::ampere, N>::type
    >;

#endif

/// Typ für Einheit
/// Konvertiert Zahl in Wert von interner Einheit, die für die Speicherung verwendet wird
template<typename Dim>
struct Unit {
public:

  /// Einheit aus Konvertierungsfaktor zu interner Einheit erstellen
  constexpr explicit
  Unit(UnitFactor fac) :
	factor(fac)
  { }

  /// Faktor zur Konvertierung von Zahl in Wert von interner Einheit
  const UnitFactor factor;
};

/// zwei Einheiten multipilieren
/// für Erstellung von Einheit
/// \code
/// 2 * Units::mm * Units::mm // ergibt 2 Quadradmillimeter
/// \endcode
template<typename Dim1, typename Dim2> constexpr
Unit<dim_mul<Dim1, Dim2>>
operator * (Unit<Dim1> a, Unit<Dim2> b)
{
  return Unit<dim_mul<Dim1, Dim2>>(a.factor * b.factor);
}

/// zwei Einheiten dividieren
/// für Erstellung von Einheit
/// \code
/// 2 * Units::m / Units::s // ergibt 2 Meter pro Sekunde
/// \endcode
template<typename Dim1, typename Dim2> constexpr
Unit<dim_div<Dim1, Dim2>>
operator / (Unit<Dim1> a, Unit<Dim2> b)
{
  return Unit<dim_div<Dim1, Dim2>>(a.factor / b.factor);
}

/// Typ um Besonderheiten der Null zu nutzen
/// Null ist Dimensionslos und lässt sich in jede Einheit ohne Operation umwandeln
/// Für Null ist damit Einheitenangabe überflüssig.
/// \code
/// void func(Length x);
///
/// // alles äquivalent:
/// func(0 * Units::mm);
/// func(0 * Units::cm);
/// func(0 * Units::m);
/// func(Units::null);
/// \endcode
constexpr struct null_t {

} null = {};

/// Tag für unsichere Funktionen
/// \internal
/// \warning nur für interne Nutzung
constexpr struct unsafe_t { } unsafe = { };

// roundToInt
#ifndef __DOXYGEN__
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
#endif

/// Typ für alle Variablen mit beliebiger Dimension
/// \tparam Dim Dimension der Variable
template <typename Dim>
struct Quantity {

  /// Dimension der Einheit
  typedef Dim DimensionType;

  /// Typ für Einheit
  typedef Unit<Dim> UnitType;

  /// Variable mit Einheit aus Zahl erstellen
  /// \warning nur verwenden wenn, man weiß man macht!
  /// \param val Zahl in Einheit der internen Darstellung
  template<typename T>
  constexpr explicit
  Quantity(T val) : value(detail::toValue(val)) { }

  /// Variable mit Einheit und Wert Null erstellen
  constexpr
  Quantity() :
	value(0)
  { }

  /// Variable Wert erstellen
  /// \internal
  /// \warning nur für interne Nutzung
  constexpr explicit
  Quantity(Value v, unsafe_t) :
	value(v)
  { }

  /// Variable mit Einheit und Wert Null erstellen
  constexpr
  Quantity(null_t) :
	value(0)
  { }

  /// Wert in Einheit \a u zurückgeben
  /// \warning Wird Rückgabewert in Ganzzahldarstellung (z.B. \a int) gespeichert
  /// Muss \ref toInt verwendet werden, da sonst Wert immer abgerundet wird.
  /// \param u Zieleinheit
  /// \returns Wert in Einheit \a u
  constexpr
  Real to(Unit<Dim> u) const {
	return value / u.factor;
  }

  /// gerundeter Ganzzahlwert in Einheit \a u zurückgeben
  /// \param u Zieleinheit
  /// \returns gerundeter Ganzzahlwert in Einheit \a u
  constexpr
  int toInt(Unit<Dim> u) const {
	return detail::roundToInt(value / u.factor);
  }

  ///
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

  const Quantity&
  operator *= (Value arg) {
	  value *= arg;
	  return *this;
  }

  const Quantity&
  operator /= (Value arg) {
	  value /= arg;
	  return *this;
  }

  /// Wert in Konfiguration festgelegter Einheit
  /// \warning nicht verwenden, außer man weiß was man tut!
  Value value;
};

// QuantityResultType ist für Dim != DimensionlessDimension: QuantityBase<Dim> sonst: Value
template <typename Dim>
using QuantityResultType = typename std::conditional<
	!std::is_same<Dim, DimensionlessDimension>(),
	Quantity<Dim>, Value>::type;

/// Einheit mit Zahl multipilizieren
///
/// Standardmethode um Variable mit Einheit zuerzeugen.
/// \code
/// Length x = 1 * Units::mm; // x entspricht einem Millimeter
/// \endcode
/// \returns Variable mit Einheit \a unit und Wert von \a value
template<typename Dim> constexpr
Quantity<Dim> operator * (Value value, Unit<Dim> unit)
{
  return Quantity<Dim>(value * unit.factor);
}

/// Einheit mit Zahl multipilizieren
/// \returns Variable mit Einheit \a unit und Wert von \a value
template<typename Dim> constexpr
Quantity<Dim> operator * (Unit<Dim> unit, Value value)
{
  return Quantity<Dim>(value * unit.factor);
}

/// Zahl durch Einheit dividieren
/// \returns Variable mit inverse Einheit \a unit und Wert von \a value
template<typename Dim> constexpr
Quantity<dim_div<DimensionlessDimension, Dim>>
operator / (Value value, Unit<Dim> unit)
{
  return Quantity<dim_div<DimensionlessDimension, Dim>>
	  (value / unit.factor);
}

/// Einheit durch Zahl dividieren
/// \returns Variable mit Einheit \a unit und inverserer Wert von \a value
template<typename Dim> constexpr
Quantity<Dim> operator / (Unit<Dim> unit, Value value)
{
  return Quantity<Dim> (unit.factor / value);
}

//


/// Verzeichen von Variable mit Einheit ändern
template<typename Dim> constexpr
Quantity<Dim> operator - (Quantity<Dim> arg)
{
  return Quantity<Dim> (-arg.value);
}

/// unärer Plusoperator für Variablen mit Einheit
template<typename Dim> constexpr
Quantity<Dim> operator + (Quantity<Dim> arg)
{
  return arg;
}

/// zwei Variablen mit Einheit addieren
template<typename Dim> constexpr
Quantity<Dim> operator + (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return Quantity<Dim> (lhs.value + rhs.value);
}

/// zwei Variablen mit Einheit subtrahieren
template<typename Dim> constexpr
Quantity<Dim> operator - (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return Quantity<Dim> (lhs.value - rhs.value);
}

/// zwei Variablen mit Einheit multiplizieren
template<typename Dim1, typename Dim2> constexpr
QuantityResultType<dim_mul<Dim1, Dim2>>
operator * (Quantity<Dim1> lhs, Quantity<Dim2> rhs)
{
  return QuantityResultType<dim_mul<Dim1, Dim2>> (lhs.value * rhs.value);
}

/// zwei Variablen mit Einheit dividieren
template<typename Dim1, typename Dim2> constexpr
QuantityResultType<dim_div<Dim1, Dim2>>
operator / (Quantity<Dim1> lhs, Quantity<Dim2> rhs)
{
  return QuantityResultType<dim_div<Dim1, Dim2>>(lhs.value / rhs.value);
}

/// zwei Variablen mit Einheit vergleichen
template<typename Dim> constexpr
bool operator <= (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value <= rhs.value;
}

/// zwei Variablen mit Einheit vergleichen
template<typename Dim> constexpr
bool operator >= (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value >= rhs.value;
}

/// zwei Variablen mit Einheit vergleichen
template<typename Dim> constexpr
bool operator == (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value == rhs.value;
}

/// zwei Variablen mit Einheit vergleichen
template<typename Dim> constexpr
bool operator != (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value != rhs.value;
}

/// zwei Variablen mit Einheit vergleichen
template<typename Dim> constexpr
bool operator <  (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value < rhs.value;
}

/// zwei Variablen mit Einheit vergleichen
template<typename Dim> constexpr
bool operator >  (Quantity<Dim> lhs, Quantity<Dim> rhs)
{
  return lhs.value > rhs.value;
}

/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator <= (Quantity<Dim> lhs, null_t) { return lhs.value <= 0.f; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator <= (null_t, Quantity<Dim> rhs) { return 0.f <= rhs.value; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator >= (Quantity<Dim> lhs, null_t) { return lhs.value >= 0.f; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator >= (null_t, Quantity<Dim> rhs) { return 0.f >= rhs.value; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator == (Quantity<Dim> lhs, null_t) { return lhs.value == 0.f; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator == (null_t, Quantity<Dim> rhs) { return 0.f == rhs.value; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator != (Quantity<Dim> lhs, null_t) { return lhs.value != 0.f; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator != (null_t, Quantity<Dim> rhs) { return 0.f != rhs.value; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator <  (Quantity<Dim> lhs, null_t) { return lhs.value < 0.f; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator <  (null_t, Quantity<Dim> rhs) { return 0.f < rhs.value; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator >  (Quantity<Dim> lhs, null_t) { return lhs.value > 0.f; }
/// Variable mit Einheit mit Null vergleichen
template<typename Dim> constexpr bool operator >  (null_t, Quantity<Dim> rhs) { return 0.f > rhs.value; }

/// Variable mit Einheit vervielfachen
template<typename Dim> constexpr
Quantity<Dim> operator * (Value lhs, Quantity<Dim> rhs)
{
  return Quantity<Dim>(lhs * rhs.value);
}

/// Variable mit Einheit mal Zahl multiplizieren
template<typename Dim> constexpr
Quantity<Dim> operator * (Quantity<Dim> lhs, Value rhs)
{
  return Quantity<Dim>(lhs.value * rhs);
}

/// Zahl durch Variable mit Einheit dividieren
template<typename Dim> constexpr
QuantityResultType<dim_div<DimensionlessDimension, Dim>>
operator / (Value lhs, Quantity<Dim> rhs)
{
  return QuantityResultType<dim_div<DimensionlessDimension, Dim>>
	  (lhs / rhs.value);
}

/// Variable mit Einheit durch Zahl dividieren
template<typename Dim> constexpr
Quantity<Dim> operator / (Quantity<Dim> lhs, Value rhs)
{
  return Quantity<Dim>(lhs.value / rhs);
}

////////////////////////////////////////////////////////////////////////////////
// Vordefinierte Dimensionen

/// Typ für Variablen ohne Einheit
typedef Value Dimensionless;

/// Typ für Längen
typedef Quantity<LengthDimension> Length;

/// Typ für Winkel
typedef Quantity<AngleDimension> Angle;

/// Typ für Zeitvariablen
typedef Quantity<TimeDimension> Time;

/// Typ für Massenvariablen
typedef Quantity<MassDimension> Mass;

/// Typ für Kraftvariablen
typedef Quantity<ForceDimension> Force;

/// Typ für Stromvariablen
typedef Quantity<CurrentDimension> Current;

/// Typ für Spannungsvariablen
typedef Quantity<VoltageDimension> Voltage;

/// Typ für Leistung
typedef decltype(Current() * Voltage()) Power;

/// Typ für Wiederstände
typedef decltype(Voltage() / Current()) Resistance;

/// Typ für Flächen
typedef Quantity< dim_mul<LengthDimension, LengthDimension> > Area;

/// Typen für Geschwindigkeit
typedef Quantity< dim_div<LengthDimension, TimeDimension> > Velocity;
typedef Quantity< dim_div<TimeDimension, LengthDimension> > InverseVelocity;

/// Typ für Winkelgeschwindigkeit
typedef Quantity< dim_div<AngleDimension, TimeDimension> > AngularVelocity;

/// Typen für Beschleunigung
typedef Quantity< dim_div<LengthDimension, QuadTimeDimension> > Acceleration;

/// Typ für Winkelbeschleunigung
typedef Quantity< dim_div<AngleDimension, QuadTimeDimension> > AngularAcceleration;

/// Typ für Moment
typedef Quantity<dim_mul<ForceDimension, LengthDimension>> Torque;

/// Typ für magmetische Feldstärke
typedef decltype(Mass() / (Current() * Time() * Time())) MagneticField;

// TODO: weitere Typen einführen

/// \}

} // namespace Units
} // namespace TURAG

#endif // TINAPP_UNITS_UNIT_H
