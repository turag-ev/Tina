#include <QPainter>
#include <algorithm>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QPixmap>
#include <QToolTip>

#define LOG_SOURCE "I"
#include <utils/debug.h>

#include <global.h>
#include <utils/debug.h>
#include <utils/eventqueue.h>
#include <utils/extra/packed.h>
#include <opponent.h>
#include <systemcontrol.h>
#include "robot-plugin.h"
#include "graphicslayer.h"
#include <utils/thread.h>
#include <utils/cansc.h>
#include "utils/time.h"
#include <utils/eventqueue.h>
#include <utils/extra/macro-helper.h>
#include <comm/id.h>
#include <drive.h>
#include <global/eb13.h>
#include <systemcontrol.h>
#include <global.h>
#include <pitch.h>
#include <interfaces/transform.h>
#include "colors.h"
#include "mc_interface.h"

using namespace SystemControl;

extern "C"
void debug_puts(const char* s) {
  SimRobot::getInstance().log(s);
}

extern "C"
void debug_printf(const char* fmt, ...) {
  char buffer[4096];

  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  SimRobot::getInstance().log(buffer);
}

SimRobot* SimRobot::instance_;

SimRobot::SimRobot() :
  QGraphicsItem(0),
  interface_(nullptr),
  team_color_(TeamColor_Unknown),
  inited_(false),
  paused_(false),
  time_step_(0)
{
  instance_ = this;

  setPos(mc.pose_.x, mc.pose_.y);
  setRotation(mc.pose_.phi);
//  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemIsSelectable);
  image_.load("../../doc/images/BotA.png");
  setAcceptHoverEvents(true);
}

SimRobot::~SimRobot(){

}

void
SimRobot::getOtherRobots(uint id, bb_oppo_state_t* oppo_state) const {
  Mutex::Lock lock(other_mutex_);

  const PhysicsF* data;
  switch (id) {
  case BB_OPPONENT1_POS:
    data = &other_data.main_opponent;
    break;

  case BB_OPPONENT2_POS:
    data = &other_data.ally_opponent;
    break;

  case BB_ROBO2_STATE:
    data = &other_data.partner;
    break;

  default:
    return;
  }

  oppo_state->pose.x      = data->pose.x;
  oppo_state->pose.y      = data->pose.y;
  oppo_state->velocity.x  = data->velocity.x;
  oppo_state->velocity.y  = data->velocity.y;
}

PoseF
SimRobot::getPose() {
  PoseF pose;
  MotionControl::Pose mc_pose = mc.getPose();
  pose.x = mc_pose.x;
  pose.y = mc_pose.y;
  pose.phi = mc_pose.phi;
  return pose;
}

void
SimRobot::getVelocity(double* dx_dt, double* dphi_dt) {
  Mutex::Lock lock(mutex_);

  MotionControl::Velocity v = mc.getVelocity();
  *dx_dt = v.trans;
  *dphi_dt = v.rot;
}

QRectF SimRobot::boundingRect() const {
  qreal adjust = 0.5;
  return QRectF(-200*SIM_SCALE_MM - adjust, -200*SIM_SCALE_MM - adjust,
                 400*SIM_SCALE_MM + adjust,  400*SIM_SCALE_MM + adjust);
}

QPainterPath SimRobot::shape() const {
  QPainterPath path;
  path.addRect(-200*SIM_SCALE_MM, -200*SIM_SCALE_MM,
                400*SIM_SCALE_MM,  400*SIM_SCALE_MM);
  return path;
}

void SimRobot::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  QToolTip::showText(event->screenPos(), QString("%1 mm / %2 mm / %3°")
             .arg(pos().x())
             .arg(pos().y())
             .arg(rotation()), nullptr);
}

void SimRobot::advance(int step) {
  if (!step || paused_) return;

  // TODO: rotations und längsgeschwindigkeit richtig überlagern
  /*setPos(pose_);
  setRotation(phi_);
  pose_ = mapToParent(speed_ * SIM_SCALE_MM * time_step_, 0);
  phi_ = norm_angle_deg(rotation() + rotation_speed_ * 180./M_PI * time_step_/1000.);
  setPos(pose_);
  setRotation(phi_);*/
  MotionControl::Pose pose = mc.advance(time_step_);
  setPos(pose.x, -pose.y);
  setRotation(-pose.phi);

  // Position der anderen Roboter aktualisieren
  Mutex::Lock other_lock(other_mutex_);
  other_data = SimRobot::getInterface().getOtherRobotPhysics();
  other_lock.unlock();

  simtime_ += time_step_;
}

const qreal SimRobot::WIDTH = 272;
const qreal SimRobot::HEIGHT = 288.;

