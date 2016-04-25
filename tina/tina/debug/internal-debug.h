#ifndef TINA_DEBUG_INTERNALDEBUG_H
#define TINA_DEBUG_INTERNALDEBUG_H

#include <tina/debugprint.h>
#include "print-impl.h"
#include "defines.h"

#ifndef __DOXYGEN__

#define TURAG_INTERNAL_LOG_SOURCE "~"

#define turag_internal_errorf(format, args...) \
	turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX TURAG_INTERNAL_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args)

#define turag_internal_error(msg) \
	turag_debug_puts_impl(TURAG_DEBUG_LINE_PREFIX TURAG_INTERNAL_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE)


#if TURAG_DEBUG_LEVEL > 2
# define turag_internal_warningf(format, args...) \
    turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX TURAG_INTERNAL_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX format TURAG_DEBUG_NEWLINE,  ##args)
# define turag_internal_warning(msg) \
    turag_debug_puts_impl(TURAG_DEBUG_LINE_PREFIX TURAG_INTERNAL_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_warningf(format, args...) TURAG_MACRO_NOOP
# define turag_warning(msg) TURAG_MACRO_NOOP
#endif

#define _turag_print_error(msg) \
	turag_debug_puts_impl(msg)

#endif // __DOXYGEN__

#endif // TINA_DEBUG_INTERNALDEBUG_H
