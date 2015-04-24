
#include <tina++/utils/base64.h>

#include <cstdarg>
#include <cstdio>
#include <atomic>

#include "graph.h"


#ifdef TURAG_DEBUG_ENABLE_GRAPH

namespace {

	// We start our index at 1. This enables us to ignore any data sent with index 0
	// what could happen if the creation of the graph is accidentaly done after
	// sending the first data.
std::atomic<unsigned> turag_graph_index(1);

} // namespace

extern "C" unsigned turag_graph_create(const char* name) {
	unsigned myIndex = turag_graph_index.fetch_add(1);
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CREATE TURAG_DEBUG_GRAPH_PREFIX "%u %s" TURAG_DEBUG_NEWLINE, myIndex, name);
    return myIndex;
}

extern "C" void turag_graph_add_channel(unsigned index, const char* title) {
	if (index) {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL TURAG_DEBUG_GRAPH_PREFIX "%d 0 %s" TURAG_DEBUG_NEWLINE, 
						   index, title);
	}
}

extern "C" void turag_graph_add_channel_fixed_time(unsigned index, const char* title, unsigned time) {
	if (index) {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL TURAG_DEBUG_GRAPH_PREFIX "%d %d %s" TURAG_DEBUG_NEWLINE, 
						   index, time, title);
	}
}

extern "C" void turag_graph_add_channel_fixed_xy(unsigned index, const char* title, int x_left, int y_bottom, unsigned width, unsigned height) {
	if (index) {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL_FIXED TURAG_DEBUG_GRAPH_PREFIX "%d %d %d %d %d %s" TURAG_DEBUG_NEWLINE, 
						   index, x_left, y_bottom, width, height, title);
	}
}

extern "C" void turag_graph_data(unsigned index, float time, float* args, int count) {
	if (!index) {
		return;
	}
	
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_DATA TURAG_DEBUG_GRAPH_PREFIX "%u ", index);

    uint8_t encoded[7] = {0};

    turag_base64_encode((uint8_t*)&time, 4, encoded);
    turag_debug_puts((char*)encoded);


    int i;
    for ( i = 0; i < count; ++i ) {
        turag_base64_encode((uint8_t*)args, 4, encoded);
        turag_debug_puts((char*)encoded);
        ++args;
    }

    turag_debug_puts(TURAG_DEBUG_NEWLINE);
}

extern "C" void turag_graph_data2d(unsigned index, float* args, int count) {
	if (!index) {
		return;
	}
	
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_DATA2D TURAG_DEBUG_GRAPH_PREFIX "%u ", index);
	
    uint8_t encoded[7] = {0};

	int i;
    for ( i = 0; i < count * 2; ++i ) {
        turag_base64_encode((uint8_t*)args, 4, encoded);
        turag_debug_puts((char*)encoded);
        ++args;
    }

    turag_debug_puts(TURAG_DEBUG_NEWLINE);
}

extern "C" void turag_graph_add_vertical_marker(unsigned index, const char* label, float time) {
	if (!index) {
		return;
	}
	
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_VERTICAL_MARKER TURAG_DEBUG_GRAPH_PREFIX "%u ", index);

    uint8_t encoded[7] = {0};

    turag_base64_encode((uint8_t*)&time, 4, encoded);
    turag_debug_puts((char*)encoded);

	turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, label);	
}

extern "C" void turag_graph_add_channel_group(unsigned index, const char* label, unsigned count, ...) {
	if (!index) {
		return;
	}
	
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNELGROUP TURAG_DEBUG_GRAPH_PREFIX "%u %u", index, count);
	
	va_list indices;
	va_start ( indices, count );
	
	unsigned i;
	for (i = 0; i < count; ++i) {
		turag_debug_printf(" %d", va_arg ( indices, int ));
	}
	
	va_end ( indices );

	turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, label);	
}


#endif
