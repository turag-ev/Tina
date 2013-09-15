#define LOG_SOURCE "I"
#include <utils/debug.h>

#include "im_interface.h"

#include <utils/extra/utils.h>
#include <utils/extra/units.h>
#include <utils/eventqueue.h>
#include <im/internal_mechanics.h>
#include <im/bluetooth.h>
#include <pitch.h>

using namespace SystemControl;
using namespace Units;

////////////////////////////////////////////////////////////////////////////////
// InternMechanics

namespace IM {

void blow_balloon() { }
void stop_blow_balloon() { }

void glasses_all_stop() {
    glasses_left_stop();
    glasses_right_stop();
}

static int tmpGlassCnt[2] = {0, 0};


static bool glasses_left_active = false;
void glasses_left_start() {
  glasses_left_active = true;
  infof("Glasses left initialized.");
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_INIT_TIME),
                              IM::event_glasses_left_initialized);
}
void glasses_left_stop() {
  infof("Glasses left shut down.");
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_INIT_TIME),
                              IM::event_glasses_left_shut_down);
  glasses_left_active = false;
}
bool glasses_left_is_active() {
  return glasses_left_active;
}

static bool glasses_right_active = false;
void glasses_right_start() {
  glasses_right_active = true;
  infof("Glasses right initialized.");
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_INIT_TIME),
                              IM::event_glasses_right_initialized);
}
void glasses_right_stop() {
  glasses_right_active = false;
  infof("Glasses right shut down.");
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_INIT_TIME),
                              IM::event_glasses_right_shut_down);
}
bool glasses_right_is_active() {
  return glasses_right_active;
}

void glasses_release_left(int lvl, int glassCnt, bool onPlate) {
    infof("moving %d Glasses left on level %d on plate %s",
          glassCnt,
          lvl,
          bool_string(onPlate));
    tmpGlassCnt[0] = glassCnt;
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_INIT_TIME),
                                IM::event_glasses_left_release_initialized);

}

void glasses_release_left_start() {
    infof("releasing glassses left");
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_REL_TIME),
                                IM::event_glasses_left_release_shut_down,
                                tmpGlassCnt[0]);
    tmpGlassCnt[0] = 0;
}

void glasses_release_right(int lvl, int glassCnt, bool onPlate) {
    infof("moving %d Glasses right on level %d on plate %s",
          glassCnt,
          lvl,
          bool_string(onPlate));
    tmpGlassCnt[1] = glassCnt;
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_INIT_TIME),
                                IM::event_glasses_right_release_initialized);
}

void glasses_release_right_start() {
    infof("releasing glassses right");
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_REL_TIME),
                                IM::event_glasses_right_release_shut_down,
                                tmpGlassCnt[1]);
    tmpGlassCnt[1] = 0;
}

void glasses_all_start() {
    infof("Starting All Glasses");
    glasses_left_start();
    glasses_right_start();
}

void glasses_release_all(int levelL, int amountL, int levelR, int amountR) {
    infof("preparing all Glasses:");
    glasses_release_left(levelL, amountL);
    glasses_release_right(levelR, amountR);
}

void glasses_release_all_start() {
    infof("releasing all Glasses:");
    glasses_release_left_start();
    glasses_release_right_start();
}

static bool cherry_shoot_active = false;
void cherry_shoot_enable() {
  cherry_shoot_active = true;
  infof("IM: cherry shoot enabled");
    EventQueue::pushTimedelayed(ms_to_ticks(IM_CHERRY_INIT_TIME),
                                IM::event_cherry_shoot_initialized);
    pitch.shootCherrys(pitch.getCherrysInRobot());
    EventQueue::pushTimedelayed(ms_to_ticks(IM_CHERRY_SHOOT_TIME),
                                IM::event_cherries_empty);
}
void cherry_shoot_disable() {
  cherry_shoot_active = false;
  infof("IM: cherry shoot disabled");
  EventQueue::removeTimedelayed(IM::event_cherries_empty);
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CHERRY_INIT_TIME),
                              IM::event_cherry_shoot_shut_down);
}
bool cherry_shoot_is_active() {
  return cherry_shoot_active;
}

static bool cherry_collect_active = false;

void cherry_collect_start() {
  cherry_collect_active = false; //true;
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CHERRY_INIT_TIME),
                              IM::event_cherries_initialized);
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CHERRY_INIT_TIME + 2000),
                              IM::event_cherry_bucket_emptied);
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CHERRY_INIT_TIME + 2500),
                              IM::event_cherries_robot_is_ready_to_drive_on);
}

void cherry_collect_stop() {
  cherry_collect_active = false;
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CHERRY_INIT_TIME),
                              IM::event_cherries_shut_down);
}
bool cherry_collect_is_active() {
  return cherry_collect_active;
}


static bool cherry_search_active = false;
void cherry_search_start() {
  cherry_search_active = true;
    EventQueue::pushTimedelayed(ms_to_ticks(500),
    /* */                            IM::event_cherry_bucket_both_sensors); //event_cherry_bucket_only_left_sensor);
}

void cherry_search_stop() {
  cherry_search_active = false;
}

bool cherry_search_is_active() {
  return cherry_search_active;
}

