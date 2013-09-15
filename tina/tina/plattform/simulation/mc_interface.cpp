#include <QPainter>

#define LOG_SOURCE "I"
#include <utils/debug.h>

#include <utils/cansc.h>
#include <utils/extra/packed.h>
#include <interfaces/transform.h>

#include "mc_interface.h"
#include "robot-plugin.h"

using namespace SystemControl;

static
double norm_angle(double x){
  if (x <= -180.) {
    x += 360.;
  } else if (x > 180.) {
    x -= 360.;
  }

  return x;
}

static inline
double sgn(double x) {
  return copysign(1, x);
}

static inline
double sqr(double x) {
  return x * x;
}

////////////////////////////////////////////////////////////////////////////////
///  ConstantVelocity

namespace {

class ConstantVelocity {
public:
  ConstantVelocity(qreal trans_velocity = 0., qreal rot_velocity = 0.);

  void advance(MotionControl::Pose& pos, unsigned delta_t);
  MotionControl::Velocity getVelocity() const { return velocity_; }

  void setMaxVelocity(qreal trans_velocity, qreal rot_velocity) {
    velocity_.trans = trans_velocity;
    velocity_.rot = rot_velocity;
  }

private:
  MotionControl::Velocity velocity_;
};

ConstantVelocity::ConstantVelocity(qreal trans_velocity, qreal rot_velocity) :
  velocity_{trans_velocity, rot_velocity}
{ }

void ConstantVelocity::advance(MotionControl::Pose& pos, unsigned delta_t) {
  double delta_s = velocity_.trans * delta_t / 1000.;
  pos.x += delta_s * cos(pos.phi * M_PI/180.);
  pos.y += delta_s * sin(pos.phi * M_PI/180.);
  pos.phi += velocity_.rot * delta_t / 1000.;
}

}

////////////////////////////////////////////////////////////////////////////////
///  StopJob

namespace {

class StopJob : public MotionControl::Job {
public:
  StopJob() { }

  bool advance(MotionControl::Pose& pos, unsigned delta_t) override;
  MotionControl::Velocity getVelocity() const override;
};

bool StopJob::advance(MotionControl::Pose&, unsigned) {
  return true;
}

MotionControl::Velocity StopJob::getVelocity() const {
  return MotionControl::Velocity{0., 0.};
}

}

////////////////////////////////////////////////////////////////////////////////
///  TurnJob

namespace {

class TurnJob : public MotionControl::Job {
public:
  enum class Direction {
    ANTI_CLOCK_WISE = 1,
    DONT_CARE = 0,
    CLOCK_WISE = -1
  };

  TurnJob(qreal src_angle, qreal dest_angle, Direction dir, SpeedIndex speed);

  bool advance(MotionControl::Pose& pos, unsigned delta_t) override;
  MotionControl::Velocity getVelocity() const override;

private:
  qreal dest_angle_;
  Direction dir_;
  ConstantVelocity velocity_;
};

TurnJob::TurnJob(qreal src_angle, qreal dest_angle, Direction dir, SpeedIndex speed) :
  dest_angle_(dest_angle), dir_(dir), velocity_()
{
  if (dir == Direction::DONT_CARE) {
    double diff = dest_angle - src_angle;
    dir_ = (((fabs(diff) <= 360.-fabs(diff)) ? -sgn(diff) : sgn(diff)) > 0)
        ? Direction::CLOCK_WISE : Direction::ANTI_CLOCK_WISE;
  }

  velocity_.setMaxVelocity(0., MotionControl::ROTATION[speed] * int(dir_));
}

bool TurnJob::advance(MotionControl::Pose& pos, unsigned delta_t) {
  velocity_.advance(pos, delta_t);

  // FIXME: Bei Drehung um längeren Winkel, springt Roboter
  double diff = norm_angle(dest_angle_ - pos.phi);
  if (dir_ == Direction::ANTI_CLOCK_WISE) {
    // im mathematisch positiver Sinn
    if (diff < 0) {
      pos.phi = dest_angle_;
      return true;
    }

  } else {
    // gegen den mathematisch positiver Sinn
    if (diff > 0) {
      pos.phi = dest_angle_;
      return true;
    }
  }

  return false;
}

MotionControl::Velocity TurnJob::getVelocity() const {
  return velocity_.getVelocity();
}

}

