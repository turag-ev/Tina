#ifndef TINAPP_GEOMETRY_GEOMETRY_H
#define TINAPP_GEOMETRY_GEOMETRY_H

#include <array>

#include <tina/tina.h>

#include "units.h"

namespace TURAG {

/// \defgroup Geometry Geometrie [C++]
/// \{

/// \brief Typ für Variablen, die Winkel darstellen.
///
/// Benutzt \ref Units::Unit für einheitenrichtiges Rechnen und Konvertieren.
typedef Units::Angle Angle;

/// \brief Typ für Variablen, die Längen darstellen.
///
/// Benutzt \ref Units::Unit für einheitenrichtiges Rechnen und Konvertieren.
typedef Units::Length Length;

/// Normiert einen Winkel auf das Intervall \f$ [\pi, -\pi) \f$
Angle norm_angle(Angle x);

/// Darstellung für zweidimensionalen, kartesischen Vektor
///
/// \tparam U Typ der Koordinaten (Standardmäßig \ref TURAG::Length)
template<typename U = Length>
struct Vector {
	/// x - Koordinate
	U x;

	/// y - Koordinate
	U y;

	/// Vektor aus Komponeten erstellen
	/// \param x,y Koordinaten
	constexpr
	Vector(U x, U y) :
		x(x), y(y)
	{ }

    /// Vektor skalieren
    /// \param Faktor
    constexpr
    Vector operator*(float rhs) {
        return Vector<U>(x*rhs, y*rhs);
    }

	/// \{
	/// Kopieroperator
	Vector(const Vector& p) = default;
	Vector() = default;
	/// \}
    ///
};

/// zweidimensionaler kartesischer Punkt
struct Point {
	/// x - Koordinate
	Length x;

	/// y - Koordinate
	Length y;

	/// Punkt aus zwei kartesischen Korrdinaten erstellen
	constexpr
	Point(Length x, Length y) :
		x(x), y(y)
	{ }

	/// Kopierkonstruktor
	constexpr
	Point(const Point& p) = default;

	/// Nullpunkt erstellen
	constexpr
	Point() :
		x(0), y(0)
	{ }

	/// zwei Punkte addieren
	constexpr
	Vector<Length> operator+(const Point& other) {
		return Vector<Length>(x + other.x, y + other.y);
	}
    /// Punkt und Vektor addieren (nur Punkt+Vektor, nicht Vektor+Punkt)
    constexpr
    Point operator+(const Vector<Length>& rhs) {
        return Point(x + rhs.x, y + rhs.y);
    }

	/// relativer Vektor zwischen zwei Punkte erhalten
	constexpr
	Vector<Length> operator-(const Point& other) {
		return Vector<Length>(x - other.x, y - other.y);
	}

	/// zwei Punkte vergleichen
	constexpr
	bool operator!=(const Point& other) {
		return (x != other.x) || (y != other.y);
	}
};

/// Darstellung für zweidimensionalen Polarvektor
///
/// \tparam U1 Typ des Radius (Standardmäßig \ref TURAG::Length)
/// \tparam U2 Typ des Winkels (Standardmäßig \ref TURAG::Angle)
template<typename U1 = Length, typename U2 = Angle>
struct VectorPolar {
	/// Radius - Koordinate
	U1 r;

	/// Winkel - Koordinate
	U2 phi;

	/// Punkt aus zwei Polarkorrdinaten erstellen
	constexpr
	VectorPolar(U1 r, U2 phi) :
		r(r), phi(phi)
	{ }

	/// Kopierkonstruktor
	constexpr
	VectorPolar(const VectorPolar& p) = default;

	/// Nullvektor erstellen
	constexpr
	VectorPolar() :
		r(0), phi(0)
	{ }
};

/// \brief Pose im zweidimensionalen Raum
///
/// zweidimensionaler kartesischer Punkt mit Winkel, der Ausrichtung darstellt.
struct Pose {
	/// x - Koordinate
	Length x;

	/// y - Koordinate
	Length y;

