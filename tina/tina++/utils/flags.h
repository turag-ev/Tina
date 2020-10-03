#ifndef TINAPP_UTILS_FLAGS_H
#define TINAPP_UTILS_FLAGS_H

#include <limits>
#include "../tina.h"
#include "../helper/integer.h"

namespace TURAG {

/// \ingroup Misc
/// \brief Bitflags
template<typename Enum, std::size_t Bits = sizeof(Enum)*8>
class Flags {
public:
  typedef Enum enum_type;
  static constexpr std::size_t bits = Bits;

  /*
   * Note: There used to be 'static constexpr Flags NOTHING{}' which is not standard compliant.
   * Static class members must have a complete type if they are constexpr.
   * (GCC does not care, but clang does.)
   * 
   * Use the default constructor instead.
   */

  static constexpr TURAG_ALWAYS_INLINE int bit(Enum b) {
    return 1 << static_cast<int>(b);
  }

  constexpr TURAG_ALWAYS_INLINE Flags(const Flags& other) :
    i(other.i)
  { }

  constexpr TURAG_ALWAYS_INLINE Flags(Enum flag) :
    i(bit(flag))
  {}

  constexpr TURAG_ALWAYS_INLINE Flags() :
    i(0)
  {}

  explicit constexpr TURAG_ALWAYS_INLINE Flags(unsigned f) :
    i(f)
  {}

  TURAG_ALWAYS_INLINE Flags &operator=(const Flags &f) {
    i = f.i;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& set(Flags f, bool value = true) {
    if (value)
      i |= f;
    else
      clear(f);

    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& set(Enum f, bool value = true) {
    if (value)
      i |= bit(f);
    else
      clear(f);

    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& set() {
    i = std::numeric_limits<decltype(i)>::max();
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& clear() {
    i = 0;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& clear(Flags f) {
    i &= ~f.i;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& clear(Enum f) {
    i &= ~bit(f);
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& reset(Flags f) {
    i &= ~f.i;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& reset(Enum f) {
    i &= ~bit(f);
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& reset() {
    i = 0;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& toggle(Flags f) {
    i ^= f.i;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& toggle(Enum f) {
    i ^= bit(f);
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& flip(Flags f) {
    i ^= f.i;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& flip(Enum f) {
    i ^= bit(f);
    return *this;
  }

  constexpr
  TURAG_ALWAYS_INLINE bool test(Flags f) const {
    return (i & int(f)) == int(f);
  }

  constexpr
  TURAG_ALWAYS_INLINE bool test(Enum f) const {
    return i & bit(f);
  }

  /// \brief Ist mindestens ein Flag gesetzt?
  constexpr
  TURAG_ALWAYS_INLINE bool any() const {
    return i != 0;
  }

  /// \brief Ist kein Flag gesetzt?
  constexpr
  TURAG_ALWAYS_INLINE bool none() const {
    return i == 0;
  }

  /// \brief Anzahl der gesetzten Flags
  TURAG_ALWAYS_INLINE bool count() const {
    constexpr static unsigned max_bit = 1 << (sizeof(i)*8-1);
    int result = 0;
    for (unsigned b = max_bit; b != 0; b >>= 1) {
      if (i & b) result++;
    }
    return result;
  }

  TURAG_ALWAYS_INLINE Flags& operator&=(int mask) {
    i &= mask;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& operator&=(unsigned mask) {
    i &= mask;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& operator|=(Flags f) {
    i |= f.i;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& operator|=(Enum f) {
    i |= bit(f);
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& operator^=(Flags f) {
    i ^= f.i;
    return *this;
  }

  TURAG_ALWAYS_INLINE Flags& operator^=(Enum f) {
    i ^= bit(f);
    return *this;
  }

  constexpr  TURAG_ALWAYS_INLINE operator unsigned() const {
    return i;
  }

  constexpr TURAG_ALWAYS_INLINE Flags operator|(Flags f) const {
    return Flags(i | f.i);
  }

  constexpr TURAG_ALWAYS_INLINE Flags operator|(Enum f) const {
    return Flags(i | bit(f));
  }

  constexpr TURAG_ALWAYS_INLINE Flags operator^(Flags f) const {
    return Flags(i ^ f.i);
  }

  constexpr TURAG_ALWAYS_INLINE Flags operator^(Enum f) const {
    return Flags(i ^ bit(f));
  }

  constexpr TURAG_ALWAYS_INLINE Flags operator&(int mask) const {
    return Flags(i & mask);
  }

  constexpr TURAG_ALWAYS_INLINE Flags operator&(unsigned mask) const {
    return Flags(i & mask);
  }

  constexpr TURAG_ALWAYS_INLINE Flags operator&(Enum f) const {
    return Flags(i & bit(f));
  }

  constexpr TURAG_ALWAYS_INLINE Flags operator~() const {
    return Flags(~i);
  }

  constexpr TURAG_ALWAYS_INLINE bool operator!() const {
    return !i;
  }

private:
  typename unsigned_integer_least_bits<Bits>::type i;
};

#define TURAG_DEFINE_FLAGS(flags_, enum_) \
  typedef Flags<enum_> flags_;

#define TURAG_DEFINE_OPERATORS_FOR_FLAGS(flags) \
  constexpr TURAG_ALWAYS_INLINE Flags<flags::enum_type, flags::bits>\
  operator|(flags::enum_type f1, flags::enum_type f2) { \
    return Flags<flags::enum_type, flags::bits>(f1) | f2; \
  } \
  constexpr TURAG_ALWAYS_INLINE Flags<flags::enum_type, flags::bits>\
  operator|(flags::enum_type f1, Flags<flags::enum_type, flags::bits> f2) { \
    return f2 | f1; \
  }

} // namespace TURAG

#endif // TINAPP_UTILS_FLAGS_H
