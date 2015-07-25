#include "print.h"

#include <tina/time.h>

#if TURAG_DEBUG_LEVEL > 0

const char* turag_log_source_table[127] = {0};

void turag_debug_print_logsources(void) {
	int i;
	for (i = 33; i < 127; i++) {
		if (turag_log_source_table[i] != 0) {
			turag_debug_printf(TURAG_DEBUG_LINE_PREFIX "%c" TURAG_DEBUG_REPORT_LOG_SOURCE_PREFIX "%s" TURAG_DEBUG_NEWLINE, i, turag_log_source_table[i]);
		}
	}
}

#endif
