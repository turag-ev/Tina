#ifndef TINAPP_HELPER_PACKED_H
#define TINAPP_HELPER_PACKED_H

namespace TURAG {

/// \addtogroup Utils
/// \{

////////////////////////////////////////////////////////////////////////////////
// Pointer casts

/// Pack any type in any other type (size of other type must be sufficient)
template<typename Packed, typename Unpacked>
inline Packed pack(Unpacked src) {
  static_assert(sizeof(Unpacked) <= sizeof(Packed), "type cast fatal because of bit loose");

  union { Unpacked src; Packed dest; } helper;
  helper.src = src;
  return helper.dest;
}

/// Unpack any type to any other type.
/**
 * \param src packed type with value generated from \a packed function
 */
template<typename Unpacked, typename Packed>
inline Unpacked unpack(Packed src) {
  union { Unpacked dest; Packed src; } helper;
  helper.src = src;
  return helper.dest;
}

/// like reinterpret_cast, but returns reference and is more powerfull
template<typename Unpacked, typename Packed>
inline Unpacked& reinterpret_reference(Packed& src) {
  return *reinterpret_cast<Unpacked*>(&src);
}

template<typename Unpacked, typename Packed>
inline const Unpacked& reinterpret_reference(const Packed& src) {
  return *reinterpret_cast<const Unpacked*>(&src);
}

/// \}

} // namespace TURAG

#endif // TINAPP_HELPER_PACKED_H