	/// Winkel - Koordinate
	Angle phi;

	/// Pose aus zwei kartesischen Korrdinaten und Winkel erstellen
	constexpr
	Pose(Length x, Length y, Angle phi) :
		x(x), y(y), phi(phi)
	{ }

	/// Kopierkonstruktor
	constexpr Pose(const Pose&) = default;

	/// Kopierkonstruktor
	Pose& operator=(const Pose&) = default;

	/// Nullpose erstellen
	constexpr
	Pose() :
		x(0), y(0), phi(0)
	{ }

	/// Pose aus kartesischen Punkt und Winkel erstellen
	explicit constexpr
	Pose(const Point& p, Angle a = Units::null) :
		x(p.x), y(p.y), phi(a)
	{ }

	/// Position aus kartesischen Punkt übernehmen. Winkel wird nicht geändert.
	void assign(const Point& p) {
		x = p.x;
		y = p.y;
	}

	/// zugehörigen Punkt zurück geben
	constexpr Point toPoint() const {
		return Point(x, y);
	}

	/// Pose um sich selber drehen (Winkel wird normalisiert).
	Pose& turn(Angle a) {
		phi = norm_angle(phi + a);
		return *this;
	}

	/// Pose in Blickrichtung verschieben
	/// \param trans Länge, um den die Position in Winkelrichtung verschoben werden soll.
	Pose& translate(Length trans);

	/// Pose in Blickrichtung verschieben
	/// \param other Vektor, um den die Position in Winkelrichtung verschoben werden soll.
	Pose& translate(const Point& other);

	/// Pose in Blickrichtung verschieben und drehen
	/// \param other
	///    Vektor, um den die Position in Winkelrichtung verschoben werden soll,
	///    und Winkel um den gedreht werden soll.
	Pose& translate(const Pose& other);

	/// neue Pose mit zusätzlichen Winkel erstellen
	Pose getTurnedPose(Angle a) const {
		return Pose(x, y, norm_angle(phi + a));
	}

	/// neuen in Blickrichtung verschobenen Punkt erstellen
	/// \param trans Länge, um den die Position in Winkelrichtung verschoben ist.
	Point getRelativePoint(Length trans) const;

	/// neue in Blickrichtung verschobene Punkt erstellen
	/// \param other Vektor, um den die Position in Winkelrichtung verschoben ist.
	Point getRelativePoint(const Point& other) const;

	/// neue in Blickrichtung verschobene Punkt erstellen
	/// \param other Vektor, um den die Position in Winkelrichtung verschoben ist (Winkel wird ignoriert).
	Point getRelativePoint(const Pose& other) const;


	/// neuen, in Blickrichtung verschobenen Punkt erstellen
	/// \param trans Länge, um den die Position in Winkelrichtung verschoben ist.
	constexpr Pose getRelativePose(Length trans) const {
		return Pose(x + trans * cos(phi), y + trans * sin(phi), phi);
	}

	/// neuen, in Blickrichtung verschobene Punkt erstellen
	/// \param other Vektor, um den die Position in Winkelrichtung verschoben ist.
	constexpr Pose getRelativePose(const Point& other) const {
		return Pose(x + other.x * cos(phi) - other.y * sin(phi),
					y + other.x * sin(phi) + other.y * cos(phi),
					phi);
	}

