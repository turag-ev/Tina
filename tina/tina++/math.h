#ifndef TINAPP_MATH_H
#define TINAPP_MATH_H

#include <tina/math.h>
#include <limits>
#include "tina.h"

namespace std {
using ::acos;
using ::atan;
using ::atan2;
using ::ceil;
using ::copysign;
using ::cos;
using ::fabs;
using ::floor;
using ::hypot;
using ::pow;
using ::sin;
using ::sqrt;
using ::tan;
using ::scalbn;
using ::lround;
using ::cbrt;
}

using ::acos;
using ::atan;
using ::atan2;
using ::ceil;
using ::copysign;
using ::cos;
using ::fabs;
using ::floor;
using ::hypot;
using ::pow;
using ::sin;
using ::sqrt;
using ::tan;
using ::scalbn;
using ::lround;

namespace TURAG {

/// \defgroup Math Mathematik [C++]
/// \{

/// Quadrat bilden
template<typename T>
constexpr _always_inline
T sqr(T x) {
  return x * x;
}

/// \brief \c sgn Funktion
/// \param x Wert
/// \retval 1.0f \f$ x > 0 \f$
/// \retval 0.0f \f$ x = 0 \f$
/// \retval -1.0f \f$ x < 0 \f$
_always_inline constexpr
float sgn(float x) {
  return (x > 0.f) ? 1.0f : ((x == 0.f) ? 0.f : -1.0f);
}

/// \brief Gibt die Dreieckszahl für n zurück.
/// https://de.wikipedia.org/wiki/Dreieckszahl
constexpr _always_inline
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
constexpr float NaNf = __builtin_nanf("");

/// \brief Prüfen ob zwei floats innerhalb der Toleranz Epsilon identisch sind
static inline bool float_equal(float a, float b) {
  return fabsf(a - b) < std::numeric_limits<float>::epsilon();
}

/// \brief Prüfen ob a >= b mit Berückrichtigung von Epsilon
constexpr
bool float_gte(float a, float b) {
  return static_cast<float>(a - b) > -std::numeric_limits<float>::epsilon();
}

/// \}

} // namespace TURAG

#endif // TINAPP_MATH_H
