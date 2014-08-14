#ifndef TINAPP_UNITS_MATH_H
#define TINAPP_UNITS_MATH_H

#include "../../math.h"
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
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, std::ratio<2> >::type >
sqr(Units::Quantity<Dim> arg) {
    return Units::Quantity< typename Units::dim_pow<Dim, std::ratio<2> >::type >
            (arg.value * arg.value);
}

/// Quadratwurzel bilden
/// \param arg Variable mit Einheit
/// \returns \f$ \sqrt{arg} \f$
///
/// \code
/// sqrt(4.f * Units::mm * Units::mm) // entspricht 2 mm
/// \endcode
template<typename Dim> math_constexpr _always_inline
Units::Quantity<typename Units::dim_root<Dim, std::ratio<2> >::type>
sqrt(Units::Quantity<Dim> arg) {
    return Units::Quantity<typename Units::dim_root<Dim, std::ratio<2> >::type>
            (std::sqrt(arg.value));
}

#ifndef DOXYGEN

namespace detail {

// allgemeine Implementation über std::pow
template<typename Pow, REQUIRES2(
         !(Pow::den == 1 && (Pow::num <= 4 && Pow::num >= 0)) && // Spezialversionen vorhanden
         !(Pow::num == 1 && (Pow::num <= 3 && Pow::num >= 0))    // Spezialversionen vorhanden
         ), typename Dim>
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>
power_helper(Units::Quantity<Dim> arg) {
    return Units::Quantity<typename Units::dim_pow<Dim, Pow>::type >
            (std::pow(arg.value, Value(Pow::num) / Value(Pow::den)));
}

template<typename Pow, REQUIRES2(Pow::den == 1 && Pow::num == 0), typename Dim>
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>
power_helper(Units::Quantity<Dim> arg) {
  return Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>(
              arg != 0. ? 1. : std::pow(arg, arg));
}

template<typename Pow, REQUIRES2(Pow::den == 1 && Pow::num == 1), typename Dim>
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, Pow>::type >
power_helper(Units::Quantity<Dim> arg) {
  return arg;
}

template<typename Pow, REQUIRES2(Pow::den == 1 && Pow::num == 2), typename Dim>
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>
power_helper(Units::Quantity<Dim> arg) {
  return sqr(arg);
}

template<typename Pow, REQUIRES2(Pow::den == 1 && Pow::num == 3), typename Dim>
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>
power_helper(Units::Quantity<Dim> arg) {
  return Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>
      (arg.value * arg.value * arg.value);
}

template<typename Pow, REQUIRES2(Pow::den == 1 && Pow::num == 4), typename Dim>
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>
power_helper(Units::Quantity<Dim> arg) {
  return sqr(sqr(arg));
}

template<typename Pow, REQUIRES2(Pow::den == 2 && Pow::num == 1), typename Dim>
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>
power_helper(Units::Quantity<Dim> arg) {
  return sqrt(arg);
}

template<typename Pow, REQUIRES2(Pow::den == 3 && Pow::num == 1), typename Dim>
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, Pow>::type>
power_helper(Units::Quantity<Dim> arg) {
  return Units::Quantity< typename Units::dim_pow<Dim, Pow>::type >
          (std::cbrt(arg.value));
}

} // detail

#endif // DOXYGEN

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
constexpr _always_inline
Units::Quantity<typename Units::dim_pow<Dim, std::ratio<Num, Denom> >::type>
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
constexpr _always_inline
Units::Quantity< typename Units::dim_root<Dim, std::ratio<Num, Denom> >::type>
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
constexpr _always_inline
Units::Quantity<Dim>
abs(Units::Quantity<Dim> arg) {
    return Units::Quantity<Dim>(std::fabs(arg.value));
}

/// Sinus berechnen
/// \param arg Variable mit Einheit
/// \returns \f$ \sin(arg) \f$
math_constexpr _always_inline
Units::Real sin(Units::Angle arg) {
    return std::sin(arg.to(Units::rad));
}

/// Kosinus berechnen
/// \param arg Variable mit Einheit
/// \returns \f$ \cos(arg) \f$
math_constexpr _always_inline
Units::Real cos(Units::Angle arg) {
    return std::cos(arg.to(Units::rad));
}

/// Arkustangens von Quotient aus y durch x berechnen, Ergebnis liegt immer im Berech zwischen Pi und -Pi
/// \param y,x Variable mit Einheit
/// \returns \f$ \arctan( \frac{y}{x} ) \f$
/// \post Ergebnis liegt im Bereich \f$ [\pi, -\pi) \f$.
math_constexpr _always_inline
Units::Angle atan2(Units::Length y, Units::Length x) {
    return std::atan2(y.value, x.value) * Units::rad;
}

/// Hypotinuse berechnen
/// \param x,y andere Seiten des Dreiecks
/// \return \f$ \sqrt{x^2 + y^2} \f$
///
/// Ist sicher gegen Variablenüberlauf im Gegensatz zur manuellen Berechnung
math_constexpr _always_inline
Units::Length hypot(Units::Length x, Units::Length y) {
    return Units::Length(::hypot(x.value, y.value));
}

/// \}

} // namespace TURAG

#endif // TINAPP_UNITS_MATH_H

