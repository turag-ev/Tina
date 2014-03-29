#ifndef TINA_HELPER_MACROS_H
#define TINA_HELPER_MACROS_H

#include "normalize.h"

////////////////////////////////////////////////////////////////////////////////
// Preprocessor utils

/// \defgroup Macros Makrohelfer [C/C++]
/// \{

#ifndef DOXYGEN

// Interna
#define TURAG_CONCAT_(x,y) x##y
#define TURAG_CONCAT3_(a,b,c) a##b##c
#define TURAG_CONCAT4_(a,b,c,d) a##b##c##d

#endif // DOXYGEN

/// Concat two symbols
/// \code CONCAT(A, BC) will give ABC \endcode
#define TURAG_CONCAT(x,y) TURAG_CONCAT_(x,y)

/// Concat three symbols
/// \code CONCAT(A, B, C) will give ABC \endcode
#define TURAG_CONCAT3(a,b,c) TURAG_CONCAT3_(a,b,c)

/// Concat three symbols
/// \code CONCAT(A, B, C, D) will give ABCD \endcode
#define TURAG_CONCAT4(a,b,c,d) TURAG_CONCAT4_(a,b,c,d)


#ifndef DOXYGEN

#define __NARG__(...) \
         __NARG_I_(__VA_ARGS__,__RSEQ_N())
#define __NARG_I_(...) \
         __ARG_N(__VA_ARGS__)
#define __ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define __RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0
         
#else // DOXYGEN

///  \brief Get number of arguments in a macro function
///
/// from here: https://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5?pli=1
#define __NARG__(...)

#endif // DOXYGEN

////////////////////////////////////////////////////////////////////////////////
// Compiler utils

/// use it to shut the compiler up talking about unused variables
#ifndef UNUSED
#  define UNUSED(x) (void)x
#endif

////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION and FINALIZATION

/// \brief macro for initialization code, called before \a main()
///
/// usage: \code INITIALIZATION {
///   function(parameter);
/// } \endcode
#define INITIALIZATION \
  static _constructor void TURAG_CONCAT(_initialization_code_, __COUNTER__)()

/// macro for finalization code, called after \a main returns
///
/// usage: \code FINALIZATION {
///   function(parameter);
/// } \endcode
#define FINALIZATION \
  static _destructor void TURAG_CONCAT(_finalization_code, __COUNTER__)()
  
/// \}

#endif // TINA_HELPER_MACROS_H
