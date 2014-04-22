/**
 * \file tina/helper/atomic.h
 * \author Martin Oemus <martin@oemus.net>
 * \date April 2014
 *
 * \brief Atomic types for C
 *
 * Atomic types for the use in C.
 *
 * These types use (with the exception of \ref turag_sig_atomic_t) mutexes
 * to make the accesses thread-safe. You should keep in mind the overhead
 * that comes with that.
 *
 */


#ifndef __TINA_HELPERS_ATOMIC__
#define __TINA_HELPERS_ATOMIC__

#include <tina/tina.h>
#include <tina/thread.h>

#include <signal.h>


#ifdef __cplusplus
extern "C" {
#endif



/*! 
 * \brief Lock-free integer type
 *
 * This type is not protected with a mutex and usually equals int.
 *
 * \warning Only use this type if you know what you are doing!
 * Read-modify-write operations like the ++-operator can still
 * cause unexpected behaviour.
 */
#ifdef ECOS
typedef cyg_atomic turag_sig_atomic_t;
#else
typedef sig_atomic_t turag_sig_atomic_t;
#endif


/*!
 * \brief Create an atomic type containing data of the specified type.
 * \param[in] type Data type of the content of te atomic variable.
 * 
 * This macro can either be used to directly define a variable for immediate use
 * as follows:
 * \code
 * static turag_atomic(int) my_atomic_int_variable;
 * \endcode
 * In this case we also used the static keyword which is not a 
 * requirement here, but it is almost always a good idea to use it.
 * 
 * If you instead want use the variable globally, you can use turag_atomic
 * together with typedef and put it in a header:
 * \code 
 * typedef turag_atomic(int) my_atomic_int_type;
 * extern my_atomic_int_type my_atomic_int_variable;
 * \endcode
 * In your source file you would say:
 * \code
 * my_atomic_int_type my_atomic_int_variable;
 * \endcode
 * 
 */
#define turag_atomic(type) 		\
		struct {				\
			TuragMutex mutex;	\
			type value;			\
		}

/*!
 * \brief Initialize an atomic variable.
 * \param[in] name Name of the variable to initialize.
 * \param[in] init_value Initial value to assign to the atomic variable.
 */
#define turag_atomic_init(name, init_value) 	\
		(name).value = init_value;				\
		turag_mutex_init(&(name).mutex)
		
/*!
 * \brief Assign the value of an atomic variable to a different variable.
 * \param[in] name Name of the atomic variable to load from.
 * \param[out] destination Name of the the variable to store the data in. 
 * This macro works on plain variables as the output argument rather than on pointers.
 */
#define turag_atomic_load(name, destination)	\
		turag_mutex_lock(&(name).mutex);		\
		destination = (name).value;				\
		turag_mutex_lock(&(name).mutex)
		
/*!
 * \brief Assign a new value to an atomic variable.
 * \param[in] name Name of the atomic variable to store data in.
 * \param[in] new_value New data.
 */
#define turag_atomic_store(name, new_value)		\
		turag_mutex_lock(&(name).mutex);		\
		(name).value = new_value;				\
		turag_mutex_lock(&(name).mutex)

#ifndef DOXYGEN
typedef turag_atomic(bool) turag_atomic_bool_t;
typedef turag_atomic(float) turag_atomic_float_t;
typedef turag_atomic(double) turag_atomic_double_t;

typedef turag_atomic(int64_t) turag_atomic_int64_t;
typedef turag_atomic(uint64_t) turag_atomic_uint64_t;

typedef turag_atomic(int32_t) turag_atomic_int32_t;
typedef turag_atomic(uint32_t) turag_atomic_uint32_t;

typedef turag_atomic(int16_t) turag_atomic_int16_t;
typedef turag_atomic(uint16_t) turag_atomic_uint16_t;

typedef turag_atomic(int8_t) turag_atomic_int8_t;
typedef turag_atomic(uint8_t) turag_atomic_uint8_t;
#else
/// \brief Atomic type containg a bool.
typedef turag_atomic_bool turag_atomic_bool_t;

/// \brief Atomic type containg a float.
typedef turag_atomic_float turag_atomic_float_t;

/// \brief Atomic type containg a double.
typedef turag_atomic_double turag_atomic_double_t;

/// \brief Atomic type containg an int64_t.
typedef turag_atomic_int64 turag_atomic_int64_t;
/// \brief Atomic type containg an uint64_t.
typedef turag_atomic_uint64 turag_atomic_uint64_t;

/// \brief Atomic type containg an int32_t.
typedef turag_atomic_int32 turag_atomic_int32_t;
/// \brief Atomic type containg an uint32_t.
typedef turag_atomic_uint32 turag_atomic_uint32_t;

/// \brief Atomic type containg an int16_t.
typedef turag_atomic_int16 turag_atomic_int16_t;
/// \brief Atomic type containg an uint16_t.
typedef turag_atomic_uint16 turag_atomic_uint16_t;

/// \brief Atomic type containg an int8_t.
typedef turag_atomic_int8 turag_atomic_int8_t;
/// \brief Atomic type containg an uint8_t.
typedef turag_atomic_uint8 turag_atomic_uint8_t;
#endif

#ifdef __cplusplus
} // extern "C"
#endif

		
#endif // __TINA_HELPERS_ATOMIC__
