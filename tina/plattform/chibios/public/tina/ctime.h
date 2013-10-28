#ifndef TURAG_CHIBI_CTIME_H
#define TURAG_CHIBI_CTIME_H

#include <ch.h>
#include "timetype.h"
#include <tina/normalize.h>

static _always_inline SystemTicks turag_s_to_ticks(int s) {
  return (SystemTicks)S2ST(s);
}

static _always_inline SystemTicks turag_ms_to_ticks(int ms) {
  return (SystemTicks)MS2ST(ms);
}

static _always_inline SystemTicks turag_us_to_ticks(int us) {
  return (SystemTicks)US2ST(us);
}

static _always_inline unsigned turag_ticks_to_s(SystemTicks ticks) {
  return (unsigned)(ticks / CH_FREQUENCY);
}

static _always_inline unsigned turag_ticks_to_ms(SystemTicks ticks) {
  return (unsigned)(((ticks - 1L) * 1000L) / CH_FREQUENCY + 1L);
}

static _always_inline unsigned turag_ticks_to_us(SystemTicks ticks) {
  return (unsigned)(((ticks - 1L) * 1000000L) / CH_FREQUENCY + 1L);
}

/// Get number of sys ticks since system start
/**
 * \return sys ticks
 */
static _always_inline SystemTicks turag_get_current_tick(void) { // [tick]
  return (SystemTicks)chTimeNow();
}


#endif // TURAG_CHIBI_CTIME_H

