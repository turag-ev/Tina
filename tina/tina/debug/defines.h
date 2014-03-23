/**
 * \file tina/debug.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief Utils for printing debug information
 *
 */

#ifndef TINA_DEBUG_DEFINES_H
#define TINA_DEBUG_DEFINES_H

#include <tina/config.h>

/// \defgroup Debug Debugging [C/C++]
/// \{

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

/// \}

#endif // TINA_DEBUG_DEFINES_H

