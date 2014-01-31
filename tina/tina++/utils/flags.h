#ifndef TINAPP_UTILS_FLAGS_H
#define TINAPP_UTILS_FLAGS_H

#include <limits>
#include "../tina.h"
#include "../helper/integer.h"

namespace TURAG {

template<typename Enum, std::size_t Bits>
class Flags {
public:
  typedef Enum enum_type;
  static constexpr std::size_t bits = Bits;

  static const Flags NOTHING;

  static constexpr _always_inline int bit(Enum b) {
    return 1 << static_cast<int>(b);
  }

  constexpr _always_inline Flags(const Flags& other) :
    i(other.i)
  { }

  constexpr _always_inline Flags(Enum flag) :
    i(bit(flag))
  {}

  constexpr _always_inline Flags() :
    i(0)
  {}

  explicit constexpr _always_inline Flags(int f) :
    i(f)
  {}

  explicit constexpr _always_inline Flags(unsigned f) :
    i(f)
  {}

  _always_inline Flags &operator=(const Flags &f) {
    i = f.i;
    return *this;
  }

  _always_inline Flags& set(Flags f, bool value = true) {
    if (value)
      i |= f;
    else
      clear(f);

    return *this;
  }

  _always_inline Flags& set(Enum f, bool value = true) {
    if (value)
      i |= bit(f);
    else
      clear(f);

    return *this;
  }

  _always_inline Flags& set() {
    i = std::numeric_limits<decltype(i)>::max();
    return *this;
  }

  _always_inline Flags& clear() {
    i = 0;
    return *this;
  }

  _always_inline Flags& clear(Flags f) {
    i &= ~f.i;
    return *this;
  }

  _always_inline Flags& clear(Enum f) {
    i &= ~bit(f);
    return *this;
  }

  _always_inline Flags& reset(Flags f) {
    i &= ~f.i;
    return *this;
  }

  _always_inline Flags& reset(Enum f) {
    i &= ~bit(f);
    return *this;
  }

  _always_inline Flags& reset() {
    i = 0;
    return *this;
  }

  _always_inline Flags& toggle(Flags f) {
    i ^= f.i;
    return *this;
  }

  _always_inline Flags& toggle(Enum f) {
    i ^= bit(f);
    return *this;
  }

  _always_inline Flags& flip(Flags f) {
    i ^= f.i;
    return *this;
  }

  _always_inline Flags& flip(Enum f) {
    i ^= bit(f);
    return *this;
  }

  constexpr
  _always_inline bool test(Flags f) const {
    return (i & int(f)) == int(f);
  }

  constexpr
  _always_inline bool test(Enum f) const {
    return i & bit(f);
  }

  constexpr
  _always_inline bool any() const {
    return i != 0;
  }

  constexpr
  _always_inline bool none() const {
    return i == 0;
  }

  _always_inline bool count() const {
    constexpr static unsigned max_bit = 1 << (sizeof(i)*8-1);
    int result = 0;
    for (unsigned b = max_bit; b != 0; b >>= 1) {
      if (i & b) result++;
    }
    return result;
  }

  _always_inline Flags& operator&=(int mask) {
    i &= mask;
    return *this;
  }

  _always_inline Flags& operator&=(unsigned mask) {
    i &= mask;
    return *this;
  }

  _always_inline Flags& operator|=(Flags f) {
    i |= f.i;
    return *this;
  }

  _always_inline Flags& operator|=(Enum f) {
    i |= bit(f);
    return *this;
  }

  _always_inline Flags& operator^=(Flags f) {
    i ^= f.i;
    return *this;
  }

  _always_inline Flags& operator^=(Enum f) {
    i ^= bit(f);
    return *this;
  }

  constexpr  _always_inline operator unsigned() const {
    return i;
  }

  constexpr _always_inline Flags operator|(Flags f) const {
    return Flags(Enum(i | f.i));
  }

  constexpr _always_inline Flags operator|(Enum f) const {
    return Flags(Enum(i | bit(f)));
  }

  constexpr _always_inline Flags operator^(Flags f) const {
    return Flags(Enum(i ^ f.i));
  }

  constexpr _always_inline Flags operator^(Enum f) const {
    return Flags(Enum(i ^ bit(f)));
  }

  constexpr _always_inline Flags operator&(int mask) const {
    return Flags(Enum(i & mask));
  }

  constexpr _always_inline Flags operator&(unsigned mask) const {
    return Flags(Enum(i & mask));
  }

  constexpr _always_inline Flags operator&(Enum f) const {
    return Flags(Enum(i & bit(f)));
  }

  constexpr _always_inline Flags operator~() const {
    return Flags(Enum(~i));
  }

  constexpr _always_inline bool operator!() const {
    return !i;
  }

private:
  typename unsigned_integer_least_bits<Bits>::type i;
};

#define DEFINE_FLAGS(flags_, enum_) \
  typedef Flags<enum_> flags_;

#define DEFINE_OPERATORS_FOR_FLAGS(flags) \
  constexpr _always_inline Flags<flags::enum_type, flags::bits>\
  operator|(flags::enum_type f1, flags::enum_type f2) { \
    return Flags<flags::enum_type, flags::bits>(f1) | f2; \
  } \
  constexpr _always_inline Flags<flags::enum_type, flags::bits>\
  operator|(flags::enum_type f1, Flags<flags::enum_type, flags::bits> f2) { \
    return f2 | f1; \
  }

template<typename Enum, std::size_t Bits>
const Flags<Enum, Bits> Flags<Enum, Bits>::NOTHING;

} // namespace TURAG

#endif // TINAPP_UTILS_FLAGS_H
