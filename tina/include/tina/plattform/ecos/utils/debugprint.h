#ifndef DEBUGPRINT_H_
#define DEBUGPRINT_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// eCos corrupted __PRETTY_FUNCTION__ makro, fixing that:
#undef __PRETTY_FUNCTION__

void debug_puts(const char* s);

/// Print string formated (same as stdio's \a printf).
#define debug_printf printf

#ifdef __cplusplus
} // extern "C"
#endif

#endif
