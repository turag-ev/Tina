#ifndef TINAPP_HELPER_MACROS_H
#define TINAPP_HELPER_MACROS_H

#include "normalize.h"
#include <tina/helper/macros.h>

////////////////////////////////////////////////////////////////////////////////
// C++ utils

/// Forbid copying
#define NOT_COPYABLE(ClassName) \
  ClassName(const ClassName&) = delete; \
  ClassName& operator=(const ClassName&) = delete

/// use default copying
#define COPYABLE(ClassName) \
  constexpr ClassName(const ClassName&) = default; \
  ClassName& operator=(const ClassName&) = default

/// Forbid copying
#define NOT_MOVABLE(ClassName) \
  ClassName(ClassName&&) = delete; \
  ClassName& operator=(ClassName&&) = delete

/// use default copying
#define MOVABLE(ClassName) \
  constexpr ClassName(ClassName&&) = default; \
  ClassName& operator=(ClassName&&) = default

///
#define REQUIRES(...) typename std::enable_if<__VA_ARGS__::value, bool>::type = false
#define REQUIRES2(...) typename std::enable_if<__VA_ARGS__, bool>::type = false

#endif // TINAPP_HELPER_MACROS_H
