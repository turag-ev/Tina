#ifndef TINAPP_INTEGER_H
#define TINAPP_INTEGER_H

#include <tina/tina.h>

namespace TURAG {

// unsigned_integer by needed bytes

template<std::size_t Bytes>
struct unsigned_integer_bytes { };

template<> struct unsigned_integer_bytes<1> { typedef  uint8_t type; };
template<> struct unsigned_integer_bytes<2> { typedef uint16_t type; };
template<> struct unsigned_integer_bytes<3> { typedef uint32_t type; };
template<> struct unsigned_integer_bytes<4> { typedef uint32_t type; };
template<> struct unsigned_integer_bytes<5> { typedef uint64_t type; };
template<> struct unsigned_integer_bytes<6> { typedef uint64_t type; };
template<> struct unsigned_integer_bytes<7> { typedef uint64_t type; };
template<> struct unsigned_integer_bytes<8> { typedef uint64_t type; };

// unsigned_integer by needed bits

template<std::size_t Bits>
struct unsigned_integer {
  typedef typename unsigned_integer_bytes<(Bits+7)/8>::type type;
};

} // namespace TURAG

#endif // TINA_INTEGER_H
