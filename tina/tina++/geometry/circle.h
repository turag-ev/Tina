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
	Circle(const P& t, Length r) :
		m(t.x, t.y), r(r)
	{ }

	/// unintialisierten Kreis erstellen (Radius ist negativ)
	constexpr explicit
	Circle() :
		m(), r(-1.f*Units::mm)
	{ }

	/// Kopierkonstruktor
	constexpr Circle(const Circle&) = default;

	/// Kopierkonstruktor
	Circle& operator=(const Circle&) = default;

	/// Mittelpunkt des Kreies
	Point m;

	/// Radius des Kreises
	Length r;
};

/// Schnittpunkte zwischen zwei Kreisen zurückgeben.
/// \param one,two Kreise
/// \param[out] results Schnittpunkte, wenn Rückgabewert \a true
///
/// \retval true es existiert endliche Anzahl an Schnittpunkten, die in \a results gespeichert wird.
/// \retval false es existiert kein Schnittpunkt zwischen den beiden Kreisen oder beide Kreise sind gleich (unendlich viele Schnittpunkte)
bool intersect(Circle one, Circle two, std::array<Point, 2>& results);

} // namespace TURAG

#endif // CIRCLE_H
