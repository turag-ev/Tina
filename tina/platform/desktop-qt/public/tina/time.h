#ifndef TINA_SIM_TIME_H
#define TINA_SIM_TIME_H

#include <tina/tina.h>
#include "timetype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  TuragSystemTicks value;
} TuragSystemTime;

// only for intern use!!!
static _always_inline
TuragSystemTime _turag_ticks_to_time(TuragSystemTicks ticks) {
  TuragSystemTime result = {ticks};
  return result;
}

static _always_inline
TuragSystemTime turag_s_to_ticks(int s) {
  return _turag_ticks_to_time(1000 * s);
}

static _always_inline
TuragSystemTime turag_ms_to_ticks(int ms) {
  return _turag_ticks_to_time(ms);
}

static _always_inline
TuragSystemTime turag_us_to_ticks(int us) {
  return _turag_ticks_to_time(us/1000);
}

static _always_inline
unsigned turag_ticks_to_s(TuragSystemTime time) {
  return time.value / 1000;
}

static _always_inline
unsigned turag_ticks_to_ms(TuragSystemTime time) {
  return time.value;
}

static _always_inline
unsigned turag_ticks_to_us(TuragSystemTime time) {
  return time.value * 1000;
}


extern uint64_t turag_plattform_dependent_get_tick(void);

/// Get number of sys ticks since system start
/**
 * \return sys ticks
 */
static _always_inline
TuragSystemTime turag_get_current_tick(void) { // [tick]
  return _turag_ticks_to_time(turag_plattform_dependent_get_tick());
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_SIM_TIME_H

