#ifndef TINA_DEBUG_GRAPH_H
#define TINA_DEBUG_GRAPH_H

#include <tina/debugprint.h>
#include "defines.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \addtogroup Debug
 * \{
 * \defgroup Debuggraph Messdatenausgabe
 * \brief Ermöglicht die grafische 2D-Darstellung von Daten.
 * 
 * Bla Bla.
 * 
 * \{
 */

#ifndef __DOXYGEN__
// graph output prefixes
#define TURAG_DEBUG_GRAPH_PREFIX    		"D"
#define TURAG_DEBUG_GRAPH_CREATE    		"n"
#define TURAG_DEBUG_GRAPH_CHANNEL   		"b"
#define TURAG_DEBUG_GRAPH_CHANNEL_FIXED     "B"
#define TURAG_DEBUG_GRAPH_DATA      		"d"
#define TURAG_DEBUG_GRAPH_DATA2D      		"D"
#define TURAG_DEBUG_GRAPH_VERTICAL_MARKER  	"m"
#define TURAG_DEBUG_GRAPH_CHANNELGROUP  	"g"
#endif // __DEOXYGEN__


#ifdef TURAG_DEBUG_ENABLE_GRAPH

///
/// \brief Legt ein neues Diagramm an.
/// \param name Titel des Diagramms.
/// \return Index of the newly created diagram.
unsigned turag_graph_create(const char* name);


/// \brief Add channels to a diagram and name them.
/// \param index Index of the diagram to add the channels to.
/// \param title Title of the channel.
void turag_graph_add_channel(unsigned index, const char* title);


/// \brief Add channels to a diagram and name them.
/// \param index Index of the diagram to add the channels to.
/// \param title Title of the channel.
/// \param time Length of the visible part of the x-axis.
void turag_graph_add_channel_fixed_time(unsigned index, const char* title, unsigned time);


/// \brief Add channels with fixed axes to a diagram and name them.
/// \param index Index of the diagram to add the channels to.
/// \param title Title of the channel.
/// \param x_left Minimum value of the x-axis.
/// \param y_bottom Minimum value of the y-axis.
/// \param width With of the x-axis.
/// \param height Height of the y-axis.
void turag_graph_add_channel_fixed_xy(unsigned index, const char* title, int x_left, int y_bottom, unsigned width, unsigned height);


/// \brief Add data to a diagram.
/// \param index Index of the diagram to add data to.
/// \param time Data for x-axis.
/// \param args Pointer to data for the y-axis starting with the first channel.
/// \param count Number of entries in args.
void turag_graph_data(unsigned index, float time, float* args, int count);

/**
 * \brief 2D-Daten zu einem Diagramm hinzufügen
 * \param index Index des Diagramms.
 * \param args Zeiger zu den Daten-Paaren.
 * \param count Anzahl der Datenpaare.
 * 
 * \note Das args-Array muss doppelt so viele float-Elemente enthalten
 * wie in count angegeben, da count die Anzahl der Datenpaare angibt.
 * In args soll stets erst x und dann y stehen.
 */
void turag_graph_data2d(unsigned index, float* args, int count);


/// \brief Add a vertical marker to a diagram.
/// \param index Index of the diagram to add the channels to.
/// \param label Label.
/// \param time Time.
void turag_graph_add_vertical_marker(unsigned index, const char* label, float time);

/// \brief Defines a channel grouping.
/// \param index Index of the diagram to define the channel group for.
/// \param label Label.
/// \param count Number of channels in the group.
/// \param ... Channel indices to add to the group.
void turag_graph_add_channel_group(unsigned index, const char* label, unsigned count, ...);



#else // TURAG_DEBUG_ENABLE_GRAPH

# define turag_graph_create(name) 0
# define turag_graph_add_channel(index, title) while(0)
# define turag_graph_add_channel_fixed_xy(index, title, x_left, y_bottom, width, height) while(0)
# define turag_graph_add_channel_fixed_time(index, title, time) while(0)
# define turag_graph_data(index, time, args, count) while(0)
# define turag_graph_add_vertical_marker(index, label, time) while(0)
# define turag_graph_add_channel_group(index, label, count, ...) while(0)

#endif // TURAG_DEBUG_ENABLE_GRAPH

/// \}
/// \}
/// \}

#ifdef __cplusplus
}           /* closing brace for extern "C" */
#endif


#endif // TINA_DEBUG_GRAPH_H
