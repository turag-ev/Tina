#ifndef TINA_SIM_DEBUGPRINT_H
#define TINA_SIM_DEBUGPRINT_H

#ifdef __cplusplus
extern "C" {
#endif

void turag_debug_puts(const char* s);
void turag_debug_printf(const char* fmt, ...) __attribute__ ((format(printf, 1, 2)));

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_SIM_DEBUGPRINT_H
