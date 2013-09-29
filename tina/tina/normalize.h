#ifndef NORMALIZE_H
#define NORMALIZE_H

/// get GCC version in useable format
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

////////////////////////////////////////////////////////////////////////////////
// override and final

#if GCC_VERSION < 40700 && defined(__cplusplus)
# define override
# define final
#endif

////////////////////////////////////////////////////////////////////////////////
// constexpr

#if GCC_VERSION < 40600 && defined(__cplusplus)
# define constexpr __attribute__((const))
#endif

////////////////////////////////////////////////////////////////////////////////
// nullptr

#if (GCC_VERSION < 40600) && defined(__cplusplus)

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

////////////////////////////////////////////////////////////////////////////////
// makros for compiler pragmas

/// Declaring a macro
#define DO_PRAGMA(x) _Pragma (#x)

/// Ignore a warning for the rest of the file
#define IGNORE_WARNING(x) DO_PRAGMA(GCC diagnostic ignored x)

/* Test for GCC >= 4.6.0 */
#if GCC_VERSION >= 40600
/// Ignore a warning until a IGNORE_WARNING_END at the same hierarchy
#define IGNORE_WARNING_BEGIN(x) \
  DO_PRAGMA(GCC diagnostic push); \
  IGNORE_WARNING(x)
/// Stop ignoring the warning given in the IGNORE_WARNING_BEGIN call at the same
/// hierarchy
#define IGNORE_WARNING_END() \
  DO_PRAGMA(GCC diagnostic pop)
#else
#define IGNORE_WARNING_BEGIN(x) IGNORE_WARNING(x)
#define IGNORE_WARNING_END()
#endif

////////////////////////////////////////////////////////////////////////////////
// extra gcc attribute shortcuts

/// Inline always (no compiler checking whether code will grow, might fail)
#define _always_inline __attribute__((always_inline)) inline

/// mark a function, that only use it's arguments and/or read global memory
#define _pure __attribute__((pure))

/// mark a function, that only use it's arguments
#define _const __attribute__((const))

/// mark a function, that very often
#define _hot __attribute__((hot))

/// mark a function, that is called very seldom
#define _cold __attribute__((cold))

/// mark a function, that dont return like \a exit and \a abort
#define _noreturn __attribute__((noreturn))

/// optimize function with given level, e.x. \code _optimize("O3") \endcode
#if GCC_VERSION >= 40400
# define _optimize(o) __attribute__((optimize(o)))
#else
# define _optimize(o)
#endif

/// priority for creation of a static instance of a class (higher is earlier)
#define _init_priority(p) __attribute__((init_priority(p)))

/// mark a function as constructor, that is called before \a main.
#define _constructor __attribute__((constructor))

/// mark a function as destructor, that is called after \a main.
#define _destructor __attribute__((destructor))

/// mark a function as constructor with priority (like init_priority)
#define _constructor_with_priority(priority) __attribute__((_constructor(priority)))

/// mark a function as destructor with priority (analogically with init_priority)
#define _destructor_with_priority(priority) __attribute__((_destructor(priority)))

/// mark a function as an alias of an other function
/// e.x. \code void __f();
/// void f() _alias("__f"); \endcode
/// In C++, the mangled name for the target must be used.
/// It is an error if ‘__f’ is not defined in the same translation unit.
#define _alias(name) __attribute__((weak, alias (name)))

/// place variable in a specific  section
#define _section(s) __attribute__ ((section("." s)))

/// ccm memory for stm
#ifdef CHIBIOS
# define _ccm_memory _section("ccm")
#else
# define _ccm_memory
#endif

#endif // NORMALIZE_H
