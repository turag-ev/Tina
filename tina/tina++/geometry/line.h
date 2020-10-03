#ifndef TINAPP_GEOMETRY_LINE_H
#define TINAPP_GEOMETRY_LINE_H

#include "circle.h"
#include "geometry.h"
#include "rect.h"
#include "units.h"

namespace TURAG {

///
/// \brief Linie zwischen zwei zweidimensionalen Punkten p und q
///
struct Line
{
    ///
    /// \brief Default-Konstruktor
    ///
    constexpr Line() :
        p(), q()
    { }
    
    ///
    /// \brief Line aus Endpunkten erstellen
    /// \param a, b Endpunkte der Linie
    ///
    constexpr Line(const Point& a, const Point& b) :
        p(a), q(b)
    { }
    
    ///
    /// \brief Line aus Startpunkt und Richtungsvektor erstellen
    /// \param a Startpunkt
    /// \param dx X-Komponente des Richtungsvektors
    /// \param dy Y-Komponente des Richtungsvektors
    ///
    constexpr Line(const Point& a, const Length dx, const Length dy) :
        p(a), q(a.x + dx, a.y + dy)
    { }
    
    ///
    /// \brief Länge der Linie ermitteln
    /// \return Länge der Linie
    ///
    TURAG_MATH_CONSTEXPR
    Length getLength() const {
        return distance(p, q);
    }
    
    ///
    /// \brief Quadrat der Länge der Linie ermitteln
    /// \return Quadrat der Länge der Line
    /// 
    /// Performanter als getLength()
    ///
    constexpr
    Units::Area getSquaredLength() const {
        return distance_sqr(p, q);
    }
    
    ///
    /// \brief Gibt minimalen X-Wert aus
    /// \return minimale X-Koordinate, die von Linie abgedekt wird
    ///
    constexpr
    Length getLeft() const {
        return min(p.x, q.x);
    }
    
    ///
    /// \brief Gibt minimalen Y-Wert aus
    /// \return minimale Y-Koordinate, die von Linie abgedekt wird
    ///
    constexpr
    Length getBottom() const {
        return min(p.y, q.y);
    }
    
    ///
    /// \brief Gibt maximalen X-Wert aus
    /// \return maximale X-Koordinate, die von Linie abgedekt wird
    ///
    constexpr
    Length getRight() const {
        return max(p.x, q.x);
    }
    
    ///
    /// \brief Gibt maximalen Y-Wert aus
    /// \return maximale Y-Koordinate, die von Linie abgedekt wird
    ///
    constexpr
    Length getTop() const {
        return max(p.y, q.y);
    }
    
    ///
    /// \brief Erster Endpunkt
    ///
    Point p;
    
    ///
    /// \brief Zweiter Endpunkt
    ///
    Point q;
};

///
/// \brief Prüft, ob sich zwei Linien schneiden
/// \param a, b zwei Linien
/// \return ob sich die zwei Linien schneiden
///
bool intersect(const Line& a, const Line& b);

///
/// \brief Ermittelt quadratische Distanz zwischen Linie und Punkt
/// \param l Linie
/// \param p Punkt
/// \return Quadrat der Distanz
///
Units::Area distance_sqr(const Line& l, const Point& p);

///
/// \brief Quadratische Distanz zwischen Linie und Punkt ermitteln
/// \param l Linie
/// \param p Punkt
/// \return Distanz
///
inline
Units::Length distance(const Line& l, const Point& p) {
    return sqrt(distance_sqr(l, p));
}

///
/// \brief Prüft, ob sich Punkt nicht weiter als r von einer Linie befindet
/// \param l Linie
/// \param p Punkt
/// \param r max. Distanz
/// \return 
///
inline
bool in_range(const Line& l, const Point& p, const Length r) {
    return distance_sqr(l, p) <= sqr(r);
}

///
/// \brief Prüft, ob sich Kreis nicht weiter als r von einer Linie befindet
/// \param l Linie
/// \param c Kreis
/// \param r max. Distanz
/// \return 
///
inline
bool in_range(const Line& l, const Circle& c, const Length r = Units::null) {
    return in_range(l, c.getCenter(), c.r + r);
}

///
/// \brief Prüft, ob sich zwei Linien nicht weiter als r voneinander entfernen
/// \param l1, l2 zwei Linien
/// \param r max. Distanz
/// \return 
///
inline
bool in_range(const Line& l1, const Line& l2, const Length r) {
    return in_range(l1, l2.p, r) || in_range(l1, l2.q, r)
            || in_range(l2, l1.p, r) || in_range(l2, l1.q, r);
}

///
/// \brief Prüfen, ob sich Rechteck nicht weiter als r von Linie entfernt
/// \param l Linie
/// \param q Rechteck
/// \param r max. Distanz
/// \return 
///
inline
bool in_range(const Line& line, const Rect& rect, const Length r) {
    const Line top(rect.getLeftTop(), rect.getRightTop());
    const Line right(rect.getRightTop(), rect.getRightBottom());
    const Line bottom(rect.getRightBottom(), rect.getLeftBottom());
    const Line left(rect.getLeftBottom(), rect.getLeftTop());
    
    return in_range(line.p, rect, r) || in_range(line.q, rect, r)
            || in_range(line, rect.getLeftTop(), r) || in_range(line, rect.getRightTop(), r)
            || in_range(line, rect.getLeftBottom(), r) || in_range(line, rect.getRightBottom(), r)
            || intersect(top, line)
            || intersect(right, line)
            || intersect(bottom, line)
            || intersect(left, line);
}

} // namespace TURAG

#endif // TINAPP_GEOMETRY_LINE_H
