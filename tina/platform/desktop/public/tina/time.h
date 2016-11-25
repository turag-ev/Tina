#ifndef TINA_SIM_TIME_H
#define TINA_SIM_TIME_H

#include <tina/tina.h>
#include "timetype.h"

#ifdef _WIN32
# include <windows.h>
#else
# include <sys/time.h>
#endif

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
  return _turag_ticks_to_time(1000 * s);
}

static _always_inline _constexpr_func
TuragSystemTime turag_ms_to_ticks(int ms) {
  return _turag_ticks_to_time(ms);
}

static _always_inline _constexpr_func
TuragSystemTime turag_us_to_ticks(int us) {
  return _turag_ticks_to_time(us/1000);
}

static _always_inline _constexpr_func
unsigned turag_ticks_to_s(TuragSystemTime time) {
  return time.value / 1000;
}

static _always_inline _constexpr_func
unsigned turag_ticks_to_ms(TuragSystemTime time) {
  return time.value;
}

static _always_inline _constexpr_func
unsigned turag_ticks_to_us(TuragSystemTime time) {
  return time.value * 1000;
}

#ifndef UINT64_MAX
#define UINT64_MAX     18446744073709551615ULL
#endif

#define TURAG_TIME_INFINITE_TICKS UINT64_MAX
#define TURAG_TIME_INFINITE _turag_ticks_to_time(UINT64_MAX)

/// Get number of sys ticks since system start
/**
 * \return sys ticks
 */
#ifdef _WIN32
# define turag_platform_dependent_get_tick GetTickCount
#else
static inline uint64_t turag_platform_dependent_get_tick(void)
{
    struct timeval time_value;
	  gettimeofday(&time_value, NULL);
	  return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}
#endif

static _always_inline
TuragSystemTime turag_get_current_tick(void) { // [tick]
  return _turag_ticks_to_time(turag_platform_dependent_get_tick());
}

/// Frequenz der plattformabhängigen Ticks
static _always_inline _constexpr_func
unsigned turag_get_systick_frequency(void) {
    return turag_s_to_ticks(1).value;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_SIM_TIME_H