void SimRobot::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *,
                     QWidget *)
{
  if (!image_.isNull()) {
    // Glässer
    painter->setPen(Qt::black);
    painter->setBrush(Qt::lightGray);

    QFont font = painter->font();
    font.setPointSizeF(font.pointSizeF() * 4);
    painter->setFont(font);

    if (pitch.getGlassesInRobotRight() > 0) {
      painter->drawEllipse(QPointF(120,  92), SIM_SCALE_MM * 40, SIM_SCALE_MM * 40);
      painter->drawText(QPointF(120 - 12, 92 + 16), QString("%1").arg(pitch.getGlassesInRobotRight()));
    }
    if (pitch.getGlassesInRobotLeft() > 0) {
      painter->drawEllipse(QPointF(120, -92), SIM_SCALE_MM * 40, SIM_SCALE_MM * 40);
      painter->drawText(QPointF(120 - 12, -92 + 16), QString("%1").arg(pitch.getGlassesInRobotLeft()));
    }

    // Roboter
    painter->drawPixmap(-WIDTH/2*SIM_SCALE_MM, -HEIGHT/2*SIM_SCALE_MM,
                        WIDTH*SIM_SCALE_MM, HEIGHT*SIM_SCALE_MM, image_);

    // Teamfarbenhütchen
    if (g.getTeamColor() == TeamColor_Left) {
      painter->setBrush(Colors::red);
    } else if (g.getTeamColor() == TeamColor_Right) {
      painter->setBrush(Colors::blue);
    } else {
      painter->setBrush(Qt::gray);
    }
    painter->drawEllipse(QRectF(-24, -24, 48, 48));

    if (pitch.getCherrysInRobot() > 0) {
      painter->setPen(Qt::white);
      painter->drawText(QPointF(-120-8, 0 + 16), QString("%1").arg(pitch.getCherrysInRobot()));
    }
  }
}

bool SimRobot::init(SimRobotInterface* iface, unsigned time_step) {
  interface_ = iface;
  time_step_ = time_step;

  if (!inited_) {
    SystemControl::init();
  } else {
    EventQueue::push(UI::event_start);
  }

  return true;
}

void SimRobot::start() {
  EventQueue::push(UI::event_start);
}

void SimRobot::pause() {
  paused_ = true;

  Thread_pause_all();
}

void SimRobot::resume() {
  paused_ = false;

  Thread_resume_all();
}

QString SimRobot::getName() {
  return "PartyLöwe";
}

RobotInterface::Model SimRobot::getModel() {
  return this;
}

// Layer Funktionen:
namespace SystemControl {
  extern void cd_paint(QPainter*,const QStyleOptionGraphicsItem*);
  extern void pp_paint(QPainter*,const QStyleOptionGraphicsItem*);
  extern void pitch_paint(QPainter*,const QStyleOptionGraphicsItem*);
  extern void astar_paint(QPainter*,const QStyleOptionGraphicsItem*);
}

RobotInterface::LayerList SimRobot::getLayers() {
  LayerList layers;
  layers.append(new GraphicsLayer("Kollisionserkennung", cd_paint));
  layers.append(new GraphicsLayer("Antriebsreglung", mc_paint));
  layers.append(new GraphicsLayer("Pfadplannung", pp_paint));
  layers.append(new GraphicsLayer("Spielfeld", pitch_paint));
  layers.append(new GraphicsLayer("AStar", astar_paint, false));
  return layers;
}

RobotInterface::StrategiesList SimRobot::getStrategies() {
  StrategiesList list;
  for (size_t i = 0; i < strategies_length; i++) {
    list.append(QString::fromUtf8(strategies[i].name));
  }
  return list;
}

void SimRobot::setStrategy(int strategy, bool user_action) {
  EventQueue::push(UI::event_set_strategy, std::size_t(strategy));
}

const char** SimRobot::getLogSources() {
  return log_source_table;
}

void SimRobot::stop() {
  EventQueue::pushToFront(UI::event_stop);
}

namespace SystemControl {
extern Thread<0> sc_thread;
}

void SimRobot::unload() {
  EventQueue::quit();

  if (paused_) {
    Thread_resume_all();
  }

  sc_thread.join();
}

void SimRobot::prepare(TeamColor_t team_color) {
  setTeamColor(team_color);
  EventQueue::push(UI::event_start);
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(robot, SimRobot)
#endif



void sim_prepared() {
  SimRobot::getInterface().setPrepared();
}

uint64_t sim_get_tick() {
  return SimRobot::getInstance().getTick();
}

void sim_breakpoint() {
  SimRobot::getInterface().setPaused();
}
