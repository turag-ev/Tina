#ifndef TINA_CHIBIOS_DEBUGPRINT_H
#define TINA_CHIBIOS_DEBUGPRINT_H

#include <stdarg.h>
#include <tina/helper/macros.h>

TURAG_C_API_BEGIN

void turag_debug_puts(const char* s);
void turag_debug_vprintf(const char* fmt, va_list vargs);

TURAG_C_API_END

#endif // TINA_CHIBIOS_DEBUGPRINT_H
