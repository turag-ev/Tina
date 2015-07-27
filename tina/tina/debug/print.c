#include "print.h"

#include <tina/time.h>
#include <stdarg.h>

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

TuragSystemTime turag_debug_print_gametime_gamestarttime = { 0 };

#if TURAG_PRINT_GAMETIME_AUTOMATIC

int turag_debug_print_gametime_auto_output_enabled = 0;

#endif

void turag_log_puts(const char* s)
{
#if TURAG_PRINT_GAMETIME_AUTOMATIC
	if (turag_debug_print_gametime_auto_output_enabled)
	{
		turag_print_system_gametime();
	}
#endif

	turag_debug_puts(s);
}

void turag_log_printf(const char* fmt, ...)
{
#if TURAG_PRINT_GAMETIME_AUTOMATIC
	if (turag_debug_print_gametime_auto_output_enabled)
	{
		turag_print_system_gametime();
	}
#endif

	va_list vargs;
	va_start(vargs, fmt);
	turag_debug_vprintf(fmt, vargs);
	va_end(vargs);
}

void turag_debug_printf(const char* fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt);
	turag_debug_vprintf(fmt, vargs);
	va_end(vargs);
}

#endif
