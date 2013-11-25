/**
 * \file debug.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief Utils for printing debug information
 *
 */

#ifndef TINA_DEBUG_H
#define TINA_DEBUG_H

#include <tina/debugprint.h>

// Debug prefixes
#define ERROR_PREFIX    "!"
#define CRITICAL_PREFIX "!"
#define WARN_PREFIX     "?"
#define INFO_PREFIX     "-"
#define DEBUG_PREFIX    "-"
#define CMENU_PREFIX    "C"

// debug level
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 5
#endif

// log source
#ifndef LOG_SOURCE
# define LOG_SOURCE "_"
#endif

#if !defined(NDEBUG) && DEBUG_LEVEL < 1
# define NDEBUG
#endif

#if DEBUG_LEVEL > 0
# define system_printf(format, args...) debug_printf(";;" format "\n\r", ##args)
# define system_print(msg) debug_puts(";;" msg "\n\r")
#else
# define system_printf(format, args...) while(0)
# define system_print(msg) while(0)
#endif

#if DEBUG_LEVEL > 0
# define errorf(format, args...) debug_printf(LOG_SOURCE ERROR_PREFIX format "\n\r", ##args)
# define error(msg) debug_puts(LOG_SOURCE ERROR_PREFIX msg "\n\r")
#else
# define errorf(format, args...) while(0)
# define error(msg) while(0)
#endif

#if DEBUG_LEVEL > 1
# define criticalf(format, args...) debug_printf(LOG_SOURCE CRITICAL_PREFIX format "\n\r", ##args)
# define critical(msg) debug_puts(LOG_SOURCE CRITICAL_PREFIX msg "\n\r")
#else
# define criticalf(format, args...) while(0)
# define critical(msg) while(0)
#endif

#if DEBUG_LEVEL > 2
# define warningf(format, args...) debug_printf(LOG_SOURCE WARN_PREFIX format "\n\r",  ##args)
# define warning(msg) debug_puts(LOG_SOURCE WARN_PREFIX msg "\n\r")
#else
# define warningf(format, args...) while(0)
# define warning(msg) while(0)
#endif

#if DEBUG_LEVEL > 3
# define infof(format, args...) debug_printf(LOG_SOURCE INFO_PREFIX format "\n\r", ##args)
# define info(msg) debug_puts(LOG_SOURCE INFO_PREFIX msg "\n\r")
#else
# define infof(format, args...) while(0)
# define info(msg) while(0)
#endif

#define menu_printf(format, args...) CMenu_Printf("^" CMENU_PREFIX format "\n\r", ##args)
#define menu_print(msg) CMenu_Printf("^" CMENU_PREFIX msg "\n\r")

#define bool_string(b) ((b)?"true":"false")
#define bool_string_de(b) ((b)?"ja":"nein")

#endif // TINA_DEBUG_H