////////////////////////////////////////////////////////////////////////////////
///  DriveJob

namespace {

class DriveJob : public MotionControl::Job {
public:
  DriveJob(MotionControl::Pose dest, Direction dir, SpeedIndex speed);

  bool advance(MotionControl::Pose& pos, unsigned delta_t) override;
  MotionControl::Velocity getVelocity() const override;

private:
  MotionControl::Pose dest_;
  Direction dir_;
  ConstantVelocity velocity_;
};

DriveJob::DriveJob(MotionControl::Pose dest, Direction dir, SpeedIndex speed) :
  dest_(dest), dir_(dir), velocity_(MotionControl::VELOCITY[speed] * int(dir), 0.)
{ }

bool DriveJob::advance(MotionControl::Pose& pos, unsigned delta_t) {
  double old_dist = sqr(pos.x - dest_.x) + sqr(pos.y - dest_.y);

  velocity_.advance(pos, delta_t);

  double new_dist = sqr(pos.x - dest_.x) + sqr(pos.y - dest_.y);
  if (new_dist >= old_dist) {
    pos.x = dest_.x;
    pos.y = dest_.y;
    return true;
  }

  return false;
}

MotionControl::Velocity DriveJob::getVelocity() const {
  return velocity_.getVelocity();
}

}

////////////////////////////////////////////////////////////////////////////////
///  DriveCircleJob

namespace {

class DriveCircleJob : public MotionControl::Job {
public:
  DriveCircleJob(double r, double phi2, int dir, SpeedIndex speed);

  bool advance(MotionControl::Pose& pos, unsigned delta_t) override;
  MotionControl::Velocity getVelocity() const override;

private:
  double phi2_;
  int dir_;
  ConstantVelocity velocity_;
};

DriveCircleJob::DriveCircleJob(double r, double phi2, int dir, SpeedIndex speed) :
  phi2_(phi2), dir_(dir), velocity_(MotionControl::VELOCITY[speed], dir * MotionControl::VELOCITY[speed]/r)
{ }

bool DriveCircleJob::advance(MotionControl::Pose& pos, unsigned delta_t) {
  // TODO
  UNUSED(pos);
  UNUSED(delta_t);

  return false;
}

MotionControl::Velocity DriveCircleJob::getVelocity() const {
  return velocity_.getVelocity();
}

}

////////////////////////////////////////////////////////////////////////////////

const double MotionControl::VELOCITY[3] = {0.8 * 1250,
                                           0.5 * 1250,
                                           0.2 * 1250}; // mm / s

const double MotionControl::ROTATION[3] = {0.8 * 400,
                                           0.6 * 400,
                                           0.2 * 400}; // deg / s

MotionControl::MotionControl() :
  sc_id(0)
{
  pen.setStyle(Qt::DashLine);
  pen.setWidth(4);
  pen.setBrush(Qt::black);
}

int MotionControl::driveToPose(Pose dest, Direction dir, SpeedIndex speed) {
  double yaw_delta = atan2(dest.y - pose_.y, dest.x - pose_.x) * 180./M_PI;

  if (dir == DIRECTION_DONT_CARE) {
    // vorfährts oder rückwärtsfahren
    double diff = yaw_delta - pose_.phi;
    dir = (fabs(norm_angle(diff)) < fabs(norm_angle(diff + 180.)))
          ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
  }

  if (dir == DIRECTION_BACKWARD) {
    yaw_delta = norm_angle(yaw_delta + 180.);
  }

  waitung_jobs_.clear();
  current_job_ = std::unique_ptr<StopJob>(new StopJob());

  if (abs(norm_angle(pose_.phi - yaw_delta)) > 0.2) {
    waitung_jobs_.emplace_back(new TurnJob(pose_.phi, yaw_delta, TurnJob::Direction::DONT_CARE, speed));
  }

  waitung_jobs_.emplace_back(new DriveJob(dest, dir, speed));

  // wenn Winkel egal, nicht drehen
  if (!isnan(dest.phi) && abs(norm_angle(yaw_delta - dest.phi)) > 0.2) {
    waitung_jobs_.emplace_back(new TurnJob(yaw_delta, dest.phi, TurnJob::Direction::DONT_CARE, speed));
  }

  notify_ = true;

  return ++sc_id;
}

