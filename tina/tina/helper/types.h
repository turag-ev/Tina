#ifndef TINA_HELPER_TYPES_H
#define TINA_HELPER_TYPES_H

#define __STDC_FORMAT_MACROS

#ifdef __cplusplus
# include <cstddef>
# include <cstdint>
# include <cinttypes>
#else
# include <stddef.h>
# include <stdint.h>
# include <inttypes.h>
#
# ifndef ECOS
#  include <stdbool.h>
# endif
#endif

#ifdef ECOS
# include <cyg/infra/cyg_type.h>
#endif

#endif // TINA_HELPER_TYPES_H
