#ifndef TINA_ECOS_DEBUGPRINT_H
#define TINA_ECOS_DEBUGPRINT_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// repariert das von eCos zerstörte __PRETTY_FUNCTION__ Makro:
#undef __PRETTY_FUNCTION__

void debug_puts(const char* s);

/// Zeichenkette formatiert ausgeben
#define debug_printf printf

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_ECOS_DEBUGPRINT_H
