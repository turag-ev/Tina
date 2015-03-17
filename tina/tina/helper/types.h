/**
 * \file tina/helper/types.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2011,2012
 *
 * \brief common used types
 *
 */

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
#endif

#ifndef ECOS
# include <stdbool.h>
#else
# include <cyg/infra/cyg_type.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// unsigned integer type
typedef unsigned int  uint;

/// unsigned char type
typedef unsigned char uchar;

/// void pointer type
typedef void* pointer;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_HELPER_TYPES_H
