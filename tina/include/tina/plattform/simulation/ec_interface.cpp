#include "ec_interface.h"

#include <comm/id.h>
#include <global/eb13.h>
#include <utils/cansc.h>

#include "robot-plugin.h"

////////////////////////////////////////////////////////////////////////////////
// Encoder

CAN_UI_RPC_HANDLER(EC_SET_TEAMCOLOR, param) {
  SimRobot::getInstance().setTeamColor(static_cast<TeamColor_t>(param));

  return 0;
}

CAN_UI_RPC_HANDLER(EC_GET_TEAMCOLOR, param) {
  UNUSED(param);
  return SimRobot::getInstance().getTeamColor();
}

CAN_UI_RPC_HANDLER(UI_PREPARE, param) {
  UNUSED(param);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Blackboard

namespace SystemControl {
namespace CAN {
void read_blackboard_request(Id id, Id owner, pointer dest) {
  if (owner == CASA_ID_EC || owner == CASA_ID_MC) {
    SimRobot& plugin = SimRobot::getInstance();

    if (id == BB_ROBO_STATE) {
      bb_robot_state_t* robot_state = static_cast<bb_robot_state_t*>(dest);
      qreal trans, rot;
      plugin.getVelocity(&trans, &rot);
      PoseF pose = plugin.getPose();

      robot_state->pose.x = pose.x;
      robot_state->pose.y = pose.y;
      robot_state->pose.phi = pose.phi * M_PI/180.;
      robot_state->velocity.trans = trans;
      robot_state->velocity.rot = rot * M_PI/180.;

    } else {
      plugin.getOtherRobots(id, static_cast<bb_oppo_state_t*>(dest));
    }
  }
}
} // namespace CAN
} // namespace SystemControl
