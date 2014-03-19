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

#include <tina/debugprint.h>
#include <tina/config.h>

// tina debug output prefix
#define TURAG_DEBUG_LINE_PREFIX "\x02"

// Debug prefixes
#define TURAG_DEBUG_ERROR_PREFIX    "!"
#define TURAG_DEBUG_CRITICAL_PREFIX "!"
#define TURAG_DEBUG_WARN_PREFIX     "?"
#define TURAG_DEBUG_INFO_PREFIX     "-"
#define TURAG_DEBUG_DEBUG_PREFIX    "_"
#define TURAG_DEBUG_REPORT_LOG_SOURCE_PREFIX  ">"

// graph output prefixes
#define TURAG_DEBUG_GRAPH_PREFIX    "D"
#define TURAG_DEBUG_GRAPH_CREATE    "n"
#define TURAG_DEBUG_GRAPH_TITLE     "b"
#define TURAG_DEBUG_GRAPH_DATA      "d"

// debug level
#ifndef TURAG_DEBUG_LEVEL
#define TURAG_DEBUG_LEVEL 4
#endif

// output graphs
#if !defined(TURAG_DEBUG_ENABLE_GRAPH)
# if TURAG_DEBUG_LEVEL > 3
#  define TURAG_DEBUG_ENABLE_GRAPH
# endif
#endif

// log source
#ifndef TURAG_DEBUG_LOG_SOURCE
# define TURAG_DEBUG_LOG_SOURCE "_"
#endif

#if !defined(NDEBUG) && TURAG_DEBUG_LEVEL < 1
# define NDEBUG
#endif

#if TURAG_DEBUG_LEVEL > 0
# define system_printf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX ";;" format TURAG_DEBUG_NEWLINE, ##args)
# define system_print(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX ";;" msg TURAG_DEBUG_NEWLINE)
#else
# define system_printf(format, args...) while(0)
# define system_print(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 0
# define errorf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define error(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define errorf(format, args...) while(0)
# define error(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 1
# define criticalf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define critical(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define criticalf(format, args...) while(0)
# define critical(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 2
# define warningf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX format TURAG_DEBUG_NEWLINE,  ##args)
# define warning(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define warningf(format, args...) while(0)
# define warning(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 3
# define infof(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define info(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define infof(format, args...) while(0)
# define info(msg) while(0)
#endif

#if TURAG_DEBUG_LEVEL > 4
# define debugf(format, args...) debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define debug(msg) debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define debugf(format, args...) while(0)
# define debug(msg) while(0)
#endif

#define bool_string(b) ((b)?"true":"false")
#define bool_string_de(b) ((b)?"ja":"nein")


#ifdef TURAG_DEBUG_ENABLE_GRAPH
  unsigned turag_graph_create(const char* name);
# define turag_graph_channel_title(index, title) debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_TITLE TURAG_DEBUG_GRAPH_PREFIX "%d %s" TURAG_DEBUG_NEWLINE, index, title)
  void turag_graph_data(int index, float time, float* args, int count);
#else
# define turag_graph_create(name) while(0)
# define turag_graph_channel_title(index, title) while(0)
# define turag_graph_data(index, time, args, count) while(0)
#endif


#endif // TINA_DEBUG_H

