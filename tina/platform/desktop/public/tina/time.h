#ifndef PLATFORM_DESKTOP_PUBLIC_TINA_TIME_H
#define PLATFORM_DESKTOP_PUBLIC_TINA_TIME_H

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
static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_ticks_to_time(TuragSystemTicks ticks) {
#ifndef __cplusplus
  TuragSystemTime result = {ticks};
  return result;
#else
  return {ticks};
#endif
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_s_to_ticks(int s) {
  return turag_ticks_to_time(1000 * s);
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_ms_to_ticks(int ms) {
  return turag_ticks_to_time(ms);
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_us_to_ticks(int us) {
  return turag_ticks_to_time(us/1000);
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_s(TuragSystemTime time) {
  return time.value / 1000;
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_ms(TuragSystemTime time) {
  return time.value;
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_us(TuragSystemTime time) {
  return time.value * 1000;
}

#ifndef UINT64_MAX
#define UINT64_MAX     18446744073709551615ULL
#endif

#define TURAG_TIME_IMMEDIATE_TICKS 0ULL
#define TURAG_TIME_IMMEDIATE turag_ticks_to_time(0ULL)

#define TURAG_TIME_INFINITE_TICKS UINT64_MAX
#define TURAG_TIME_INFINITE turag_ticks_to_time(UINT64_MAX)

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

static TURAG_ALWAYS_INLINE
TuragSystemTime turag_get_current_tick(void) { // [tick]
  return turag_ticks_to_time(turag_platform_dependent_get_tick());
}

/// Frequenz der plattformabhängigen Ticks
static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_get_systick_frequency(void) {
    return turag_s_to_ticks(1).value;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PLATFORM_DESKTOP_PUBLIC_TINA_TIME_H

