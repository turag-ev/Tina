#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <utils/timetype.h>
#include <limits>
#include <systemcontrol.h>

namespace SystemControl {

class Watchdog {
public:
  enum {
    event_wuff = EventNamespace('W','d','g')
  };

  constexpr
  Watchdog() :
    last_move_(),
    interval_(TS_WATCHDOG_TIMEOUT)
  { }

  void update(bool reset = false);
  void reset();
  void setInterval(SystemTime d);

private:
  SystemTime last_move_;
  SystemTime interval_;
};

extern Watchdog watchdog;

} // namespace SystemControl

#endif // WATCHDOG_H
