/**
 * \file types.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2011,2012
 *
 * \brief common used types
 *
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stddef.h>

#if !defined(ECOS) || defined(SYSTEM_CONTROL_ECOS)
	#include <stdint.h>

#else
# include <cyg/infra/cyg_type.h>

	typedef cyg_uint8  uint8_t;
	typedef cyg_int8   int8_t;

	typedef cyg_uint16 uint16_t;
	typedef cyg_int16  int16_t;

	typedef cyg_uint32 uint32_t;
	typedef cyg_int32  int32_t;

	typedef cyg_uint64 uint64_t;
	typedef cyg_int64  int64_t;
#endif

////////////////////////////////////////////////////////////////////////////////
// integral types

/// unsigned integer type
typedef unsigned int  uint;

/// unsigned char type
typedef unsigned char uchar;

/// void pointer type
typedef void* pointer;

#endif /* TYPES_H_ */
