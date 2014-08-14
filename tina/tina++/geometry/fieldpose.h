#ifndef POSES_H
#define POSES_H

#include "tina++/tina.h"
#include "geometry.h"

#include <global/eurobot.h>

namespace TURAG {

/// \defgroup EdgeUndep Spielfeldpositionen
///
/// Positionen und Winkel werden immer nur für rechte Teamfarbe angegeben
/// bei Verwendung von linker Teamfarbe werden Positionen und Winkel gespiegelt
///
/// \{

/// abhängig von Teamfarbe \p right oder \p left übergeben
template<typename T>
_always_inline
T for_teamcolor(T right, T left);

/// \}

/// \ingroup Geometry
/// Spielfeldposition
///
/// Tool zur vereinfachten Angabe von Spielfeldpositionen, die abhängig von der Teamfarbe wechseln.
/// Die Angabe erfolgt immer für rechte Teamfarbe, für linke Teamfarbe wird Pose gespiegelt.
/// Es ist aber auch möglich für unsymetrische Aufgaben die rechte und linke Seite
/// explizit anzugeben. Die jeweilig richtige Position wird abhängig von der aktuellen
/// Teamfarbe beim Umwandeln von Fieldpose nach Pose automatisch ausgewählt.
///
/// Damit die aktuelle Teamfarbe bekannt ist muss vorher TURAG::set_teamcolor
/// aufgerufen werden.
///
/// \warning FieldPose darf erst in Pose umgewandelt werden, wenn die Teamfarbe
/// auf TeamColor_Left oder TeamColor_Right gesetzt wurde! Sonst wird die
/// immer die rechte Position ausgewählt!
class FieldPose {
public:
  /// Feldposition aus einzelnen Koordinaten erstellen
  constexpr explicit
  FieldPose(Length x, Length y, Angle phi, bool mirror_phi=true) :
    left_(-x, y, (mirror_phi && (phi != NO_ANGLE * Units::rad))
                  ? (phi >= Units::null ? Units::angle_pi - phi : -Units::angle_pi - phi)
                  : phi),
    right_(x, y, phi)
  { }

  /// Feldposition aus Pose erstellen
  constexpr explicit
  FieldPose(const Pose& pose) :
    left_(-pose.x, pose.y, (pose.phi != NO_ANGLE * Units::rad)
                            ? (pose.phi >= Units::null ? Units::angle_pi - pose.phi : -Units::angle_pi - pose.phi)
                            : pose.phi),
    right_(pose)
  { }

  /// Feldposition aus zwei Posen für die jeweilige Teamfarbe erstellen (wenn Roboter asymmentrisch ist)
  constexpr explicit
  FieldPose(const Pose& left, const Pose& right) :
    left_(left), right_(right)
  { }

  /// Pose abhängig von Teamfarbe ausgeben
  Pose get() const;

  operator Pose() const {
    return get();
  }

  operator Point() const {
    return get().toPoint();
  }

  /// \brief Teamfarbe initialisieren
  static void setTeamcolor(TeamColor_t teamcolor) {
      teamcolor_ = teamcolor;
  }

  /// aktuelle Teamfarbe erhalten
  static TeamColor_t getTeamcolor() {
      return teamcolor_;
  }

  /// Haben wir die rechte Teamfarbe?
  static bool isRightTeamcolor() {
      return teamcolor_ == TeamColor_Right;
  }

  /// Pose für andere Teamfarbe spiegeln
  static void switchSide(Pose& pose);

  /// Pose für andere Teamfarbe
  static _always_inline Pose otherSide(Pose pose) {
      switchSide(pose);
      return pose;
  }

  /// Winkel abhängig von Teamfarbe ausgeben
  static Angle independentAngle(Angle angle);

  /// Pose abhängig von Teamfarbe ausgeben
  static Pose independentPose(const Pose& pose);

  /// Punkt abhängig von Teamfarbe ausgeben
  inline static Point independentPoint(const Point& point) {
    return for_teamcolor(point, Point(-point.x, point.y));
  }

  /// Punkt für Gegnerseite abhängig von Teamfarbe ausgeben
  inline static Point independentOpponentPoint(const Point& point) {
    return for_teamcolor(Point(-point.x, point.y), point);
  }

private:
  /// Pose für linke Teamfarbe
  const Pose left_;

  /// Pose für rechte Teamfarbe
  const Pose right_;

  static TeamColor_t teamcolor_;
};

template<typename T>
_always_inline
T for_teamcolor(T right, T left) {
    return FieldPose::isRightTeamcolor() ? right : left;
}

} // namespace TURAG

#endif // POSES_H
