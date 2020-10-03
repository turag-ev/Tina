#ifndef TINAPP_MATH_H
#define TINAPP_MATH_H

#include <tina/math.h>
#include <cmath>
#include <limits>
#include "tina.h"

namespace TURAG {

/// \defgroup Math Mathematik [C++]
/// \{

/// Quadrat bilden
template<typename T>
constexpr TURAG_ALWAYS_INLINE
T sqr(T x) {
  return x * x;
}

/// Würfel bilden
template<typename T>
constexpr TURAG_ALWAYS_INLINE
T cbc(T x) {
  return x * x * x;
}

/// \brief \c sgn Funktion
/// \param x Wert
/// \retval 1.0f \f$ x > 0 \f$
/// \retval 0.0f \f$ x = 0 \f$
/// \retval -1.0f \f$ x < 0 \f$
template<typename T>
constexpr TURAG_ALWAYS_INLINE
int sgn(T x) {
  return (x > T(0)) ? 1 : (x < T(0)) ? -1 : 0;
}

/// \brief Gibt die Dreieckszahl für n zurück.
/// https://de.wikipedia.org/wiki/Dreieckszahl
constexpr TURAG_ALWAYS_INLINE
unsigned int triangular_number(unsigned int n) {
  return (n*(n+1))/2;
}

/// \brief Gibt den größten gemeinsamen Teiler von a und b zurück.
/// https://de.wikipedia.org/wiki/Gr%C3%B6%C3%9Fter_gemeinsamer_Teiler
constexpr
unsigned int gcd(int a, int b) {
  return (a % b == 0) ? b : gcd(a, a % b);
}

/// \brief silent not-a-number of float type
constexpr float NaNf = std::numeric_limits<float>::quiet_NaN();

/// \brief Prüfen ob zwei floats innerhalb der Toleranz Epsilon identisch sind
template<typename T>
TURAG_MATH_CONSTEXPR
bool float_equal(T a, T b) {
  return std::abs(a - b) < std::numeric_limits<T>::epsilon();
}

/// \brief Prüfen ob a >= b mit Berückrichtigung von Epsilon
template<typename T>
constexpr
bool float_gte(T a, T b) {
  return (a - b) > -std::numeric_limits<T>::epsilon();
}

/// \brief Prüfen ob a <= b mit Berückrichtigung von Epsilon
template<typename T>
constexpr
bool float_lte(T a, T b) {
  return (b - a) > -std::numeric_limits<T>::epsilon();
}

/// \brief x zwischen min und max begrenzen
template<typename T>
constexpr
T clamp(T x, T min, T max) {
  return x < min ? min : x > max ? max : x;
}
/// \}

} // namespace TURAG

#endif // TINAPP_MATH_H
