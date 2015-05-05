#ifndef TINA_DEBUG_INTERNALDEBUG_H
#define TINA_DEBUG_INTERNALDEBUG_H

#include <tina/debugprint.h>
#include <tina/debug/print.h>
#include "defines.h"

#ifndef __DOXYGEN__

#define TURAG_INTERNAL_LOG_SOURCE "~"

#if TURAG_DEBUG_LEVEL > 0
# if TURAG_PRINT_GAMETIME_AUTOMATIC == 1
#  define turag_internal_errorf(format, args...) \
	do{ \
	if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }              \
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_INTERNAL_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args); \
	}while(0)
#  define turag_internal_error(msg) \
	do{ \
	if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
	turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_INTERNAL_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE); \
	}while(0)
# else
#  define turag_internal_errorf(format, args...) \
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_INTERNAL_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
#  define turag_internal_error(msg) \
	turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_INTERNAL_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE)
# endif
#else
# define turag_internal_errorf(format, args...) while(0)
# define turag_internal_error(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 0
# if TURAG_PRINT_GAMETIME_AUTOMATIC == 1
#  define _turag_print_error(msg) \
	do{ \
	if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
	turag_debug_puts(msg); \
	}while(0)
# else
#  define _turag_print_error(msg) \
	turag_debug_puts(msg)
# endif
#else
# define _turag_print_error(msg) while(0)
#endif

#endif // __DOXYGEN__

#endif // TINA_DEBUG_INTERNALDEBUG_H