	/// neue in Blickrichtung verschobene Punkt erstellen
	/// \param other
	///    Vektor, um den die Position in Winkelrichtung verschoben ist,
	///    und Winkel um den gedreht wird.
	Pose getRelativePose(const Pose& other) const {
		return Pose(*this).translate(other);
	}
};

////////////////////////////////////////////////////////////////////////////////

/// zwei Posen vergleichen
///
/// \warning in realen Situationen sind zwei Positionen nur sehr selten gleich,
/// weil es immer zu Abweichungen kommt, deswegen sorgsam verwenden! Wenn zwei
/// Posen vergliechen werden sollen, ist fast immer \ref TURAG::in_range besser geeignet.
///
constexpr _always_inline
bool operator==(const Pose& l, const Pose& r) {
  return (l.x == r.x) && (l.y == r.y) && (l.phi == r.phi);
}

/// zwei Punkte vergleichen
///
/// \warning in realen Situationen sind zwei Positionen nur sehr selten gleich,
/// weil es immer zu Abweichungen kommt, deswegen sorgsam verwenden! Wenn zwei
/// Punkte vergliechen werden sollen, ist fast immer \ref TURAG::in_range besser geeignet.
///
constexpr _always_inline
bool operator==(const Point& l, const Point& r) {
  return (l.x == r.x) && (l.y == r.y);
}

////////////////////////////////////////////////////////////////////////////////
// algorithm

/// allgemeine Implementation um den quatratischen Abstand zwischen zwei
/// kartesischen Punkten zubestimmen.
///
/// \code{.cpp}
/// template<typename T1, typename T2>
/// Units::Area distance_sqr(const T1& a, const T2& b) {
///     return sqr(a.x - b.x)  + sqr(a.y - b.y);
/// }
/// \endcode
template<typename T1, typename T2>
constexpr _always_inline
Units::Area distance_sqr(const T1& a, const T2& b) {
  return sqr(a.x - b.x)  + sqr(a.y - b.y);
}

/// allgemeine Implementation um den einfachen Abstand zwischen zwei
/// kartesischen Punkten zubestimmen.
///
/// \warning Zur Berechnung muss, die Wurzel gezogen werden, was rechentechnisch
///     aufwendig ist (vor allem bei der Benutzung von Ganzzahlen),
///     meistens ist es besser die quatratischen Abstand zu vergleichen.
///     Will man wissen, ob zwei Punkte in einem bestimmten Radius zueinander liegen.
///     Sollte \ref TURAG::in_range verwendet werden, da die Funktion schneller ist
///     und besser gegen Integer-Overflow schützt.
///
template<typename T1, typename T2>
constexpr _always_inline
Length distance(const T1& a, const T2& b) {
  return sqrt(distance_sqr(a, b));
}

/// einfachen Abstand zwischen zwei kartesischen Punkten zubestimmen.
math_constexpr inline Length distance(Point a, Point b) { return hypot(b.x - a.x, b.y - a.y); }

/// einfachen Abstand zwischen zwei kartesischen Punkten zubestimmen.
math_constexpr inline Length distance(Pose  a, Point b) { return hypot(b.x - a.x, b.y - a.y); }

/// einfachen Abstand zwischen zwei kartesischen Punkten zubestimmen.
math_constexpr inline Length distance(Point a, Pose  b) { return hypot(b.x - a.x, b.y - a.y); }

/// einfachen Abstand zwischen zwei kartesischen Punkten zubestimmen.
math_constexpr inline Length distance(Pose  a, Pose  b) { return hypot(b.x - a.x, b.y - a.y); }


/// Schauen ob Punkt \a a in einem maximalen Radius \a r zu Punkt \a b liegt.
/// \returns \f$ (x_a - x_b)^2 + (y_a - y_b)^2 \leq r^2 \f$
template<typename T1, typename T2>
constexpr
bool in_range(const T1& a, const T2& b, Length r) {
  // Umgestellt um vor Overflow zu schützen
  return sqr(a.x - b.x) <= sqr(r) - sqr(a.y - b.y);
}

/// Winkel zwischen zwei Punkten berechnen
template<typename T1, typename T2>
constexpr _always_inline
Angle angle_between(const T1& a, const T2& b) {
  return atan2(b.y - a.y, b.x - a.x);
}


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

/// \brief Rechteck
///
class Rect {
public:
    /// leeres Rechteck erstellen
    constexpr Rect() :
		lb_(), rt_()
    { }

    /// \brief Rechteck aus zwei Punkten erstellen
	/// \param a Punkt 1
	/// \param b Punkt 2
	constexpr Rect(Point a, Point b) :
		lb_(min(a.x, b.x), min(a.y, b.y)),
		rt_(max(a.x, b.x), max(a.y, b.y))
	{ }

