
#include <tina/utils/base64.h>
#include <stdarg.h>
#include <stdio.h>

#include "graph.h"


#ifdef TURAG_DEBUG_ENABLE_GRAPH


static unsigned turag_graph_index = 0;


unsigned turag_graph_create(const char* name) {
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CREATE TURAG_DEBUG_GRAPH_PREFIX "%u %s" TURAG_DEBUG_NEWLINE, turag_graph_index, name);
    return turag_graph_index++;
}


void turag_graph_data(unsigned index, float time, float* args, int count) {
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

void turag_graph_add_vertical_marker(unsigned index, const char* label, float time) {
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_VERTICAL_MARKER TURAG_DEBUG_GRAPH_PREFIX "%u ", index);

    uint8_t encoded[7] = {0};

    turag_base64_encode((uint8_t*)&time, 4, encoded);
    turag_debug_puts((char*)encoded);

	turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, label);	
}

void turag_graph_add_channel_group(unsigned index, const char* label, unsigned count, ...) {
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
