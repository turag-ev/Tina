#ifndef TINA_HELPER_NORMALIZE_H
#define TINA_HELPER_NORMALIZE_H

// /////////////////////////////////////////////////////////////////////////////
// require at least C99 and C++11

#if defined(__STDC_VERSION__) && __STDC_VERSION__ < 199901L
#error TinA requires at least C99!
#endif

#if defined(__cplusplus) && __cplusplus < 201103L
#error TinA requires at least C++11!
#endif

/// \defgroup Compiler Compilerabstraktion [C/C++]
/// \{

// /////////////////////////////////////////////////////////////////////////////
// makros for version comperisons

#define TURAG_VERSION_EQUAL(lhs_major, lhs_minor, lhs_patch, rhs_major, rhs_minor, rhs_patch) \
    ((lhs_major) == (rhs_major) && (lhs_minor == rhs_minor) && (lhs_patch == rhs_patch))

#define TURAG_VERSION_LESS(lhs_major, lhs_minor, lhs_patch, rhs_major, rhs_minor, rhs_patch) \
    ((lhs_major) < (rhs_major)                                                               \
     || ((lhs_major) == (rhs_major) && (lhs_minor) < (rhs_minor))                            \
     || ((lhs_major) == (rhs_major) && (lhs_minor) == (rhs_minor) && (lhs_patch) < (rhs_patch)))

#define TURAG_VERSION_LESS_EQUAL(lhs_major, lhs_minor, lhs_patch, rhs_major, rhs_minor, rhs_patch) \
    (TURAG_VERSION_EQUAL(lhs_major, lhs_minor, lhs_patch, rhs_major, rhs_minor, rhs_patch)         \
     || TURAG_VERSION_LESS(lhs_major, lhs_minor, lhs_patch, rhs_major, rhs_minor, rhs_patch))

#define TURAG_VERSION_GREATER(lhs_major, lhs_minor, lhs_patch, rhs_major, rhs_minor, rhs_patch) \
    TURAG_VERSION_LESS(rhs_major, rhs_minor, rhs_patch, lhs_major, lhs_minor, lhs_patch)

#define TURAG_VERSION_GREATER_EQUAL(lhs_major, lhs_minor, lhs_patch, rhs_major, rhs_minor, rhs_patch) \
    TURAG_VERSION_LESS_EQUAL(rhs_major, rhs_minor, rhs_patch, lhs_major, lhs_minor, lhs_patch)

// /////////////////////////////////////////////////////////////////////////////
// makros for compiler and language versions