static bool cherry_extended_collect_active = false;
void cherry_extended_collect_start() {
  cherry_extended_collect_active = true;
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_INIT_TIME),
                              IM::event_cherries_initialized);
}
void cherry_extended_request_collection() {
  // TODO
}
void cherry_extended_collect_stop() {
  cherry_extended_collect_active = false;
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GLAS_INIT_TIME),
                              IM::event_cherries_shut_down);
}
bool cherry_extended_is_active() {
  return cherry_extended_collect_active;
}

static bool candles_left_active = false;

void candles_left_start() {
  candles_left_active = true;
  infof("IM: candles left initialized.");
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CANDLE_INIT_TIME),
                              IM::event_candles_left_initialized);
}
void candles_left_stop() {
  candles_left_active = false;
  infof("IM: candles left stopped");
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CANDLE_INIT_TIME),
                              IM::event_candles_left_shut_down);
}
bool candles_left_is_active() {
  return candles_left_active;
}

static bool candles_right_active = false;
void candles_right_start() {
  candles_right_active = true;
  infof("IM: candles right initialized");
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CANDLE_INIT_TIME),
                              IM::event_candles_right_initialized);
}
void candles_right_stop() {
  candles_right_active = false;
  infof("IM: candles right stopped");
  EventQueue::pushTimedelayed(ms_to_ticks(IM_CANDLE_INIT_TIME),
                              IM::event_candles_right_shut_down);
}
bool candles_right_is_active() {
  return candles_right_active;
}

void candles_left_manual_start() {
    EventQueue::pushTimedelayed(ms_to_ticks(IM_CANDLE_HIT_TIME), IM::event_candles_left_shut_down);
    pitch.blowCandleOut(0);
    infof("IM -> Dem haben wir das Licht ausgeblasen!");
}

void candles_right_manual_start() {
    EventQueue::pushTimedelayed(ms_to_ticks(IM_CANDLE_HIT_TIME), IM::event_candles_right_shut_down);
    pitch.blowCandleOut(0);
    infof("IM -> Dem haben wir das Licht ausgeblasen!");
}


void gifts_right_prepare() {
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_gift_initialized);
    infof("IM -> Geschenkarm rechts vorbereitet");
}

void gifts_left_prepare() {
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_gift_initialized);
    infof("IM -> Geschenkarm links vorbereitet");
}

void gifts_unwrap() {
    //pitch.triggerGift();
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_gift_shut_down);
}

void gifts_exit() {
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_gift_shut_down);

    infof("IM -> Geschenkarm wieder eingeklappt");
}

void manipulator_open() {
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_manipulator_initialized);
    infof("IM -> Zunge ausgeklappt");
}

void manipulator_close() {
    EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_manipulator_shut_down);
    infof("IM -> Zunge ausgeklappt");
}


static Pose startpose;

static bool gift_auto_active = false;

void gifts_auto_right_start(void) {
  startpose = g.robot.pose;
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_gift_initialized);
  gift_auto_active = true;
  infof("IM -> Geschenkarm rechts vorbereitet");
}

void gifts_auto_left_start(void) {
  startpose = g.robot.pose;
  gift_auto_active = true;
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_gift_initialized);
  infof("IM -> Geschenkarm links vorbereitet");
}

void gifts_auto_stop(void) {
  gift_auto_active = false;
  Length farben_blau[4] = {986*mm, 386*mm, -214*mm, -814*mm};
  Length farben_rot[4] = {-986*mm, -386*mm, 214*mm, 814*mm};

  for (int i=0;i<4;i++)
  {
      if (g.getTeamColor() == TeamColor_Left){
          if ((farben_rot[i] >= startpose.x && farben_rot[i] <= g.robot.pose.x) ||
              (farben_rot[i] <= startpose.x && farben_rot[i] >= g.robot.pose.x)){
              infof("IM -> rotes Geschenk %d ausgelöst", i);
              pitch.triggerGift(i);
          }
      }
      else{
          if ((farben_blau[i] >= startpose.x && farben_blau[i] <= g.robot.pose.x) ||
              (farben_blau[i] <= startpose.x && farben_blau[i] >= g.robot.pose.x)){
              infof("IM -> blaues Geschenk %d ausgelöst", i);
              pitch.triggerGift(i);
          }
      }
  }
  EventQueue::pushTimedelayed(ms_to_ticks(IM_GIFT_INIT_TIME), IM::event_gift_shut_down);
  infof("IM -> Geschenkarm deaktiviert");
}

bool gifts_auto_is_active() {
  return gift_auto_active;
}

void finalShutdown() {
    infof("IM: It's the final Shutdown");
}

bool isGlasservoOpen() {
  return false;
}

IM_Info_t im_info;

} // namespace IM

////////////////////////////////////////////////////////////////////////////////
// LegoBot

namespace bluetooth {

void init() { }

LegoBot::LegoBot() { }
bool LegoBot::makeTone(uint16_t frequency, uint16_t duration)   { return true; }
bool LegoBot::sendText(uint8_t box, uint8_t* data, uint8_t len) { return true; }
bool LegoBot::drivePath(int n, Point* points)                   { return true; }
bool LegoBot::stop()                                            { return true; }
bool LegoBot::driveTo(Point point)                              { return true; }
bool LegoBot::doGifts()                                         { return true; }
bool LegoBot::updateBlackboard(TeamColor_t colour, Point big_rob, Point lego_rob, Point enemy_a, Point enemy_b) { return true;}
bool LegoBot::gameOver()                                        {	return true; }
LegoBot* LegoBot::getLegoBot() {
  static LegoBot bot;
  return &bot;
}

}
