#ifndef TURAG_CHIBI_CTIME_H
#define TURAG_CHIBI_CTIME_H

#include <ch.h>
#include "timetype.h"
#include <tina/normalize.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  TuragSytemTicks value;
} TuragSystemTime;

// only for intern use!!!
_always_inline
TuragSystemTime _turag_ticks_to_time(TuragSystemTicks ticks) {
  TuragSystemTime result = {ticks};
  return result;
}

_always_inline
TuragSystemTime turag_s_to_ticks(int s) {
  return _turag_ticks_to_time(S2ST(s));
}

_always_inline
TuragSystemTime turag_ms_to_ticks(int ms) {
  return _turag_ticks_to_time(MS2ST(ms));
}

_always_inline
TuragSystemTime turag_us_to_ticks(int us) {
  return _turag_ticks_to_time(US2ST(us));
}

_always_inline
unsigned turag_ticks_to_s(TuragSystemTime time) {
  return (time.value / CH_FREQUENCY);
}

_always_inline
unsigned turag_ticks_to_ms(TuragSystemTime time) {
  return (((time.value - 1L) * 1000L) / CH_FREQUENCY + 1L);
}

_always_inline
unsigned turag_ticks_to_us(TuragSystemTime time) {
  return (((time.value - 1L) * 1000000L) / CH_FREQUENCY + 1L);
}

/// Get number of sys ticks since system start
/**
 * \return sys ticks
 */
_always_inline
SystemTicks turag_get_current_tick(void) { // [tick]
  return _turag_ticks_to_time(chTimeNow());
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TURAG_CHIBI_CTIME_H

