#ifndef CIRCLE_H
#define CIRCLE_H

#include <array>

#include "geometry.h"

namespace TURAG {

/// Darstellung für Kreis aus Mittelpunkt und Radius
struct Circle {
	/// Kreis aus kartesischen Punkt und Radius erstellen
	/// \tparam Typ mit x- und y-Koordinate
	template<typename P>
	constexpr explicit
	Circle(const P& t, Length r_) :
		m(t.x, t.y), r(r_)
	{ }

	/// unintialisierten Kreis erstellen (Radius ist negativ)
	constexpr explicit
	Circle() :
		m(), r(-1.f*Units::mm)
	{ }

	constexpr Point getCenter() const { return m; }

	/// Mittelpunkt des Kreies
	Point m;

	/// Radius des Kreises
	Length r;
};

/// Schnittpunkte zwischen zwei Kreisen zurückgeben.
/// \relates Circle
///
/// \param one,two Kreise
/// \param[out] results Schnittpunkte, wenn Rückgabewert \a true
///
/// \retval true es existiert endliche Anzahl an Schnittpunkten, die in \a results gespeichert wird.
/// \retval false es existiert kein Schnittpunkt zwischen den beiden Kreisen oder beide Kreise sind gleich (unendlich viele Schnittpunkte)
bool intersect(const Circle& one, const Circle& two, std::array<Point, 2>& results);

_always_inline
bool in_range(const Circle& one, const Circle& two, Length radius) {
	return in_range(one.m, two.m, one.r + two.r + radius);
}

_always_inline
bool in_range(const Pose& one, const Circle& two, Length radius) {
	return in_range(one, two.m, two.r + radius);
}
_always_inline
bool in_range(const Circle& one, const Pose& two, Length radius) {
	return in_range(two, one, radius);
}

_always_inline
bool in_range(const Point& one, const Circle& two, Length radius) {
	return in_range(one, two.m, two.r + radius);
}
_always_inline
bool in_range(const Circle& one, const Point& two, Length radius) {
	return in_range(two, one, radius);
}

} // namespace TURAG

#endif // CIRCLE_H
