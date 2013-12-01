#ifndef CYGONCE_LIBM_MATHINCL_FDLIBM_H
#define CYGONCE_LIBM_MATHINCL_FDLIBM_H
//===========================================================================
//
//      fdlibm.h
//
//      Internal definitions for math library implementation based on fdlibm
//
//===========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-02-09
// Purpose:     
// Description:   Internal implementation-specific header for math library
//                based on fdlibm
// Usage:         From within this package, #include "mathincl/fdlibm.h"
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libm.h>   // Configuration header

// Include the Math library?
#ifdef CYGPKG_LIBM     

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <math.h>                  // Main header for math library
#include <float.h>                 // Properties of FP representation on this
                                   // platform
#include <sys/ieeefp.h>            // Cyg_libm_ieee_double_shape_type

// SANITY CHECKS

// Just check that we support IEEE-style 64-bit doubles. If not, this
// math library will not work
// This check will go away when support for single-precision alternatives are
// provided

#if DBL_MAX_EXP != 1024
# error IEEE-style 64-bit doubles are required to use the math library
#endif // if DBL_MAX_EXP == 1024


// TYPES

typedef cyg_int32   __int32_t;
typedef cyg_uint32  __uint32_t;
typedef Cyg_libm_ieee_double_shape_type ieee_double_shape_type;
typedef Cyg_libm_ieee_float_shape_type ieee_float_shape_type;

/* Most routines need to check whether a float is finite, infinite, or not a
   number, and many need to know whether the result of an operation will
   overflow.  These conditions depend on whether the largest exponent is
   used for NaNs & infinities, or whether it's used for finite numbers.  The
   macros below wrap up that kind of information:

   FLT_UWORD_IS_FINITE(X)
	True if a positive float with bitmask X is finite.

   FLT_UWORD_IS_NAN(X)
	True if a positive float with bitmask X is not a number.

   FLT_UWORD_IS_INFINITE(X)
	True if a positive float with bitmask X is +infinity.

   FLT_UWORD_MAX
	The bitmask of FLT_MAX.

   FLT_UWORD_HALF_MAX
	The bitmask of FLT_MAX/2.

   FLT_UWORD_EXP_MAX
	The bitmask of the largest finite exponent (129 if the largest
	exponent is used for finite numbers, 128 otherwise).

   FLT_UWORD_LOG_MAX
	The bitmask of log(FLT_MAX), rounded down.  This value is the largest
	input that can be passed to exp() without producing overflow.

   FLT_UWORD_LOG_2MAX
	The bitmask of log(2*FLT_MAX), rounded down.  This value is the
	largest input than can be passed to cosh() without producing
	overflow.

   FLT_LARGEST_EXP
	The largest biased exponent that can be used for finite numbers
	(255 if the largest exponent is used for finite numbers, 254
	otherwise) */

#ifdef _FLT_LARGEST_EXPONENT_IS_NORMAL
#define FLT_UWORD_IS_FINITE(x) 1
#define FLT_UWORD_IS_NAN(x) 0
#define FLT_UWORD_IS_INFINITE(x) 0
#define FLT_UWORD_MAX 0x7fffffff
#define FLT_UWORD_EXP_MAX 0x43010000
#define FLT_UWORD_LOG_MAX 0x42b2d4fc
#define FLT_UWORD_LOG_2MAX 0x42b437e0
//#define HUGE ((float)0X1.FFFFFEP128)
#else
#define FLT_UWORD_IS_FINITE(x) ((x)<0x7f800000L)
#define FLT_UWORD_IS_NAN(x) ((x)>0x7f800000L)
#define FLT_UWORD_IS_INFINITE(x) ((x)==0x7f800000L)
#define FLT_UWORD_MAX 0x7f7fffffL
#define FLT_UWORD_EXP_MAX 0x43000000
#define FLT_UWORD_LOG_MAX 0x42b17217
#define FLT_UWORD_LOG_2MAX 0x42b2d4fc
//#define HUGE ((float)3.40282346638528860e+38)
#endif
#define FLT_UWORD_HALF_MAX (FLT_UWORD_MAX-(1L<<23))
#define FLT_LARGEST_EXP (FLT_UWORD_MAX>>23)

/* Many routines check for zero and subnormal numbers.  Such things depend
   on whether the target supports denormals or not:

   FLT_UWORD_IS_ZERO(X)
	True if a positive float with bitmask X is +0.	Without denormals,
	any float with a zero exponent is a +0 representation.	With
	denormals, the only +0 representation is a 0 bitmask.

   FLT_UWORD_IS_SUBNORMAL(X)
	True if a non-zero positive float with bitmask X is subnormal.
	(Routines should check for zeros first.)

   FLT_UWORD_MIN
	The bitmask of the smallest float above +0.  Call this number
	REAL_FLT_MIN...

   FLT_UWORD_EXP_MIN
	The bitmask of the float representation of REAL_FLT_MIN's exponent.

   FLT_UWORD_LOG_MIN
	The bitmask of |log(REAL_FLT_MIN)|, rounding down.

   FLT_SMALLEST_EXP
	REAL_FLT_MIN's exponent - EXP_BIAS (1 if denormals are not supported,
	-22 if they are).
*/

