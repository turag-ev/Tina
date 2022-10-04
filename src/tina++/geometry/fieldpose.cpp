#include "fieldpose.h"

namespace TURAG {

TeamColor_t FieldPose::teamcolor_ = TeamColor_None;

void FieldPose::switchSide(Pose& pose) {
  pose.x = -pose.x;
  pose.phi = mirrorPhi(pose.phi);
}

Angle FieldPose::independentAngle(Angle angle) {
  if (!FieldPose::isRightTeamcolor()) {
    return mirrorPhi(angle);
  } else {
    return angle;
  }
}

Pose FieldPose::independentPose(const Pose& pose) {
  if (!FieldPose::isRightTeamcolor()) {
    return Pose(-pose.x, pose.y, mirrorPhi(pose.phi));
  } else {
    return pose;
  }
}

Pose FieldPose::get() const {
  return for_teamcolor(right_, left_);
}

} // namespace TURAG
