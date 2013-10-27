/**
 * \file bits.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief Bit operations on integral types and enums
 *
 */

#ifndef UTILS_BITS_H
#define UTILS_BITS_H

#include "../types.h"

namespace TURAG {

template<std::size_t Bytes>
struct unsigned_integer { }

template<> struct unsigned_integer<1> { typedef  uint8_t type; }
template<> struct unsigned_integer<2> { typedef uint16_t type; }
template<> struct unsigned_integer<4> { typedef uint32_t type; }
template<> struct unsigned_integer<8> { typedef uint64_t type; }

///////////////////////////////////////////////////////////////////////////////
//   bit access and register manipulations

template<std::size_t Bytes>
class BitMask {
private:
  unsigned_integer<Bytes>::type mask_;
};

template<std::size_t Bytes>
class BitRange {
public:
  BitRange(std::size_t start_bit, std::size_t end_bit) :
    mask_(start_bit, end_bit), shift_(end_bit)
  { }

private:
  BitMask<Bytes> mask_;
  std::size_t shift_;
};

template<std::size_t Bytes>
class BitRegister {
  
  getBitRange(std::size_t start, std::size_t end) {
    return BitRangeProxy<Bytes>(this, start, end);
  }
  
  getBit
  BitRangeProxy<Bytes> getPin(std::size_t n) {
    return BitRangeProxy<Bytes>(this, n, n);
  }
  
private:
  violate Register* register_;
}

template<std::size_t Bytes>
class BitRangeProxy {
  BitProxy<Bytes>& operator= (unsigned_integer<Bytes>::type lhs) {
    mask_    
    return *this
  }

private:
  BitRange range_;
  BitRegister register_;
  
  constexpr BitProxy(BitMask<Bytes> range, BitRegister<Bytes> reg) :
    range_(range), register_(reg)
  { }
  
  friend class BitRegister;
}

///////////////////////////////////////////////////////////////////////////////
//   bit operations

/// create a bitmask
/// \param bits number of bits
/// \example create_bitmask(4) gives 00001111b = 0Fh
constexpr
inline unsigned create_bitmask(size_t bits) {
  return static_cast<unsigned>((1<<bits)-1);
}

// TODO:
// read_bitfield(T b, size_t pos, size_t n)
// write_bitfield(T b, U value, size_t pos, size_t n)

/// set bits in the bitmask in value to true
/// \param t any integer value
/// \param mask bitmask
/// \returns new value
/// example:
/// \code{.cpp}
/// // value is 11110000b;
/// set_bitmask(value, 11b);
/// // value is 11110011b;
/// \endcode
template<typename T>
inline T
set_bitmask(T& t, T mask) {
  return t = static_cast<T>(t | mask);
}

template<typename T>
constexpr inline T
add_bitmask(T t, T mask) {
  return static_cast<T>(t | mask);
}

/// Set bit in value to true
/// \param t any integer value
/// \param n bit number
/// \returns new value
/// example:
/// \code{.cpp}
/// // value is 11110000b;
/// set_bit(value, 2);
/// // value is 11110100b;
/// \endcode
template<typename T>
inline T
set_bit(T& t, size_t n) {
  return t = static_cast<T>(t | (1 << n));
}

template<typename T>
constexpr inline T
add_bit(T t, size_t n) {
  return static_cast<T>(t | (1 << n));
}

/// Clear bits in the bitmask in value
/// \param t any integer value
/// \param mask bitmask
/// \returns new value
/// example:
/// \code{.cpp}
/// // value is 00001111b;
/// clear_bitmask(value, 11b);
/// // value is 00001100b;
/// \endcode
template<typename T>
inline T
clear_bitmask(T& t, T mask) {
  return t = static_cast<T>(t & ~mask);
}

template<typename T>
constexpr inline T
remove_bitmask(T t, T mask) {
  return static_cast<T>(t & ~mask);
}

/// Clear bit in value
/// \param t any integer value
/// \param mask bitmask
/// \returns new value
/// example:
/// \code{.cpp}
/// // value is 00001111b;
/// clear_bit(value, 2);
/// // value is 00001011b;
/// \endcode
template<typename T>
inline T
clear_bit(T& t, size_t n) {
  return t = static_cast<T>(t &~(1 << n));
}

template<typename T>
constexpr inline T
remove_bit(T t, size_t n) {
  return static_cast<T>(t &~(1 << n));
}

/// Toggle bits in the bitmask in value
/// \param t any integer value
/// \param mask bitmask
/// \returns new value
/// example:
/// \code{.cpp}
/// // value is 00001110b;
/// toggle_bitmask(value, 11b);
/// // value is 00001101b;
/// \endcode
template<typename T>
inline T
toggle_bitmask(T& t, T mask) {
  return t = static_cast<T>(t ^ mask);
}

/// Toogle bit in value
/// \param t any integer value
/// \param mask bitmask
/// \returns new value
/// example:
/// \code{.cpp}
/// // value is 00001111b;
/// toggle_bit(value, 2);
/// // value is 00001011b;
/// \endcode
template<typename T>
inline T
toggle_bit(T& t, size_t n) {
  return t = static_cast<T>(t ^ (1 << n));
}

/// Test bits in the bitmask in value
/// \param t any integer value
/// \param mask bitmask
/// \returns true if all bits set in bitmask are also set in value, otherwise false
/// example:
/// \code{.cpp}
/// // value is 00001110b;
/// toggle_bitmask(value, 11b);
/// // returns false
/// \endcode
template<typename T>
constexpr inline bool
test_bitmask(T t, T mask) {
  return (t & mask) == mask;
}

/// Test bit in value
/// \param t any integer value
/// \param mask bitmask
/// \returns true if bit \a n is set in value, otherwise false
/// example:
/// \code{.cpp}
/// // value is 00001110b;
/// toggle_bitmask(value, 2);
/// // returns true
/// \endcode
template<typename T>
constexpr inline bool
test_bit(T t, size_t n) {
  return (t & (1 << n));
}

} // namespace TURAG

#endif // UTILS_BITS_H
