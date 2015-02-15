#include "fieldpose.h"

namespace TURAG {

TeamColor_t FieldPose::teamcolor_ = TeamColor_None;

void FieldPose::switchSide(Pose& pose) {
  pose.x = -pose.x;
  pose.phi = (pose.phi != NO_ANGLE * Units::rad)
      ? (pose.phi >= Units::null ? Units::angle_pi - pose.phi : -Units::angle_pi - pose.phi)
      : NO_ANGLE * Units::rad;
}

Angle FieldPose::independentAngle(Angle angle) {
  if (!FieldPose::isRightTeamcolor()) {
    return (angle != NO_ANGLE * Units::rad)
        ? (angle >= Units::null ? Units::angle_pi - angle : -Units::angle_pi - angle)
        : NO_ANGLE * Units::rad;
  } else {
    return angle;
  }
}

Pose FieldPose::independentPose(const Pose& pose) {
  if (!FieldPose::isRightTeamcolor()) {
    return Pose(-pose.x, pose.y, (pose.phi != NO_ANGLE * Units::rad)
                ? (pose.phi >= Units::null ? Units::angle_pi - pose.phi : -Units::angle_pi - pose.phi)
                : NO_ANGLE * Units::rad);
  } else {
    return pose;
  }
}

Pose FieldPose::get() const {
  return for_teamcolor(right_, left_);
}

} // namespace TURAG