int MotionControl::driveCircle(MotionControl::Pose dest, MotionControl::Pose m, Direction dir, SpeedIndex speed) {
  double phi1 = norm_angle(90. + atan2(pose_.y - m.y, pose_.x - m.x));
  double phi2 = norm_angle(90. + atan2(dest.y - m.y, dest.x - m.x));
  double diff = phi2 - phi1;
  int pdir = (fabs(diff) <= 360.-fabs(diff)) ? -sgn(diff) : sgn(diff);
  double r = sqrt(sqr(m.x - pose_.x) + sqr(m.y - pose_.y));

  if (dir == DIRECTION_DONT_CARE) {
    // vorfährts oder rückwärtsfahren TODO
    double dphit = phi1 - pose_.phi;
    dir = (fabs(norm_angle(dphit)) < fabs(norm_angle(dphit + 180.)))
          ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
  }

  if ((dir == DIRECTION_BACKWARD && pdir == -1) ||
      (dir == DIRECTION_FORWARD  && pdir == 1)) {
    phi1 = norm_angle(phi1 + 180.);
    phi2 = norm_angle(phi2 + 180.);
  }

  waitung_jobs_.clear();
  current_job_ = std::unique_ptr<StopJob>(new StopJob());

  if (abs(norm_angle(pose_.phi - phi1)) > 0.2) {
    waitung_jobs_.emplace_back(new TurnJob(pose_.phi, phi1, TurnJob::Direction::DONT_CARE, speed));
  }

  waitung_jobs_.emplace_back(new DriveCircleJob(r, phi2, pdir, speed));

  return ++sc_id;
}

int MotionControl::stop() {
  waitung_jobs_.clear();
  current_job_ = std::unique_ptr<StopJob>(new StopJob());

  return ++sc_id;
}

MotionControl::Velocity MotionControl::getVelocity() const {
  Mutex::Lock lock(mutex_);

  if (current_job_) {
    return current_job_->getVelocity();
  } else {
    return Velocity{0., 0.};
  }
}

MotionControl::Pose MotionControl::getPose() const {
  Mutex::Lock lock(mutex_);
  return pose_;
}

MotionControl::Pose MotionControl::advance(unsigned delta_t) {
  Mutex::Lock lock(mutex_);

  if (current_job_) {
    bool done = current_job_->advance(pose_, delta_t);
    pose_.phi = norm_angle(pose_.phi);
    if (done) {
      current_job_.reset();
      if (waitung_jobs_.size() > 0) {
        current_job_ = std::move(waitung_jobs_.front());
        waitung_jobs_.pop_front();
      } else if (notify_) {
        SystemControl::CAN::call(CASA_ID_SC, SC_TARGET_REACHED, sc_id);
      }
    }
  }

  return pose_;
}

