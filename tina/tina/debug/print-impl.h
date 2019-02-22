#ifndef PRINTIMPL_H
#define PRINTIMPL_H

#include "defines.h"

TURAG_C_API_BEGIN

#ifndef __DOXYGEN__

// turag_debug_* Implementation wählen
#if TURAG_DEBUG_LEVEL > 0

// gibt im Unterschied zu turag_debug_puts in Regelmäßigen Abständen die Spielzeit aus
void turag_log_puts(const char* s);

// gibt im Unterschied zu turag_debug_printf in Regelmäßigen Abständen die Spielzeit aus
void turag_log_printf(const char* fmt, ...) __attribute__ ((format(printf, 1, 2)));

void turag_debug_printf(const char* fmt, ...) __attribute__ ((format(printf, 1, 2)));

# if TURAG_PRINT_GAMETIME_AUTOMATIC
#  define turag_debug_puts_impl turag_log_puts
#  define turag_debug_printf_impl turag_log_printf
# else // TURAG_PRINT_GAMETIME_AUTOMATIC
#  define turag_debug_puts_impl turag_debug_puts
#  define turag_debug_printf_impl turag_debug_printf
# endif // TURAG_PRINT_GAMETIME_AUTOMATIC

#else  // TURAG_DEBUG_LEVEL > 0

# define turag_debug_puts_impl(...) TURAG_MACRO_NOOP
# define turag_debug_printf_impl(...) TURAG_MACRO_NOOP
# define turag_log_puts(...) TURAG_MACRO_NOOP
# define turag_log_printf(...) TURAG_MACRO_NOOP
# define turag_debug_printf(...) TURAG_MACRO_NOOP

#endif  //  TURAG_DEBUG_LEVEL > 0

#endif // __DOXYGEN __

TURAG_C_API_END

#endif // PRINTIMPL_H

