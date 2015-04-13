#include "geometry.h"

namespace TURAG {

using namespace Units;

Angle norm_angle(Angle x) {
  while (x <= -angle_pi) {
    x += angle_pi * 2.f;
  }

  while (x > angle_pi) {
    x -= angle_pi * 2.f;
  }

  return x;
}

Point Pose::getRelativePoint(Length trans) const {
  return Point(x + trans * cos(phi),
               y + trans * sin(phi));
}

Point Pose::getRelativePoint(const Point& other) const {
  return Point(x + other.x * cos(phi) - other.y * sin(phi),
               y + other.x * sin(phi) + other.y * cos(phi));
}

Point Pose::getRelativePoint(const Pose& other) const {
  return Point(x + other.x * cos(phi) - other.y * sin(phi),
               y + other.x * sin(phi) + other.y * cos(phi));
}

Pose& Pose::translate(Length trans) {
  x += trans * cos(phi);
  y += trans * sin(phi);
  return *this;
}

Pose& Pose::translate(const Point& other) {
  x += other.x * cos(phi) - other.y * sin(phi);
  y += other.x * sin(phi) + other.y * cos(phi);
  return *this;
}

Pose& Pose::translate(const Pose& other) {
  x += other.x * cos(phi) - other.y * sin(phi);
  y += other.x * sin(phi) + other.y * cos(phi);
  phi = norm_angle(phi + other.phi);
  return *this;
}

} // namespace TURAG
