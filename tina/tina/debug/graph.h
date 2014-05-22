#ifndef TINA_DEBUG_GRAPH_H
#define TINA_DEBUG_GRAPH_H

#include <tina/debugprint.h>
#include "defines.h"


#ifdef __cplusplus
extern "C" {
#endif


/// \addtogroup Debug
/// \{
/// \defgroup Debuggraph Messdatenausgabe
/// \{

// graph output prefixes
#define TURAG_DEBUG_GRAPH_PREFIX    		"D"
#define TURAG_DEBUG_GRAPH_CREATE    		"n"
#define TURAG_DEBUG_GRAPH_CHANNEL   		"b"
#define TURAG_DEBUG_GRAPH_CHANNEL_FIXED     "B"
#define TURAG_DEBUG_GRAPH_DATA      		"d"

#ifdef TURAG_DEBUG_ENABLE_GRAPH

///
/// \brief Creates a new diagram.
/// \param name Title of the diagram.
/// \return Index of the newly created diagram.
///
unsigned turag_graph_create(const char* name);

#ifdef DOXYGEN

/// \brief Add channels to a diagram and name them.
/// \param index Index of the diagram to add the channels to.
/// \param title Title of the channel.
void turag_graph_add_channel(unsigned index, const char* title);

#else // DOXYGEN

# define turag_graph_add_channel(index, title) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL TURAG_DEBUG_GRAPH_PREFIX "%d 0 %s" TURAG_DEBUG_NEWLINE, index, title)

#endif // DOXYGEN

#ifdef DOXYGEN

/// \brief Add channels to a diagram and name them.
/// \param index Index of the diagram to add the channels to.
/// \param title Title of the channel.
/// \param time Length of the visible part of the x-axis.
void turag_graph_add_channel_fixed_time(unsigned index, const char* title, unsigned time);

#else // DOXYGEN

# define turag_graph_add_channel_fixed_time(index, title, time) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL TURAG_DEBUG_GRAPH_PREFIX "%d %d %s" TURAG_DEBUG_NEWLINE, index, time, title)

#endif // DOXYGEN

#ifdef DOXYGEN

/// \brief Add channels with fixed axes to a diagram and name them.
/// \param index Index of the diagram to add the channels to.
/// \param title Title of the channel.
/// \param x_left Minimum value of the x-axis.
/// \param y_bottom Minimum value of the y-axis.
/// \param width With of the x-axis.
/// \param height Height of the y-axis.

void turag_graph_add_channel_fixed_xy(unsigned index, const char* title, int x_left, int y_bottom, unsigned width, unsigned height);

#else // DOXYGEN

# define turag_graph_add_channel_fixed_xy(index, title, x_left, y_bottom, width, height) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL_FIXED TURAG_DEBUG_GRAPH_PREFIX "%d %d %d %d %d %s" TURAG_DEBUG_NEWLINE, \
    		index, x_left, y_bottom, width, height, title)

#endif // DOXYGEN

/// \brief Add data to a diagram.
/// \param index Index of the diagram to add data to.
/// \param time Data for x-axis.
/// \param args Pointer to data for the y-axis starting with the first channel.
/// \param count Number of entries in args.
void turag_graph_data(unsigned index, float time, float* args, int count);

#else // TURAG_DEBUG_ENABLE_GRAPH

# define turag_graph_create(name) while(0)
# define turag_graph_channel_title(index, title) while(0)
# define turag_graph_data(index, time, args, count) while(0)

#endif // TURAG_DEBUG_ENABLE_GRAPH

/// \}
/// \}
/// \}

#ifdef __cplusplus
}           /* closing brace for extern "C" */
#endif


#endif // TINA_DEBUG_GRAPH_H
