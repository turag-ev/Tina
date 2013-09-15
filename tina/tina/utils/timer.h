#ifndef TIME_H
#define TIME_H

#include <utils/time.h>

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

#endif // TIME_H
