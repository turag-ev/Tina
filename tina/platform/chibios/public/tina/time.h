#ifndef TINA_CHIBIOS_TIME_H
#define TINA_CHIBIOS_TIME_H

#include <ch.h>

#include <tina/tina.h>
#include "timetype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  TuragSystemTicks value;
} TuragSystemTime;

// only for intern use!!!
static _always_inline _constexpr_func
TuragSystemTime _turag_ticks_to_time(TuragSystemTicks ticks) {
#ifndef __cplusplus
  TuragSystemTime result = {ticks};
  return result;
#else
  return {ticks};
#endif
}

static _always_inline _constexpr_func
TuragSystemTime turag_s_to_ticks(int s) {
  return _turag_ticks_to_time(S2ST(s));
}

static _always_inline _constexpr_func
TuragSystemTime turag_ms_to_ticks(int ms) {
	return ms == 0 ? _turag_ticks_to_time(0) : _turag_ticks_to_time(MS2ST(ms));
}

static _always_inline _constexpr_func
TuragSystemTime turag_us_to_ticks(int us) {
	return us == 0 ? _turag_ticks_to_time(0) : _turag_ticks_to_time(US2ST(us));
}

static _always_inline _constexpr_func
unsigned turag_ticks_to_s(TuragSystemTime time) {
  return (time.value / CH_FREQUENCY);
}

static _always_inline _constexpr_func
unsigned turag_ticks_to_ms(TuragSystemTime time) {
	return time.value == 0 ? 0 : (((time.value - 1L) * 1000L) / CH_FREQUENCY + 1L);
}

static _always_inline _constexpr_func
unsigned turag_ticks_to_us(TuragSystemTime time) {
	return time.value == 0 ? 0 : (((time.value - 1L) * 1000000L) / CH_FREQUENCY + 1L);
}

#define TURAG_TIME_INFINITE_TICKS TIME_INFINITE
#define TURAG_TIME_INFINITE _turag_ticks_to_time(TIME_INFINITE)

/// Get number of sys ticks since system start
/**
 * \return sys ticks
 */
static _always_inline
TuragSystemTime turag_get_current_tick(void) { // [tick]
  return _turag_ticks_to_time(chTimeNow());
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_CHIBIOS_TIME_H

