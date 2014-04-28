#ifndef TINA_CHIBIOS_DEBUGPRINT_H
#define TINA_CHIBIOS_DEBUGPRINT_H

#include <debug/cooldebug.h>

#ifdef __STDC_FORMAT_MACROS

#include <inttypes.h>

// fix this for chibios:

# if __WORDSIZE == 64
#  define __PRI64_PREFIX	"l"
#  define __PRIPTR_PREFIX	"l"
# else
#  define __PRI64_PREFIX	"ll"
#  define __PRIPTR_PREFIX
# endif

#undef PRIdPTR
#undef PRIiPTR
#undef PRIoPTR
#undef PRIuPTR
#undef PRIxPTR
#undef PRIXPTR

# define PRIdPTR	__PRIPTR_PREFIX "d"
# define PRIiPTR	__PRIPTR_PREFIX "i"
# define PRIoPTR	__PRIPTR_PREFIX "o"
# define PRIuPTR	__PRIPTR_PREFIX "u"
# define PRIxPTR	__PRIPTR_PREFIX "x"
# define PRIXPTR	__PRIPTR_PREFIX "X"

#endif // __STDC_FORMAT_MACROS

#ifdef __cplusplus
extern "C" {
#endif

#define turag_debug_puts      TINA_DEBUG
#define turag_debug_printf    TINA_DEBUG

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_CHIBIOS_DEBUGPRINT_H
