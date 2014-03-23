#ifndef TINA_DEBUG_GRAPH_H
#define TINA_DEBUG_GRAPH_H

#include <tina/debugprint.h>
#include "defines.h"

/// \addtogroup Debug
/// \{
/// \defgroup Debuggraph Messdatenausgabe
/// \{

// graph output prefixes
#define TURAG_DEBUG_GRAPH_PREFIX    "D"
#define TURAG_DEBUG_GRAPH_CREATE    "n"
#define TURAG_DEBUG_GRAPH_TITLE     "b"
#define TURAG_DEBUG_GRAPH_DATA      "d"

#ifdef TURAG_DEBUG_ENABLE_GRAPH

///
/// \brief turag_graph_create
/// \param name
///
unsigned turag_graph_create(const char* name);

#ifdef DOXYGEN

/// \brief turag_graph_channel_title
/// \param index
/// \param title
void turag_graph_channel_title(int index, const char* title);

#else // DOXYGEN

# define turag_graph_channel_title(index, title) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_TITLE TURAG_DEBUG_GRAPH_PREFIX "%d %s" TURAG_DEBUG_NEWLINE, index, title)

#endif // DOXYGEN

/// \brief turag_graph_data
/// \param index
/// \param time
/// \param args
/// \param count
void turag_graph_data(int index, float time, float* args, int count);

#else // TURAG_DEBUG_ENABLE_GRAPH

# define turag_graph_create(name) while(0)
# define turag_graph_channel_title(index, title) while(0)
# define turag_graph_data(index, time, args, count) while(0)

#endif // TURAG_DEBUG_ENABLE_GRAPH

/// \}
/// \}
/// \}

#endif // TINA_DEBUG_GRAPH_H