#ifdef _FLT_NO_DENORMALS
#define FLT_UWORD_IS_ZERO(x) ((x)<0x00800000L)
#define FLT_UWORD_IS_SUBNORMAL(x) 0
#define FLT_UWORD_MIN 0x00800000
#define FLT_UWORD_EXP_MIN 0x42fc0000
#define FLT_UWORD_LOG_MIN 0x42aeac50
#define FLT_SMALLEST_EXP 1
#else
#define FLT_UWORD_IS_ZERO(x) ((x)==0)
#define FLT_UWORD_IS_SUBNORMAL(x) ((x)<0x00800000L)
#define FLT_UWORD_MIN 0x00000001
#define FLT_UWORD_EXP_MIN 0x43160000
#define FLT_UWORD_LOG_MIN 0x42cff1b5
#define FLT_SMALLEST_EXP -22
#endif

// MACRO DEFINITIONS

#ifndef __STDC__
# define __STDC__ 1
#endif
#define CYG_LIBM_HI(__x)  (((Cyg_libm_ieee_double_shape_type *)&__x)->parts.msw)
#define CYG_LIBM_LO(__x)  (((Cyg_libm_ieee_double_shape_type *)&__x)->parts.lsw)
#define CYG_LIBM_HIp(__x) (((Cyg_libm_ieee_double_shape_type *)__x)->parts.msw)
#define CYG_LIBM_LOp(__x) (((Cyg_libm_ieee_double_shape_type *)__x)->parts.lsw)

#define CYG_LIBM_WORD(__x)  (((Cyg_libm_ieee_float_shape_type *)&__x)->asi32)

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,f)					\
do {								\
  Cyg_libm_ieee_float_shape_type gf_u;					\
  gf_u.value = (f);						\
  (i) = gf_u.asi32;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(f,i)					\
do {								\
  Cyg_libm_ieee_float_shape_type sf_u;					\
  sf_u.asi32 = (i);						\
  (f) = sf_u.value;						\
} while (0)

/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)                                \
do {                                                            \
  Cyg_libm_ieee_double_shape_type ew_u;                         \
  ew_u.value = (d);                                             \
  (ix0) = ew_u.parts.msw;                                       \
  (ix1) = ew_u.parts.lsw;                                       \
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)                                      \
do {                                                            \
  Cyg_libm_ieee_double_shape_type gh_u;                         \
  gh_u.value = (d);                                             \
  (i) = gh_u.parts.msw;                                         \
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)                                       \
do {                                                            \
  Cyg_libm_ieee_double_shape_type gl_u;                         \
  gl_u.value = (d);                                             \
  (i) = gl_u.parts.lsw;                                         \
} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)                                 \
do {                                                            \
  Cyg_libm_ieee_double_shape_type iw_u;                         \
  iw_u.parts.msw = (ix0);                                       \
  iw_u.parts.lsw = (ix1);                                       \
  (d) = iw_u.value;                                             \
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)                                      \
do {                                                            \
  Cyg_libm_ieee_double_shape_type sh_u;                         \
  sh_u.value = (d);                                             \
  sh_u.parts.msw = (v);                                         \
  (d) = sh_u.value;                                             \
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)                                       \
do {                                                            \
  Cyg_libm_ieee_double_shape_type sl_u;                         \
  sl_u.value = (d);                                             \
  sl_u.parts.lsw = (v);                                         \
  (d) = sl_u.value;                                             \
} while (0)


// REPLACEMENTS FOR STUFF FROM MATH.H DUE TO CONFIG OPTION

#ifdef CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION

#define HUGE            FLT_MAX    // from float.h
#define DOMAIN          1
#define SING            2
#define OVERFLOW        3
#define UNDERFLOW       4
#define TLOSS           5
#define PLOSS           6

#endif // ifdef CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION

// FUNCTION PROTOTYPES

__int32_t rem_pio2f(float x, float *y);

/* float versions of fdlibm kernel functions */
externC float __kernel_standard_float( float, float, int );
externC float __kernel_sinf (float,float,int);
externC float __kernel_cosf (float,float);
externC float __kernel_tanf (float,float,int);
externC int   __kernel_rem_pio2f (float*,float*,int,int,int,const cyg_int32*);

#endif // ifdef CYGPKG_LIBM     

#endif // CYGONCE_LIBM_MATHINCL_FDLIBM_H multiple inclusion protection

// EOF fdlibm.h
