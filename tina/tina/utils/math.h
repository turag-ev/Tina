#ifndef MATH_H
#define MATH_H

#include <cmath>
#include "../normalize.h"

namespace TURAG {

template<typename T>
constexpr _always_inline
T sqr(T x) {
  return x * x;
}

_always_inline constexpr
float sgn(float x) {
  return (x > 0.f) ? 1.0f : ((x == 0.f) ? 0.f : -1.0f);
}

//_always_inline constexpr
//float sgn(float x) {
//  return (x == 0.f) ? 0.f : std::copysign(1.0f, x);
//}

//constexpr _always_inline
//int hypot(int a, int b) {
//  return std::sqrt(sqr(a)+sqr(b));
//}

/// Gibt die Dreieckszahl für n zurück
/// https://de.wikipedia.org/wiki/Dreieckszahl
constexpr _always_inline
unsigned int triangular_number(unsigned int n) {
  return (n*(n+1))/2;
}

/// Gibt den größten gemeinsamen Teiler von a und b zurück
/// https://de.wikipedia.org/wiki/Gr%C3%B6%C3%9Fter_gemeinsamer_Teiler
constexpr
unsigned int gcd(int a, int b) {
  return (a % b == 0) ? b : gcd(a, a % b);
}

} // namespace TURAG

#endif // MATH_H
