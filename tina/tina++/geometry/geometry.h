#ifndef TINAPP_GEOMETRY_GEOMETRY_H
#define TINAPP_GEOMETRY_GEOMETRY_H

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
constexpr
Angle norm_angle(Angle x) {
    return (x <= -Units::angle_pi)
            ? norm_angle(x + 2 * Units::angle_pi)
            : ( x > Units::angle_pi)
              ? norm_angle(x - 2 * Units::angle_pi)
              : x;
}

// Forward declaration wegen getTurnedPoseTowards
template<typename T1, typename T2>
constexpr _always_inline
Angle angle_between(const T1& a, const T2& b);

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
	/// \param x_corr,y_corr Koordinaten
	constexpr
	Vector(U x_corr, U y_corr) :
		x(x_corr), y(y_corr)
    { }

    /// Nullvektor erstellen
    constexpr
    Vector() :
        x(0), y(0)
    { }

    /// Kopierkonstruktor
    constexpr
    Vector(const Vector& p) = default;

    /// zwei Vektoren addieren
    constexpr
    Vector operator+(const Vector<U>& other) const {
        return Vector(x + other.x, y + other.y);
    }

    /// zwei Vektoren subtrahieren.
    /// Sonderfall Ortsvektor: Relativen Vektor erhalten
    constexpr
    Vector operator-(const Vector<U>& other) const {
        return Vector(x - other.x, y - other.y);
    }

    /// zwei Punkte vergleichen
    constexpr
    bool operator==(const Vector<U>& other) const {
        return (x == other.x) && (y == other.y);
    }

    /// zwei Punkte vergleichen
    constexpr
    bool operator!=(const Vector<U>& other) const {
        return (x != other.x) || (y != other.y);
    }

    /// Vektor skalar multiplizieren
    /// \param rhs Faktor
    template<typename U2> constexpr
    Vector operator*(U2 rhs) const {
        return Vector(x*rhs, y*rhs);
    }

    /// Vektor skalar dividieren
    /// \param rhs Quotient
    template<typename U2> constexpr
    Vector operator/(U2 rhs) const {
        return Vector(x/rhs, y/rhs);
    }

    /// Betrag des Vektors ausgeben
    constexpr inline
    U magnitude() const {
        return U(::hypot(x.value, y.value));
    }
};

/// Ortsvektor
typedef Vector<Units::Length> Point;

/// Geschwindigkeitsvektor
typedef Vector<Units::Velocity> PositionVelocity;

/// Beschleunigungsvektor
typedef Vector<Units::Acceleration> PositionAcceleration;

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
	VectorPolar(U1 r_, U2 phi_) :
		r(r_), phi(phi_)
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

/// \brief Darstellung für gerichtete Objekte im zweidimensionalen Raum
///
/// Datentyp mit einer linearen Vektorkomponente und einer Angularen Komponente.
/// Geeignet für Posen und deren zeitlichen Ableitungen.
///
/// Eigentlich sollte der Datentyp 'GenericPose' heißen. Eine ausgiebige Diskussion
/// um 2 Uhr nachts zwischen Lukas M. und Hermann mit ausreichend Captain Grand
/// ergab, dass eine Pose eigentlich nur den Spezialfall der stationären Pose
/// beschreibt. Der Generische Datentyp beschreibt eine Komposition aus linearen
/// und angularen Datentypen im zweidimensionalen Raum und lässt beliebige
/// Zeitableitungen zu.
template<typename U1 = Length, typename U2 = Angle>
struct LinearAngular2d {
    /// Lineare Komponente
    Vector<U1> p;

    /// Aliase für Vektormember
    U1 &x = p.x;
    U1 &y = p.y;

    /// Angulare Komponente
    U2 phi;

    /// Aus linearem Vektor und angularer Komponente erstellen
    explicit constexpr
    LinearAngular2d(const Vector<U1>& p_, U2 a_ = Units::null) :
        p(p_), phi(a_)
    { }

    /// Aus zwei linearen und einer angularen Komponente erstellen
    constexpr
    LinearAngular2d(U1 x_, U1 y_, U2 phi_) :
        p(x_, y_), phi(phi_)
    { }