	/// \brief Rechteck aus Ursprungspunkt und Größe erstellen
	/// \param o Ursprungspunkt
	/// \param width Breite
	/// \param height Höhe
	constexpr Rect(Point o, Length width, Length height) :
		Rect(o, Point(o.x + width, o.y + height))
	{ }

    /// \brief Mittelpunkt von Rechteck bestimmen
	constexpr Point getCenter() const {
		return Point((lb_.x + rt_.x)/2.f, (lb_.y + rt_.y)/2.f);
	}

	constexpr Length getLeft() const { return lb_.x; }
	constexpr Length getBottom() const { return lb_.y; }
	constexpr Length getRight() const { return rt_.x; }
	constexpr Length getTop() const { return rt_.y; }

	constexpr Point getLeftBottom() const { return lb_; }
	constexpr Point getRightTop() const { return rt_; }
	constexpr Point getLeftTop() const { return Point(getLeft(), getTop()); }
	constexpr Point getRightBottom() const { return Point(getRight(), getBottom()); }

	constexpr Length getWidth() const { return rt_.x - lb_.x; }
	constexpr Length getHeight() const { return rt_.y - lb_.y; }
	constexpr Vector<Length> getSize() const { return Vector<Length>(getWidth(), getHeight()); }

	void inflate(Length dx, Length dy) {
		lb_.x -= dx/2;
		lb_.y -= dy/2;
		rt_.x += dx/2;
		rt_.y += dy/2;
	}

	void inflate(Length left, Length bottom, Length right, Length top) {
		lb_.x -= left;
		lb_.y -= bottom;
		rt_.x += right;
		rt_.y += top;
	}

private:
	/// Punkt links unten (kleinster x und y Wert)
	Point lb_;

	/// Punkt rechts oben (größter x und y Wert)
	Point rt_;
};

///
/// \brief Prüft ob Punkt in Rechteck liegt.
/// Liegt der Punkt auf dem Rand des Rechtecks, so wird auch \a true zurück gegeben.
/// \param rect Rechteck
/// \param point Punkt
/// \return \a true, wenn Punkt in Rechteck liegt, sonst \a false
///
bool located_in(const Rect& rect, const Point& point);

/// \brief Prüft ob ein Rechteck sich in der Nähe von Punkt befindet
/// Berührt der Kreis aus Punkt und Radius \a r den Rand des Rechtecks, so wird auch \a true zurück gegeben.
/// \param pos Position
/// \param rect Rechteck
/// \return \a true, wenn Kreis aus Punkt \a pos und Radius \a r das Rechteck \a rect schneidet, sonst \a false
bool in_range(const Point& pos, const Rect& rect, Length r);

/// \brief Prüft ob ein Rechteck sich in der Nähe von Punkt befindet
/// Berührt der Kreis aus Punkt \a pos und Radius \a r den Rand des Rechtecks, so wird auch \a true zurück gegeben.
/// \param pos Position
/// \param rect Rechteck
/// \return \a true, wenn Kreis aus Punkt \a pos und Radius \a r das Rechteck \a rect schneidet, sonst \a false
inline bool in_range(const Pose& pos, const Rect& rect, Length r) {
	return in_range(pos.toPoint(), rect, r);
}

/// Schnittpunkte zwischen zwei Kreisen zurückgeben.
/// \param one,two Kreise
/// \param[out] results Schnittpunkte, wenn Rückgabewert \a true
///
/// \retval true es existiert endliche Anzahl an Schnittpunkten, die in \a results gespeichert wird.
/// \retval false es existiert kein Schnittpunkt zwischen den beiden Kreisen oder beide Kreise sind gleich (unendlich viele Schnittpunkte)
bool intersect(Circle one, Circle two, std::array<Point, 2>& results);

/// \}

} // namespace TURAG

#endif // TINAPP_GEOMETRY_GEOMETRY_H
