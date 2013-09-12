#ifndef MC_INTERFACE_H
#define MC_INTERFACE_H

#include <list>
#include <memory>
#include <QPen>
#include <QStyleOptionGraphicsItem>

#include <utils/types.h>
#include <global/eb13.h>
#include <interfaces/robot-interface.h>
#include <utils/thread.h>

#include "drive.h"

class MotionControl {
public:
  struct Pose {
    COPYABLE(Pose);
    MOVABLE(Pose);

    Pose(qreal x, qreal y, qreal phi) :
      x(x), y(y), phi(phi)
    { }

    Pose() :
      x(0.), y(0.), phi(0.)
    { }

    qreal x;
    qreal y;
    qreal phi;
  };

  struct Velocity {
    qreal trans;
    qreal rot;
  };

  class Job {
  public:
    virtual ~Job() {}

    virtual bool advance(Pose& pos, unsigned delta_t) = 0;
    virtual Velocity getVelocity() const = 0;
  };


  static const int SLEEP_MS = 50;

  static const double VELOCITY[3];  // mm / s
  static const double ROTATION[3]; // deg / s

  void run();
  void drive();
  void circle();
  void exit();

  Pose advance(unsigned delta_t);

  std::list<std::unique_ptr<Job>> waitung_jobs_;
  std::unique_ptr<Job> current_job_;
  Pose pose_;
  bool notify_;
  mutable SystemControl::Mutex mutex_;

  int driveToPose(Pose dest, SystemControl::Direction dir, SystemControl::SpeedIndex speed);
  int driveCircle(Pose dest, Pose m, SystemControl::Direction dir, SystemControl::SpeedIndex speed);
  int stop();
  Velocity getVelocity() const;
  Pose getPose() const;

  int sc_id;

  QPen pen;

  MotionControl();

  static
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *);
};

void mc_paint(QPainter *painter, const QStyleOptionGraphicsItem *);

#endif // MC_INTERFACE_H
