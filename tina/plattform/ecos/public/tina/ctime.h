#ifndef TURAG_ECOS_CTIME_H
#define TURAG_ECOS_CTIME_H

#include <pkgconf/kernel.h>
#include "timetype.h"
#include <tina/normalize.h>

#ifdef __cplusplus
extern "C" {
#endif

// old-style eCos time Macros

#define S_TO_TICK(s)    ((cyg_uint64)(s)  * (cyg_uint64)1000000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)
#define MS_TO_TICK(ms)	((cyg_uint64)(ms) * (cyg_uint64)1000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)
#define US_TO_TICK(us) 	((cyg_uint64)(us) * (cyg_uint64)1000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)

#define TICK_TO_S(ticks)	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))
#define TICK_TO_MS(ticks) 	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))
#define TICK_TO_US(ticks)	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))

// tina-style time fucntions

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
  return _turag_ticks_to_time(S_TO_TICK(s));
}

static _always_inline
TuragSystemTime turag_ms_to_ticks(int ms) {
  return _turag_ticks_to_time(MS_TO_TICK(ms));
}

static _always_inline 
TuragSystemTime turag_us_to_ticks(int us)	{
  return _turag_ticks_to_time(US_TO_TICK(us));
}

static _always_inline
unsigned turag_ticks_to_s(TuragSystemTime time)	{
  return TICK_TO_S(time.value);
}

static _always_inline
unsigned turag_ticks_to_ms(TuragSystemTime time) {
  return TICK_TO_MS(time.value);
}

static _always_inline
unsigned turag_ticks_to_us(TuragSystemTime time) {
  return TICK_TO_US(time.value);
}

static _always_inline
TuragSystemTime turag_get_current_tick(void) {
  return _turag_ticks_to_time(cyg_current_time());
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TURAG_ECOS_CTIME_H

