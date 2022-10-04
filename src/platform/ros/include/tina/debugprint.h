#ifndef PLATFORM_ROS_INCLUDE_TINA_DEBUGPRINT_H
#define PLATFORM_ROS_INCLUDE_TINA_DEBUGPRINT_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void turag_debug_puts(const char* s);
void turag_debug_vprintf(const char* fmt, va_list vargs);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PLATFORM_ROS_INCLUDE_TINA_DEBUGPRINT_H
