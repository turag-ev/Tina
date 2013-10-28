#ifndef TURAG_ECOS_CTIME_H
#define TURAG_ECOS_CTIME_H


#include <pkgconf/kernel.h>
#include "timetype.h"
#include <tina/normalize.h>

// old-style eCos time Macros

#define S_TO_TICK(s)    ((cyg_uint64)(s)  * (cyg_uint64)1000000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)
#define MS_TO_TICK(ms)	((cyg_uint64)(ms) * (cyg_uint64)1000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)
#define US_TO_TICK(us) 	((cyg_uint64)(us) * (cyg_uint64)1000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)

#define TICK_TO_S(ticks)	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))
#define TICK_TO_MS(ticks) 	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))
#define TICK_TO_US(ticks)	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))

// tina-style time fucntions

static _always_inline SystemTicks turag_s_to_ticks(int s) 	{ return (SystemTicks)S_TO_TICK(s); }
static _always_inline SystemTicks turag_ms_to_ticks(int ms) 	{ return (SystemTicks)MS_TO_TICK(ms); }
static _always_inline SystemTicks turag_us_to_ticks(int us)	{ return (SystemTicks)US_TO_TICK(us); }
static _always_inline unsigned turag_ticks_to_s(SystemTicks ticks)	{ return (unsigned)TICK_TO_S(ticks); }
static _always_inline unsigned turag_ticks_to_ms(SystemTicks ticks)	{ return (unsigned)TICK_TO_MS(ticks); }
static _always_inline unsigned turag_ticks_to_us(SystemTicks ticks)	{ return (unsigned)TICK_TO_US(ticks); }

static _always_inline SystemTicks turag_get_current_tick(void) { return (SystemTicks)cyg_current_time(); }


#endif // TURAG_ECOS_CTIME_H

