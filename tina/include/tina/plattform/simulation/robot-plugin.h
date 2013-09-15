#ifndef ROBOTPLUGIN_H
#define ROBOTPLUGIN_H

#include <QtPlugin>
#include <QGraphicsItem>
#include <QString>
#include <atomic>

#include <global/eb13.h>
#include <interfaces/robot-interface.h>
#include <utils/thread.h>

#include "mc_interface.h"

class SimRobot : public QObject, public QGraphicsItem, public RobotInterface {
  Q_OBJECT
  Q_INTERFACES(RobotInterface)
  Q_INTERFACES(QGraphicsItem)
#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "SimEuroBot.Robot")
#endif
public:
  SimRobot();
  virtual ~SimRobot();

  // RobotInterface
  bool init(SimRobotInterface* iface, unsigned time_step) override;
  void prepare(TeamColor_t team_color) override;
  void start() override;
  QString getName() override;
  Model getModel() override;
  LayerList getLayers() override;
  StrategiesList getStrategies() override;
  void setStrategy(int strategy, bool user_action) override;
  const char **getLogSources() override;
  void stop() override;
  void pause() override;
  void resume() override;
  void unload() override;

  // QGraphicsItem
  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
  void advance(int step) override;

  void getOtherRobots(uint id, bb_oppo_state_t *oppo_state) const;
  PoseF getPose();
  void getVelocity(double* dx_dt, double* dphi_dt);

  TeamColor_t setTeamColor(TeamColor_t team_color) {
    // FIXME: mutex nötig
    return team_color_ = team_color; }
  TeamColor_t getTeamColor() const { return team_color_; }

  uint64_t getTick() { return simtime_; }

  static const qreal WIDTH;
  static const qreal HEIGHT;

  void log(QString&& msg) {
    interface_->log(std::move(msg));
  }

  static SimRobot& getInstance() {
    return *instance_;
  }

  static SimRobotInterface& getInterface() {
    return*(instance_->interface_);
  }

public:
  MotionControl mc;

private:
  static SimRobot* instance_;
  SimRobotInterface* interface_;

  TeamColor_t team_color_;

  mutable SystemControl::Mutex mutex_;

  bool inited_;
  bool paused_;

  mutable SystemControl::Mutex other_mutex_;
  SimRobotInterface::RobotsPhysics other_data;

  QPixmap image_;

  std::atomic<uint64_t> simtime_;
  unsigned time_step_;

private:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
};

#endif // ROBOTPLUGIN_H
