/**
 * \file tina/debug.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief Utils for printing debug information
 *
 */

#ifndef TINA_DEBUG_H
#define TINA_DEBUG_H

#include <tina/debug/print.h>
#include <tina/debug/graph.h>

#if TURAG_DEBUG_LEVEL > 0
# define system_printf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX ";;" format TURAG_DEBUG_NEWLINE, ##args)
# define system_print(msg)  \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX ";;" msg TURAG_DEBUG_NEWLINE)
#else
# define system_printf(format, args...) while(0)
# define system_print(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 0
# define errorf(format, args...)  \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define error(msg)  \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define errorf(format, args...) while(0)
# define error(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 1
# define criticalf(format, args...)  \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define critical(msg)  \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define criticalf(format, args...) while(0)
# define critical(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 2
# define warningf(format, args...)  \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX format TURAG_DEBUG_NEWLINE,  ##args)
# define warning(msg)  \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define warningf(format, args...) while(0)
# define warning(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 3
# define infof(format, args...)  \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define info(msg)  \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define infof(format, args...) while(0)
# define info(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 4
# define debugf(format, args...)  \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define debug(msg)  \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define debugf(format, args...) while(0)
# define debug(msg) while(0)
#endif

#define bool_string(b) ((b)?"true":"false")
#define bool_string_de(b) ((b)?"ja":"nein")

#endif // TINA_DEBUG_H