void mc_paint(QPainter *painter, const QStyleOptionGraphicsItem *) {
  UNUSED(painter);
//  PoseF pose = SimRobot::getInstance().getPose();

//  if (!mc.terminated) {
//    Mutex::Lock lock(mc.mutex);
//    painter->setBrush(Qt::NoBrush);
//    painter->setPen(mc.pen);

//    if (mc.request == MotionControl::REQUEST_DRIVE_POSE) {
//      painter->drawLine(SimLine(pose.x, pose.y, mc.drive_pose.x, mc.drive_pose.y));
//      painter->drawEllipse(SimPoint(mc.drive_pose.x, mc.drive_pose.y), 20, 20);

//    } else if (mc.request == MotionControl::REQUEST_DRIVE_CIRCLE) {
//      double r = sqrt(sqr(mc.circle_pose.xm - mc.circle_pose.x) + sqr(mc.circle_pose.ym - mc.circle_pose.y));
//      double phi1 = atan2(mc.pose.y - mc.circle_pose.ym, mc.pose.x - mc.circle_pose.xm);
//      double phi2 = atan2(mc.circle_pose.y - mc.circle_pose.ym, mc.circle_pose.x - mc.circle_pose.xm);
//      painter->drawArc(SimRectCenter(mc.circle_pose.xm, mc.circle_pose.ym, r*2, r*2),
//                       to_deg(phi1) * 16., to_deg(phi2 - phi1) * 16);
//      painter->drawEllipse(SimPoint(mc.circle_pose.x, mc.circle_pose.y), 20, 20);
//    }
//  }
}

CAN_MC_RPC_HANDLER(MC_DRIVE_DISTANCE_ANGLE, param) {
  MotionControl& mc = SimRobot::getInstance().mc;
  Mutex::Lock lock(mc.mutex_);

  drive_distance_angle_t arg;
  arg.raw = param;

  return mc.driveToPose(MotionControl::Pose(mc.pose_.x + arg.distance * cos(mc.pose_.phi * M_PI/180.),
                                            mc.pose_.y + arg.distance * sin(mc.pose_.phi * M_PI/180.),
                                            norm_angle(mc.pose_.phi + arg.angle/1000. * 180./M_PI)),
                        (arg.distance > 0) ? DIRECTION_FORWARD : DIRECTION_BACKWARD,
                        static_cast<SpeedIndex>(arg.speed));
}

CAN_MC_RPC_HANDLER(MC_STOP_ROBOT, param) {
  MotionControl& mc = SimRobot::getInstance().mc;
  Mutex::Lock lock(mc.mutex_);

  UNUSED(param);
  mc.stop();

  return 0;
}

CAN_MC_RPC_HANDLER(MC_DRIVE_TO_POSE, param) {
  MotionControl& mc = SimRobot::getInstance().mc;
  Mutex::Lock lock(mc.mutex_);

  drive_to_pose_t& arg = extra::reinterpret_reference<drive_to_pose_t>(param);
  double phi = (arg.phi > M_PI*1000+1) ? (NAN) : (arg.phi/1000. * 180./M_PI);
  return mc.driveToPose(MotionControl::Pose(arg.x, arg.y, phi),
                        static_cast<Direction>(arg.dir),
                        static_cast<SpeedIndex>(arg.speed));
}

CAN_MC_RPC_HANDLER(MC_DRIVE_CIRCLE, param) {
  MotionControl& mc = SimRobot::getInstance().mc;
  Mutex::Lock lock(mc.mutex_);

  drive_circle_t& arg = extra::reinterpret_reference<drive_circle_t>(param);
  return mc.driveCircle(MotionControl::Pose(arg.x / 1000., arg.y / 1000., 0.),
                        MotionControl::Pose(arg.x_m / 1000., arg.y_m / 1000., 0.),
                        static_cast<Direction>(arg.dir), SpeedIndex(arg.speed));
}

CAN_MC_RPC_HANDLER(MC_PREPARE, param) {
  MotionControl& mc = SimRobot::getInstance().mc;
  Mutex::Lock lock(mc.mutex_);

  UNUSED(param);

  if (SimRobot::getInstance().getTeamColor() == TeamColor_Right) {
    mc.pose_.x = SPOS_RIGHT_X;
    mc.pose_.y = SPOS_RIGHT_Y;
    mc.pose_.phi = SPOS_RIGHT_PHI * 180./M_PI;
  } else {
    mc.pose_.x = SPOS_LEFT_X;
    mc.pose_.y = SPOS_LEFT_Y;
    mc.pose_.phi = SPOS_LEFT_PHI * 180./M_PI;
  }

  return 0;
}
