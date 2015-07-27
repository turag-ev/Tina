#ifndef TINA_ECOS_DEBUGPRINT_H
#define TINA_ECOS_DEBUGPRINT_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// repariert das von eCos zerstörte __PRETTY_FUNCTION__ Makro:
#undef __PRETTY_FUNCTION__

void turag_debug_puts(const char* s);

/// Zeichenkette formatiert ausgeben
#define turag_debug_vprintf vprintf

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_ECOS_DEBUGPRINT_H
