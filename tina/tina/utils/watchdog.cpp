#include "watchdog.h"
#include <utils/time.h>
#include "eventqueue.h"

namespace SystemControl {

Watchdog watchdog;

void Watchdog::setInterval(SystemTime d) {
  interval_ = d;
}

void Watchdog::update(bool reset) {
  if (reset) {
    last_move_ = get_current_tick();
  } else {
    if (get_current_tick() - last_move_ > interval_) {
      EventQueue::push(Watchdog::event_wuff);
      last_move_ = get_current_tick();
    }
  }
}

void Watchdog::reset() {
  last_move_ = get_current_tick();
}

} // namespace SystemControl
