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
#include <tina/config.h>

// tina debug output prefix
#define TURAG_DEBUG_LINE_PREFIX "\x02"

// Debug prefixes
#define ERROR_PREFIX    "!"
#define CRITICAL_PREFIX "!"
#define WARN_PREFIX     "?"
#define INFO_PREFIX     "-"
#define DEBUG_PREFIX    "_"
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
# define system_printf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX ";;" format TURAG_DEBUG_NEWLINE, ##args)
# define system_print(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX ";;" msg TURAG_DEBUG_NEWLINE)
#else
# define system_printf(format, args...) while(0)
# define system_print(msg) while(0)
#endif

#if DEBUG_LEVEL > 0
# define errorf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX LOG_SOURCE ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define error(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX LOG_SOURCE ERROR_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define errorf(format, args...) while(0)
# define error(msg) while(0)
#endif

#if DEBUG_LEVEL > 1
# define criticalf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX LOG_SOURCE CRITICAL_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define critical(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX LOG_SOURCE CRITICAL_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define criticalf(format, args...) while(0)
# define critical(msg) while(0)
#endif

#if DEBUG_LEVEL > 2
# define warningf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX LOG_SOURCE WARN_PREFIX format TURAG_DEBUG_NEWLINE,  ##args)
# define warning(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX LOG_SOURCE WARN_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define warningf(format, args...) while(0)
# define warning(msg) while(0)
#endif

#if DEBUG_LEVEL > 3
# define infof(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX LOG_SOURCE INFO_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define info(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX LOG_SOURCE INFO_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define infof(format, args...) while(0)
# define info(msg) while(0)
#endif

#if DEBUG_LEVEL > 4
# define debugf(format, args...) debug_printf(LOG_SOURCE DEBUG_PREFIX format "\n\r", ##args)
# define debug(msg) debug_puts(LOG_SOURCE DEBUG_PREFIX msg "\n\r")
#else
# define debugf(format, args...) while(0)
# define debug(msg) while(0)
#endif

#define menu_printf(format, args...) CMenu_Printf("^" CMENU_PREFIX format "\r\n", ##args)
#define menu_print(msg) CMenu_Printf("^" CMENU_PREFIX msg "\r\n")

#define bool_string(b) ((b)?"true":"false")
#define bool_string_de(b) ((b)?"ja":"nein")

#endif // TINA_DEBUG_H

