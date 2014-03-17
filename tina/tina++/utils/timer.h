#ifndef TINAPP_UTILS_TIMER_H
#define TINAPP_UTILS_TIMER_H

#include <tina++/time.h>

namespace TURAG {

/// @addtogroup Utils
/// @{

////////////////////////////////////////////////////////////////////////////////
// timer (to stop times)

class Timer {
public:
  Timer() :
    start_time_(get_current_tick())
  { }

  SystemTime elapsed() const {
    return get_current_tick() - start_time_;
  }

private:
  SystemTime start_time_;
};

/// @}

} // namespace TURAG

#endif // TINAPP_UTILS_TIMER_H
