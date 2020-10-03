#ifndef PLATFORM_ROS_INCLUDE_TINA_TIME_H
#define PLATFORM_ROS_INCLUDE_TINA_TIME_H

#include <tina/tina.h>
#include "timetype.h"

#ifdef __cplusplus
#include <ros/ros.h>
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
TuragSystemTime turag_s_to_ticks(unsigned s) {
  return turag_ticks_to_time((TuragSystemTicks)s * 1000ull * 1000ull * 1000ull);
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_ms_to_ticks(unsigned ms) {
  return turag_ticks_to_time((TuragSystemTicks)ms * 1000ull * 1000ull);
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
TuragSystemTime turag_us_to_ticks(unsigned us) {
  return turag_ticks_to_time((TuragSystemTicks)us * 1000ull);
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_s(TuragSystemTime time) {
  return time.value / 1000 / 1000 / 1000;
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_ms(TuragSystemTime time) {
  return time.value / 1000 / 1000;
}

static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_ticks_to_us(TuragSystemTime time) {
  return time.value / 1000;
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
#ifdef __cplusplus
static TURAG_ALWAYS_INLINE
TuragSystemTime turag_get_current_tick(void) { // [tick]
  return turag_ticks_to_time(ros::Time::now().toNSec());
}
#endif //__cplusplus

/// Frequenz der plattformabhängigen Ticks
static TURAG_ALWAYS_INLINE TURAG_CONSTEXPR_FUNC
unsigned turag_get_systick_frequency(void) {
    return 1000 * 1000 * 1000;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PLATFORM_ROS_INCLUDE_TINA_TIME_H

