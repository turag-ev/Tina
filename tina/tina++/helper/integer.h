#ifndef TINAPP_HELPER_INTEGER_H
#define TINAPP_HELPER_INTEGER_H

#include <tina/tina.h>
#include <cmath>

namespace TURAG {

// helper functions

constexpr bool power_of_two(unsigned n) {
    return (n & (n - 1)) == 0;
}

constexpr unsigned next_power_of_two(unsigned n) {
    return power_of_two(n) ? n : next_power_of_two(n + 1);
}

constexpr unsigned least_bytes_needed(unsigned n) {
    return (n + 7) / 8;
}

// unsigned_integer by needed bytes

template<std::size_t Bytes>
struct unsigned_integer_bytes { };

template<> struct unsigned_integer_bytes<0> { typedef  void type; };
template<> struct unsigned_integer_bytes<1> { typedef  uint8_t type; };
template<> struct unsigned_integer_bytes<2> { typedef uint16_t type; };
template<> struct unsigned_integer_bytes<4> { typedef uint32_t type; };
template<> struct unsigned_integer_bytes<8> { typedef uint64_t type; };

// unsigned_integer by needed bits

template<std::size_t Bits>
struct unsigned_integer_least_bits {
  typedef typename unsigned_integer_bytes<least_bytes_needed(Bits)>::type type;
};

} // namespace TURAG

#endif // TINAPP_HELPER_INTEGER_H
