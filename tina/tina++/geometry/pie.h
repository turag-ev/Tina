#ifndef PIE_H
#define PIE_H

#include <array>
#include <tina++/geometry/circle.h>
#include "geometry.h"

namespace TURAG {

/// Darstellung für Kreisausschnitt aus Mittelpunkt, Radius, Startwinkel und Breitenwinkel
struct Pie {
    /// Kreisausschnitt aus kartesischen Punkt, Radius, Start- und Breitenwinkel erstellen
    /// \tparam Typ mit x- und y-Koordinate
    template<typename P>
    constexpr explicit
    Pie(const P& t, Length r_, Angle start_, Angle width_) :
        m(t.x, t.y), r(r_), start(start_), width(norm_angle(width_) )
    { }

    /// unintialisierten Kreisausschnitt erstellen (Radius ist negativ)
    constexpr explicit
    Pie() :
		m(), r(-1.f*Units::mm), start(0), width(0)
    { }

    constexpr Point getCenter() const { return m; }

    /// Mittelpunkt des Kreies
    Point m;

    /// Radius des Kreises
    Length r;

    /// Startwinkel
    Angle start;

    /// Breitenwinkel
    Angle width;
};

/// Prüfen ob Überschneidung zwischen Kreisausschnitt und Kreis existiert.
/// \relates Circle
/// \relates Pie
/// \param pie Kreisausschnitt
/// \param circle Kreis
///
/// \retval true Überschneidung existiert
/// \retval false keine Überschneidung existiert
bool intersect(const Pie& pie, const Circle& circle);

} // namespace TURAG

#endif // PIE_H
