#ifndef DEBUGPRINT_H_
#define DEBUGPRINT_H_

#ifdef __cplusplus
extern "C" {
#endif

void debug_puts(const char* s);
void debug_printf(const char* fmt, ...);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
