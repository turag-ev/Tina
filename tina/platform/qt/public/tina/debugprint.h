#ifndef TINA_SIM_DEBUGPRINT_H
#define TINA_SIM_DEBUGPRINT_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void turag_debug_puts(const char* s);
void turag_debug_vprintf(const char* fmt, va_list vargs);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_SIM_DEBUGPRINT_H
