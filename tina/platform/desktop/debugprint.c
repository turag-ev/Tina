#include <tina/debugprint.h>
#include <stdio.h>
#include <stdarg.h>


void turag_debug_puts(const char* s) {
    printf("%s", s);
}

void turag_debug_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}