    /// Kopierkonstruktor
    constexpr
    LinearAngular2d(const LinearAngular2d<U1, U2>& other) :
        p(other.p), phi(other.phi)
    { }

    /// Standardkonstruktor
    constexpr
    LinearAngular2d() :
        p()
    { }

    /// Zuweisung
    LinearAngular2d& operator=(const LinearAngular2d<U1, U2>& other) {
        p = other.p;
        phi = other.phi;
        return *this;
    }

    /// Lineare Komponenten aus Vektor übernehmen. Angulare Komponente wird nicht geändert.
    /// TODO: Obsolet, da Vektor Klassenmember?
    void assign(const Vector<U1>& p_) {
        p = p_;
    }

    /// zugehörigen Vektor zurückgeben
    /// TODO: Obsolet, da Vektor Klassenmember?
    constexpr Vector<U1> toVector() const {
        return p;
    }

    /// Betrag der linearen Komponenten bilden
    constexpr inline
    U1 magnitude() const {
        return p.magnitude();
    }
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
	Pose(Length x_, Length y_, Angle phi_) :
		x(x_), y(y_), phi(phi_)
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

    /// neue Pose erstellen, deren Winkel in Richtung des angegebenen Punkts zeigt.
    Pose getTurnedPoseTowards(Point p) const {
        return Pose(x, y, angle_between(toPoint(), p));
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

    /// neue Pose erstellen mit überschriebenem x-Wert
    /// \param new_x neuer x-Wert
    constexpr Pose overwriteX(const Length new_x) const {
        return Pose(new_x, y, phi);
    }

    /// neue Pose erstellen mit überschriebenem y-Wert
    /// \param new_y neuer y-Wert
    constexpr Pose overwriteY(const Length new_y) const {
        return Pose(x, new_y, phi);
    }
    /// neue Pose erstellen mit überschiebenem Winkel
    constexpr Pose overwritePhi(const Angle new_phi) const {
        return Pose(x, y, new_phi);
    }
    /// Winkel, um den rotiert werden muss, um auf Punkst p zu zeigen
    constexpr Angle getAngleToPoint(const Point& p) const {
        return norm_angle(angle_between(*this, p) - phi);
    }
};

/// robot velocity/acceleration in translation and rotation
typedef LinearAngular2d<Units::Velocity, Units::AngularVelocity> PoseVelocity;
typedef LinearAngular2d<Units::Acceleration, Units::AngularAcceleration> PoseAcceleration;

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
math_constexpr inline Length distance(const Point& a, const Point& b) { return hypot(b.x - a.x, b.y - a.y); }

/// einfachen Abstand zwischen zwei kartesischen Punkten zubestimmen.
math_constexpr inline Length distance(const Pose&  a, const Point& b) { return hypot(b.x - a.x, b.y - a.y); }

/// einfachen Abstand zwischen zwei kartesischen Punkten zubestimmen.
math_constexpr inline Length distance(const Point& a, const Pose&  b) { return hypot(b.x - a.x, b.y - a.y); }

/// einfachen Abstand zwischen zwei kartesischen Punkten zubestimmen.
math_constexpr inline Length distance(const Pose&  a, const Pose&  b) { return hypot(b.x - a.x, b.y - a.y); }

/// Schauen ob Punkt \a a in einem maximalen Radius \a r zu Punkt \a b liegt.
/// \returns \f$ (x_a - x_b)^2 + (y_a - y_b)^2 \leq r^2 \f$
template<typename T1, typename T2, typename T3>
constexpr
bool in_range(const T1& a, const T2& b, T3 r) {
  // Umgestellt um vor Overflow zu schützen
  return sqr(a.x - b.x) <= sqr(r) - sqr(a.y - b.y);
}

/// Winkel zwischen zwei Punkten berechnen
template<typename T1, typename T2>
constexpr _always_inline
Angle angle_between(const T1& a, const T2& b) {
  return atan2(b.y - a.y, b.x - a.x);
}

/// \}

} // namespace TURAG

#endif // TINAPP_GEOMETRY_GEOMETRY_H
