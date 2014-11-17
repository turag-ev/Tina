#include "time.h"

static TuragSystemTime system_time;

TuragSystemTime turag_get_current_tick(void) {
  return system_time;
}

void turag_set_current_tick(TuragSystemTime new_system_time) {
  system_time = new_system_time;
}
