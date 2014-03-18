
#include "debug.h"
#include <stdarg.h>
#include <tina/utils/base64.h>

#ifdef TURAG_DEBUG_ENABLE_GRAPH

unsigned turag_graph_index = 0;


unsigned turag_graph_create(const char* name) {
    debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CREATE TURAG_DEBUG_GRAPH_PREFIX "%d %s" TURAG_DEBUG_NEWLINE, turag_graph_index, name);
    return turag_graph_index++;
}


void turag_graph_data(int index, float time, float* args, int count) {
    debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_DATA TURAG_DEBUG_GRAPH_PREFIX "%d ", index);

    uint8_t encoded[7] = {0};

    turag_base64_encode((uint8_t*)&time, 4, encoded);
    debug_puts((char*)encoded);


    int i;
    for ( i = 0; i < count; ++i ) {
        turag_base64_encode((uint8_t*)args, 4, encoded);
        debug_puts((char*)encoded);
        ++args;
    }

    debug_puts(TURAG_DEBUG_NEWLINE);
}


#endif
