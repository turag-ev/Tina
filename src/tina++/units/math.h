#ifndef TINAPP_UNITS_MATH_H
#define TINAPP_UNITS_MATH_H

#include "../math.h"
#include "units.h"

namespace TURAG {
namespace Units {

/// \defgroup UnitsConsts mathematische Konstanten
/// \brief mathematische Konstantanten mit zugehörigier Einheit
/// \ingroup Units
/// \{

/// Kreizahl Pi
constexpr Dimensionless pi = M_PI;

/// Pi für die Berechung von Winkeln
constexpr Angle angle_pi = M_PI * rad;

/// \}

} // namespace Units

/// \defgroup UnitsMath mathematische Funktionen
/// \ingroup Units
/// \{

/// Quadrat bilden
/// \param arg Variable mit Einheit
/// \returns \f$ arg^2 \f$
///
/// \code
/// sqr(4.f * Units::mm) // entspricht 8 mm
/// \endcode
template<typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, std::ratio<2> > >
sqr(Units::Quantity<Dim> arg) {
    return Units::Quantity< Units::dim_pow<Dim, std::ratio<2> > >
            (arg.value * arg.value);
}

/// Quadratwurzel bilden
/// \param arg Variable mit Einheit
/// \returns \f$ \sqrt{arg} \f$
///
/// \code
/// sqrt(4.f * Units::mm * Units::mm) // entspricht 2 mm
/// \endcode
template<typename Dim> TURAG_MATH_CONSTEXPR TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_root<Dim, std::ratio<2> >>
sqrt(Units::Quantity<Dim> arg) {
    return Units::Quantity<Units::dim_root<Dim, std::ratio<2> >>
            (std::sqrt(arg.value));
}

#ifndef __DOXYGEN__

namespace detail {

// allgemeine Implementation über std::pow
template<typename Pow, typename std::enable_if<(
         !(Pow::den == 1 && (Pow::num <= 4 && Pow::num >= 0)) && // Spezialversionen vorhanden
         !(Pow::num == 1 && (Pow::num <= 3 && Pow::num >= 0))    // Spezialversionen vorhanden
         ), bool>::type = false, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, Pow>>
power_helper(Units::Quantity<Dim> arg) {
    return Units::Quantity<Units::dim_pow<Dim, Pow> >
            (std::pow(arg.value, Value(Pow::num) / Value(Pow::den)));
}

template<typename Pow, typename std::enable_if<(Pow::den == 1 && Pow::num == 0), bool>::type = false, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, Pow>>
power_helper(Units::Quantity<Dim> arg) {
  return Units::Quantity<Units::dim_pow<Dim, Pow>>(
              arg != 0. ? 1. : std::pow(arg, arg));
}

template<typename Pow, typename std::enable_if<(Pow::den == 1 && Pow::num == 1), bool>::type = false, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, Pow> >
power_helper(Units::Quantity<Dim> arg) {
  return arg;
}

template<typename Pow, typename std::enable_if<(Pow::den == 1 && Pow::num == 2), bool>::type = false, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, Pow>>
power_helper(Units::Quantity<Dim> arg) {
  return sqr(arg);
}

template<typename Pow, typename std::enable_if<(Pow::den == 1 && Pow::num == 3), bool>::type = false, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, Pow>>
power_helper(Units::Quantity<Dim> arg) {
  return Units::Quantity<Units::dim_pow<Dim, Pow>>
      (arg.value * arg.value * arg.value);
}

template<typename Pow, typename std::enable_if<(Pow::den == 1 && Pow::num == 4), bool>::type = false, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, Pow>>
power_helper(Units::Quantity<Dim> arg) {
  return sqr(sqr(arg));
}

template<typename Pow, typename std::enable_if<(Pow::den == 2 && Pow::num == 1), bool>::type = false, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, Pow>>
power_helper(Units::Quantity<Dim> arg) {
  return sqrt(arg);
}

template<typename Pow, typename std::enable_if<(Pow::den == 3 && Pow::num == 1), bool>::type = false, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, Pow>>
power_helper(Units::Quantity<Dim> arg) {
  return Units::Quantity< Units::dim_pow<Dim, Pow> >
          (::cbrt(arg.value));
}

} // detail

#endif // __DOXYGEN__

/// Potenz bilden
/// \tparam Num Zähler von Potenz
/// \tparam Denom Nenner von Potenz, standardmäßig 1
/// \returns \f$ arg^{\frac{Num}{Denom}} \f$
///
/// \code
/// power<3>(3.f * Units::mm) // entspricht 27 mm^3
/// power<1,3>(27.f * Units::mm*Units::mm*Units::mm) // entspricht 3 mm
/// \endcode
template<std::intmax_t Num, std::intmax_t Denom = 1, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Units::dim_pow<Dim, std::ratio<Num, Denom> >>
power(Units::Quantity<Dim> arg) {
    return detail::power_helper<std::ratio<Num, Denom> >(arg);
}

/// Wurzel bilden
/// \tparam Num Zähler von Potenz der Wurzel
/// \tparam Denom Nenner von Potenz der Wurzel, standardmäßig 1
/// \returns \f$ \sqrt[\frac{Num}{Denom}]{arg} \f$
///
/// \code
/// root<3>(27.f * Units::mm * Units::mm * Units::mm) // entspricht 3 mm
/// root<1,3>(3.f * Units::mm) // entspricht 27 mm^3
/// \endcode
template<std::intmax_t Num, std::intmax_t Denom = 1, typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity< Units::dim_root<Dim, std::ratio<Num, Denom> >>
root(Units::Quantity<Dim> arg) {
    return detail::power_helper<std::ratio<Denom, Num> >(arg);
}

/// Absolutbetrag bilden
/// \param arg Variable mit Einheit
/// \returns \f$ |arg| \f$
///
/// \code
/// abs(-4.f * Units::mm) // entspricht 4 mm
/// \endcode
template<typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Dim>
abs(Units::Quantity<Dim> arg) {
    return Units::Quantity<Dim>(std::abs(arg.value));
}

/// Den größeren Wert zurückgeben
/// \param a Wert 1
/// \param b Wert 2
/// \returns \f$ max(a, b) \f$
///
/// \code
/// max(-4.f * Units::mm, 6.f * Units::mm) // entspricht 6 mm
/// \endcode
template<typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Dim>
max(Units::Quantity<Dim> a, Units::Quantity<Dim> b) {
	return Units::Quantity<Dim>((a.value > b.value) ? a.value : b.value);
}

/// Den kleineren Wert zurückgeben
/// \param a Wert 1
/// \param b Wert 2
/// \returns \f$ min(a, b) \f$
///
/// \code
/// min(-4.f * Units::mm, 6.f * Units::mm) // entspricht -4 mm
/// \endcode
template<typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Dim>
min(Units::Quantity<Dim> a, Units::Quantity<Dim> b) {
	return Units::Quantity<Dim>((a.value < b.value) ? a.value : b.value);
}

/// Den größeren Wert zurückgeben
/// \param val Wert
/// \param min_val minimaler Wert
/// \param max_val maximaler Wert
/// \returns \f$ min(max(val, min_val), max_val) \f$
///
/// \code
/// saturate(-4.f * Units::mm, -1.f * Units::mm, 6.f * Units::mm) // entspricht -1 mm
/// \endcode
template<typename Dim>
constexpr TURAG_ALWAYS_INLINE
Units::Quantity<Dim>
saturate(Units::Quantity<Dim> val, Units::Quantity<Dim> min_val, Units::Quantity<Dim> max_val) {
	return min(max(val, min_val), max_val);
}

/// \brief \c sgn Funktion
/// \param x Wert
/// \retval 1 \f$ x > 0 \f$
/// \retval 0 \f$ x = 0 \f$
/// \retval -1 \f$ x < 0 \f$
///
/// \code
/// sgn(-4.f * Units::mm) // entspricht -1
/// \endcode
template<typename Dim>
constexpr TURAG_ALWAYS_INLINE
int
sgn(Units::Quantity<Dim> x) {
	using namespace Units;
  return (x > null) ? 1 : (x < null) ? -1 : 0;
}

/// Sinus berechnen
/// \param arg Variable mit Einheit
/// \returns \f$ \sin(arg) \f$
TURAG_MATH_CONSTEXPR TURAG_ALWAYS_INLINE
Units::Real sin(Units::Angle arg) {
    return std::sin(arg.to(Units::rad));
}

/// Kosinus berechnen
/// \param arg Variable mit Einheit
/// \returns \f$ \cos(arg) \f$
TURAG_MATH_CONSTEXPR TURAG_ALWAYS_INLINE
Units::Real cos(Units::Angle arg) {
    return std::cos(arg.to(Units::rad));
}

/// Arkustangens von Quotient aus y durch x berechnen, Ergebnis liegt immer im Berech zwischen Pi und -Pi
/// \param y,x Variable mit Einheit
/// \returns \f$ \arctan( \frac{y}{x} ) \f$
/// \post Ergebnis liegt im Bereich \f$ [\pi, -\pi) \f$.
TURAG_MATH_CONSTEXPR TURAG_ALWAYS_INLINE
Units::Angle atan2(Units::Length y, Units::Length x) {
    return std::atan2(y.value, x.value) * Units::rad;
}

/// Hypotinuse berechnen
/// \param x,y andere Seiten des Dreiecks
/// \return \f$ \sqrt{x^2 + y^2} \f$
///
/// Ist sicher gegen Variablenüberlauf im Gegensatz zur manuellen Berechnung
TURAG_MATH_CONSTEXPR TURAG_ALWAYS_INLINE
Units::Length hypot(Units::Length x, Units::Length y) {
    return Units::Length(::hypot(x.value, y.value));
}

/// Is value not a Number (NaN)
template<typename Dim>
constexpr TURAG_ALWAYS_INLINE
bool isnan(Units::Quantity<Dim> arg) {
	return std::isnan(arg.value);
}

template<typename Dim>
TURAG_MATH_CONSTEXPR TURAG_ALWAYS_INLINE
bool isfinite(Units::Quantity<Dim> arg) {
    return std::isfinite(arg.value);
}

struct NaN {
	template<typename Dim>
	explicit operator Units::Quantity<Dim>() {
		return Units::Quantity<Dim>(std::numeric_limits<Units::Value>::quiet_NaN(), Units::unsafe);
	}
};

/// \}

} // namespace TURAG

#endif // TINAPP_UNITS_MATH_H

