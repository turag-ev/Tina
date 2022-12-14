#ifndef PLATFORM_CHIBIOS3_PUBLIC_TINA_TIME_H
#define PLATFORM_CHIBIOS3_PUBLIC_TINA_TIME_H

#include <ch.h>

#include <tina/tina.h>
#include "timetype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TURAG_TIME_IMMEDIATE_TICKS   TIME_IMMEDIATE
#define TURAG_TIME_IMMEDIATE         turag_ticks_to_time(TIME_IMMEDIATE)

#define TURAG_TIME_INFINITE_TICKS   TIME_INFINITE
#define TURAG_TIME_INFINITE         turag_ticks_to_time(TIME_INFINITE)
/* Wrapper defines for ChibiOS 19 */
#ifndef S2ST
#define S2ST TIME_S2I
#endif
#ifndef MS2ST
#define MS2ST TIME_MS2I
#endif
#ifndef US2ST
#define US2ST TIME_US2I
#endif

typedef struct {
  TuragSystemTicks value;
} TuragSystemTime;

// only for intern use!!!
static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_ticks_to_time(TuragSystemTicks ticks) {
#ifndef __cplusplus
  TuragSystemTime result = {ticks};
  return result;
#else
  return {ticks};
#endif
}

/// Frequenz der plattformabhängigen Ticks
static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_get_systick_frequency(void) {
    return CH_CFG_ST_FREQUENCY;
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_s_to_ticks(int s) {
  return turag_ticks_to_time(S2ST(s));
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_ms_to_ticks(int ms) {
	return ms == 0 ? turag_ticks_to_time(0) : turag_ticks_to_time(MS2ST(ms));
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_us_to_ticks(int us) {
	return us == 0 ? turag_ticks_to_time(0) : turag_ticks_to_time(US2ST(us));
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_s(TuragSystemTime time) {
    return (time.value / turag_get_systick_frequency());
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_ms(TuragSystemTime time) {
	return time.value == 0 ? 0 : (((time.value - 1L) * 1000L) / turag_get_systick_frequency() + 1L);
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_us(TuragSystemTime time) {
	return time.value == 0 ? 0 : (((time.value - 1L) * 1000000L) / turag_get_systick_frequency() + 1L);
}

/// Get number of sys ticks since system start
static TURAG_ALWAYS_INLINE
TuragSystemTime turag_get_current_tick(void) { // [tick]
  return turag_ticks_to_time(chVTGetSystemTime());
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PLATFORM_CHIBIOS3_PUBLIC_TINA_TIME_H

