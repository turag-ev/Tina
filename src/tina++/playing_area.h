#ifndef TINAPP_PLAYING_AREA_H
#define TINAPP_PLAYING_AREA_H

#include <tina++/units.h>

namespace TURAG {

static constexpr Units::Length PLAYING_AREA_X_MIN = -1500.0f * Units::mm;
static constexpr Units::Length PLAYING_AREA_X_MAX = 1500.0f * Units::mm;
static constexpr Units::Length PLAYING_AREA_Y_MIN = 0.0f * Units::mm;
static constexpr Units::Length PLAYING_AREA_Y_MAX = 2000.0f * Units::mm;
static constexpr Units::Length PLAYING_AREA_WIDTH = PLAYING_AREA_X_MAX - PLAYING_AREA_X_MIN;
static constexpr Units::Length PLAYING_AREA_HEIGHT = PLAYING_AREA_Y_MAX - PLAYING_AREA_Y_MIN;

static constexpr Units::Angle PLAYING_AREA_X_MIN_NORMAL_PHI = M_PIf * Units::rad;
static constexpr Units::Angle PLAYING_AREA_X_MAX_NORMAL_PHI = 0 * Units::rad;
static constexpr Units::Angle PLAYING_AREA_Y_MIN_NORMAL_PHI = -M_PI_2f * Units::rad;
static constexpr Units::Angle PLAYING_AREA_Y_MAX_NORMAL_PHI = M_PI_2f * Units::rad;

} // namespace TURAG

#endif // TINAPP_PLAYING_AREA_H