#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#define TURAG_HAS_AT_LEAST_GCC_VERSION(major, minor, patch) \
    TURAG_VERSION_LESS_EQUAL(major, minor, patch, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
#define TURAG_HAS_AT_LEAST_GCC_VERSION(major, minor, patch) (0)
#endif

#if defined(__clang__)
#define TURAG_HAS_AT_LEAST_CLANG_VERSION(major, minor, patch) \
    TURAG_VERSION_LESS_EQUAL(major, minor, patch, __clang_major__, __clang_minor__, __clang_patchlevel__)
#else
#define TURAG_HAS_AT_LEAST_CLANG_VERSION(major, minor, patch) (0)
#endif

#if defined(__STDC_VERSION__)
#define TURAG_HAS_C95 (__STDC_VERSION__ >= 199409L)
#define TURAG_HAS_C99 (__STDC_VERSION__ >= 199901L)
#define TURAG_HAS_C11 (__STDC_VERSION__ >= 201112L)
#define TURAG_HAS_C17 (__STDC_VERSION__ >= 201710L)
#else
#define TURAG_HAS_C95 (0)
#define TURAG_HAS_C99 (0)
#define TURAG_HAS_C11 (0)
#define TURAG_HAS_C17 (0)
#endif

#if defined(__cplusplus)
#define TURAG_HAS_CXX98 (__cplusplus >= 199711L)
#define TURAG_HAS_CXX11 (__cplusplus >= 201103L)
#define TURAG_HAS_CXX14 (__cplusplus >= 201402L)
#define TURAG_HAS_CXX17 (__cplusplus >= 201703L)
#define TURAG_HAS_CXX20 (__cplusplus >= 202002L)
#else
#define TURAG_HAS_CXX98 (0)
#define TURAG_HAS_CXX11 (0)
#define TURAG_HAS_CXX14 (0)
#define TURAG_HAS_CXX17 (0)
#define TURAG_HAS_CXX20 (0)
#endif

// /////////////////////////////////////////////////////////////////////////////
// makros for compiler pragmas

/// Declaring a compiler pragma
#define TURAG_PRAGMA(x) _Pragma(#x)

#if TURAG_HAS_AT_LEAST_GCC_VERSION(4, 6, 0) || defined(__clang__) || defined(__DOXYGEN__)

/// \brief Ignore a warning for the rest of the file
/// \sa TURAG_IGNORE_WARNING_BEGIN
#define TURAG_IGNORE_WARNING(x) TURAG_PRAGMA(GCC diagnostic ignored x)

/// \brief Ignore a warning until a ::TURAG_IGNORE_WARNING_END at the same hierarchy
/// \pre gcc > 4.6
/// \sa TURAG_IGNORE_WARNING_END
#define TURAG_IGNORE_WARNING_BEGIN(x)  \
    TURAG_PRAGMA(GCC diagnostic push); \
    TURAG_IGNORE_WARNING(x)

/// Stop ignoring the warning given in the ::TURAG_IGNORE_WARNING_BEGIN call at the same
/// hierarchy
/// \pre gcc > 4.6
/// \sa TURAG_IGNORE_WARNING_BEGIN
#define TURAG_IGNORE_WARNING_END() \
    TURAG_PRAGMA(GCC diagnostic pop)

#else
#define TURAG_IGNORE_WARNING_BEGIN(x) TURAG_IGNORE_WARNING(x)
#define TURAG_IGNORE_WARNING_END()
#endif

// /////////////////////////////////////////////////////////////////////////////
// inline

/// \brief function attribute to always inline a function
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#if defined(__cplusplus)
// C++ and GCC/clang version
#define TURAG_INLINE __attribute__((always_inline)) inline
#else
// C and GCC/clang version
#define TURAG_INLINE static __attribute__((always_inline)) inline
#endif
#else
#if defined(__cplusplus)
// Pure C++ version
#define TURAG_INLINE inline
#else
// Pure C version
#define TURAG_INLINE static inline
#endif
#endif

// /////////////////////////////////////////////////////////////////////////////
// constexpr

/// \brief function attribute to define a function a constexpr
#if TURAG_HAS_CXX11 || defined(__DOXYGEN__)
// C++11 version
#define TURAG_CONSTEXPR_FUNC constexpr
#elif defined(__GNUC__) || defined(__clang__)
// GCC/clang version
#define TURAG_CONSTEXPR_FUNC __attribute__((const))
#else
// General version
#define TURAG_CONSTEXPR_FUNC
#endif

// /////////////////////////////////////////////////////////////////////////////
// Builtin functions

#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
/// provide the compiler with branch prediction information
#define TURAG_LIKELY(x) __builtin_expect(!!(x), 1)
/// provide the compiler with branch prediction information
#define TURAG_UNLIKELY(x) __builtin_expect(!!(x), 0)
/// tell the compiler that a code path is never executed (e.g. after a switch)
#define TURAG_UNREACHABLE() __builtin_unreachable()
#else
#define TURAG_LIKELY(x) (x)
#define TURAG_UNLIKELY(x) (x)
#define TURAG_UNREACHABLE() while (1)
#endif

// /////////////////////////////////////////////////////////////////////////////
// extra gcc attribute shortcuts

/// Inline always (no compiler checking whether code will grow, might fail)
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define TURAG_ALWAYS_INLINE inline
#endif

/// mark a function, that only use it's arguments and/or read global memory
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_PURE_FUNC __attribute__((pure))
#else
#define TURAG_PURE_FUNC
#endif

/// mark a function, that only use it's arguments
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_CONST_FUNC __attribute__((const))
#else
#define TURAG_CONST_FUNC
#endif

/// mark a function, that is called very often
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_HOT_FUNC __attribute__((hot))
#else
#define TURAG_HOT_FUNC
#endif

/// mark a function, that is called very seldom
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_COLD_FUNC __attribute__((cold))
#else
#define TURAG_COLD_FUNC
#endif

/// mark a function, that does not return like \a exit and \a abort
#if TURAG_HAS_CXX11 || defined(__DOXYGEN__)
#define TURAG_NORETURN [[noreturn]]
#elif defined(__GNUC__) || defined(__clang__)
#define TURAG_NORETURN __attribute__((noreturn))
#else
#define TURAG_NORETURN
#endif

/// \brief optimize function with given level
/// e.x. \code TURAG_OPTIMIZE("O3") \endcode
/// \pre gcc >= 4.4
#if TURAG_HAS_AT_LEAST_GCC_VERSION(4, 4, 0) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_OPTIMIZE(o) __attribute__((optimize(o)))
#else
#define TURAG_OPTIMIZE(o)
#endif

/// priority for creation of a static instance of a class (higher is earlier)
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_INIT_PRIORITY(p) __attribute__((init_priority(p)))
#else
#define TURAG_INIT_PRIORITY(p)
#endif

/// mark a function as constructor, that is called before \a main.
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_CONSTRUCTOR __attribute__((constructor))
#else
#define TURAG_CONSTRUCTOR
#endif

/// mark a function as destructor, that is called after \a main.
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_DESTRUCTOR __attribute__((destructor))
#else
#define TURAG_DESTRUCTOR
#endif

/// mark a function as constructor with priority (like init_priority)
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_CONSTRUCTOR_WITH_PRIORITY(priority) __attribute__((constructor(priority)))
#else
#define TURAG_CONSTRUCTOR_WITH_PRIORITY(priority)
#endif

/// mark a function as destructor with priority (analogically with init_priority)
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_DESTRUCTOR_WITH_PRIORITY(priority) __attribute__((destructor(priority)))
#else
#define TURAG_DESTRUCTOR_WITH_PRIORITY(priority)
#endif

/// mark a function as an alias of an other function
/// e.x. \code void __f();
/// void f() TURAG_ALIAS("__f"); \endcode
/// In C++, the mangled name for the target must be used.
/// It is an error if ‘__f’ is not defined in the same translation unit.
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_ALIAS(name) __attribute__((weak, alias(name)))
#else
#define TURAG_ALIAS(name)
#endif

/// place variable in a specific section
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_SECTION(s) __attribute__((section("." s)))
#else
#define TURAG_SECTION(s)
#endif

/// \brief attribute for a struct to remove any padding in it.
/// \warning can influence performance and thread-safety
#if defined(__GNUC__) || defined(__clang__) || defined(__DOXYGEN__)
#define TURAG_PACKED __attribute__((packed))
#else
#define TURAG_PACKED
#endif

/// \brief GCC does not produce a warning for this variable.
/// This attribute, attached to a variable, means that the variable is meant to be possibly unused.
/// \bug Sorgt in MingW für eine Namenskollision.

// we include windows.h to circumvent a name clash
// in winioctl.h
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if TURAG_HAS_CXX17 || defined(__DOXYGEN__)
#define TURAG_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
#define TURAG_UNUSED __attribute__((unused))
#else
#define TURAG_UNUSED
#endif

#if TURAG_HAS_CXX14 || defined(__DOXYGEN__)
#define TURAG_DEPRECATED(msg) [[deprecated(msg)]]
#elif defined(__GNUC__) || defined(__clang__)
#define TURAG_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define TURAG_DEPRECATED(msg)
#endif

/// \}

#endif // TINA_HELPER_NORMALIZE_H
