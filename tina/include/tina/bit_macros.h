
#ifndef BIT_MACROS_H
#define BIT_MACROS_H

/// get bit
/// example: \code BIT(3) = 1 << 3 = 1000b = 0x8 \endcode
/// \param n number of bit
#define BIT(n)                     (1 << (n))

/// get bit mask
/// example: \code BIT_MASK(3) = 0111b = 0x7 \endcode
/// \param n length of bit mask
#define BIT_MASK(n)                (BIT(n) - 1)  

/// get bit mask of a range
/// example: \code BIT_RANGE_MASK(1, 3) = 1110b = 0xE \endcode
/// \param start first bit of bit range mask
/// \param end last bit of bit range mask  
#define BIT_RANGE_MASK(start, end) (BIT_MASK((end)-(start)+1) << start)

/// returns a with bits set in range
/// example: \code BIT_RANGE(1, 3, 2) = 0100b = 0x4 \endcode
/// \param start first bit of bit range mask
/// \param end last bit of bit range mask  
#define BIT_RANGE(start, end, val) (((val) & BIT_MASK((end)-(start))) << start)

/// set one bit to 1
/// example:
/// \code
///    x = 4;
///    BIT_SET(x, 0);
///    // x = 0101b = 0x5
/// \endcode
/// \param value to modify
/// \param n number of bit to set
#define BIT_SET(x, n)              (x |= BIT(n))

/// set bits in bit range
/// example:
/// \code
///    x = 4;
///    BIT_SET_RANGE(x, 1, 3, 4);
///    // x = 1000b = 0x8
/// \endcode
/// \param value to modify
/// \param start first bit of bit range to modify
/// \param end last bit of bit range to modify
/// \param val value to write to bit range
#define BIT_SET_RANGE(x, start, end, val) \
  (x = (x & ~BIT_RANGE_MASK(start, end)) | ((val) << start))
  
/// set one bit to 0
/// example:
/// \code
///    x = 4;
///    BIT_CLEAR(x, 2);
///    // x = 0000b = 0x0
/// \endcode
/// \param value to modify
/// \param n number of bit to clear
#define BIT_CLEAR(x, n)              (x &= ~BIT(n))

#endif // BIT_MACROS_H


