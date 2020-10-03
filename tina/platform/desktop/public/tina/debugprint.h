#ifndef PLATFORM_DESKTOP_PUBLIC_TINA_DEBUGPRINT_H
#define PLATFORM_DESKTOP_PUBLIC_TINA_DEBUGPRINT_H


#ifdef QT_CORE_LIB

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void turag_debug_puts(const char* s);
void turag_debug_vprintf(const char* fmt, va_list vargs);

#ifdef __cplusplus
} // extern "C"
#endif

#else // #ifdef QT_CORE_LIB

#include <stdio.h>

#define turag_debug_puts puts
#define turag_debug_vprintf vprintf

#endif // #ifdef QT_CORE_LIB


#endif // PLATFORM_DESKTOP_PUBLIC_TINA_DEBUGPRINT_H
