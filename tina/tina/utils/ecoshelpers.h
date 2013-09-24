#ifndef COMMON_UTIL_ECOSHELPERS_H
#define COMMON_UTIL_ECOSHELPERS_H

#ifndef SIMULATION

#include <pkgconf/kernel.h>

#define S_TO_TICK(s)    ((cyg_uint64)(s)  * (cyg_uint64)1000000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)
#define MS_TO_TICK(ms)	((cyg_uint64)(ms) * (cyg_uint64)1000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)
#define US_TO_TICK(us) 	((cyg_uint64)(us) * (cyg_uint64)1000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR / (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR)

#define TICK_TO_S(ticks)	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))
#define TICK_TO_MS(ticks) 	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))
#define TICK_TO_US(ticks)	(((cyg_uint64)(ticks) * (cyg_uint64)CYGNUM_HAL_RTC_NUMERATOR) / ((cyg_uint64)1000 * (cyg_uint64)CYGNUM_HAL_RTC_DENOMINATOR))

#else

#define S_TO_TICK(s)    (s * 1000)
#define MS_TO_TICK(ms)  (ms)
#define US_TO_TICK(us)  (us / 1000)

#define TICK_TO_S(ticks)	(ticks / 1000)
#define TICK_TO_MS(ticks) (ticks)
#define TICK_TO_US(ticks)	(ticks * 1000)

#endif


#endif // COMMON_UTIL_ECOSHELPERS_H

