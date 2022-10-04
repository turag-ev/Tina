#ifndef TINA_UTILS_BIT_MACROS_H
#define TINA_UTILS_BIT_MACROS_H

/// get bit
/// example: \code turag_bit(3) = 1 << 3 = 1000b = 0x8 \endcode
/// \param n number of bit
#define turag_bit(n)                        (1 << (n))

/// get bit mask
/// example: \code turag_bit_mask(3) = 0111b = 0x7 \endcode
/// \param n length of bit mask
#define turag_bit_mask(n)                   (turag_bit(n) - 1)

/// get bit mask of a range
/// example: \code turag_bit_range_mask(1, 3) = 1110b = 0xE \endcode
/// \param start first bit of bit range mask
/// \param end last bit of bit range mask  
#define turag_bit_range_mask(start, end)    (turag_bit_mask((end)-(start)+1) << start)

/// returns a with bits set in range
/// example: \code turag_bit_range(1, 3, 2) = 0100b = 0x4 \endcode
/// \param start first bit of bit range mask
/// \param end last bit of bit range mask  
#define turag_bit_range(start, end, val)    (((val) & turag_bit_mask((end)-(start))) << start)

/// set one bit to 1
/// example:
/// \code
///    x = 4;
///    turag_set_bit(x, 0);
///    // x = 0101b = 0x5
/// \endcode
/// \param value to modify
/// \param n number of bit to set
#define turag_set_bit(x, n)                 (x |= turag_bit(n))

/// set bits in bit range
/// example:
/// \code
///    x = 4;
///    turag_set_bit_range(x, 1, 3, 4);
///    // x = 1000b = 0x8
/// \endcode
/// \param value to modify
/// \param start first bit of bit range to modify
/// \param end last bit of bit range to modify
/// \param val value to write to bit range
#define turag_set_bit_range(x, start, end, val) \
  (x = (x & ~turag_bit_range_mask(start, end)) | ((val) << start))
  
/// set one bit to 0
/// example:
/// \code
///    x = 4;
///    turag_clear_bit(x, 2);
///    // x = 0000b = 0x0
/// \endcode
/// \param value to modify
/// \param n number of bit to clear
#define turag_clear_bit(x, n)               (x &= ~turag_bit(n))

#endif // TINA_UTILS_BIT_MACROS_H


