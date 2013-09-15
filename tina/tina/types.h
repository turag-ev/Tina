/**
 * \file types.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2011,2012
 *
 * \brief common used types
 *
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <limits>
#include <stddef.h>
#include <util/types.h>
#include "normalize.h"

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// integral types

/// unsigned integer type
typedef unsigned int  uint;

/// unsigned char type
typedef unsigned char uchar;

/// void pointer type
typedef void* pointer;

////////////////////////////////////////////////////////////////////////////////
// intern types

/// type for event ids
typedef uint32_t EventId;

/// function for event processing
typedef void (*EventMethod)(EventId id, pointer data);

/// create a unsigned integer with the first three bytes filled with characters
/// to build a namespace. The last byte can be used for any value >= 0 and < 256.
/// example:
/// \code enum { event_x = EventNamespace('X', 'X', 'X') + 100 }; \endcode
constexpr
int32_t EventNamespace(char a, char b, char c) {
  return (a << 24) | (b << 16) | (c << 8);
}
constexpr
int32_t SCnamespace(char a, char b, char c) {
  return EventNamespace(a, b, c);
}

/// silent not-a-number of float type
static const float NaNf = __builtin_nanf("");

typedef uint Points;

} // namespace TURAG

#endif /* TYPES_H_ */
