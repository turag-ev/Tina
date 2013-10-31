#ifndef TINAPP_NORMALIZE_H
#define TINAPP_NORMALIZE_H

#include <tina/helper/normalize.h>

////////////////////////////////////////////////////////////////////////////////
// override and final

#if GCC_VERSION < 40700
# define override
# define final
#endif

////////////////////////////////////////////////////////////////////////////////
// constexpr

#if GCC_VERSION < 40600
# define constexpr
#endif

// constexpr for functions from math.h?
#if GCC_VERSION < 40700
# define math_constexpr inline
#else // GCC_VERSION < 40700
# define math_constexpr constexpr
#endif // GCC_VERSION < 40700

////////////////////////////////////////////////////////////////////////////////
// nullptr

#if GCC_VERSION < 40600

namespace TURAG {

// from http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2431.pdf
const                        // this is a const object...
class nullptr_t {
public:
  template<class T>          // convertible to any type
  constexpr operator T*() const        // of null non-member
  { return 0; }              // pointer...

  template<class C, class T> // or any type of null
  constexpr operator T C::*() const    // member pointer...
  { return 0; }

private:
  void operator&() const;    // whose address can't be taken

} nullptr = {};              // and whose name is nullptr

} // namespace TURAG

#endif

#endif // NORMALIZE_H
