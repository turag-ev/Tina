#ifndef TINAPP_HELPER_NORMALIZE_H
#define TINAPP_HELPER_NORMALIZE_H

#include <tina/helper/normalize.h>

////////////////////////////////////////////////////////////////////////////////
// constexpr

// constexpr for functions from math.h?
#if !TURAG_HAS_AT_LEAST_GCC_VERSION(4, 7, 0) || defined(ECOS) || defined(__clang__)
# define TURAG_MATH_CONSTEXPR
#else
# define TURAG_MATH_CONSTEXPR constexpr
#endif

#endif // TINAPP_HELPER_NORMALIZE_H
