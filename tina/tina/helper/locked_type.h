/**
 * \file tina/helper/locked_type.h
 * \author Martin Oemus <martin@oemus.net>
 * \date April 2014
 *
 * \brief Makros zum erzeugen durch Mutexes geschützter Typen.
 *
 * Diese Makros können benutzt werden, um Typen zu erzeugen, deren Zugriff
 * automatisch durch einen Mutex geschützt wird, um so Thread-Sicherheit zu erreichen.
 *
 */


#ifndef __TINA_HELPERS_LOCKED_STRUCT__
#define __TINA_HELPERS_LOCKED_STRUCT__

#include <tina/tina.h>
#include <tina/thread.h>


#ifdef __cplusplus
extern "C" {
#endif





/*!
 * \brief Create a locked type containing data of the specified type.
 * \param[in] type Data type of the content of the locked variable.
 * 
 * This macro can either be used to directly define a variable for immediate use
 * as follows:
 * \code
 * static turag_atomic(my_struct_type) my_locked_variable;
 * \endcode
 * In this case we also used the static keyword which is not a 
 * requirement here, but it is almost always a good idea to use it.
 * 
 * If you instead want use the variable globally, you can use turag_locked
 * together with typedef and put it in a header:
 * \code 
 * typedef turag_locked(int) my_locked_type;
 * extern my_locked_type my_locked_variable;
 * \endcode
 * In your source file you would say:
 * \code
 * my_locked_type my_locked_variable;
 * \endcode
 * 
 */
#define turag_locked(type) 		\
		struct {				\
			TuragMutex mutex;	\
			type value;			\
		}

/*!
 * \brief Initialize a locked variable.
 * \param[in] name Name of the variable to initialize.
 * \param[in] init_value Initial value to assign to the locked variable.
 */
#define turag_locked_init(name, init_value) 	\
	TURAG_MACRO_BEGIN                          	\
		(name).value = init_value;				\
		turag_mutex_init(&(name).mutex);        \
	TURAG_MACRO_END

/*!
 * \brief Assign the value of a locked variable to a different variable.
 * \param[in] name Name of the locked variable to load from.
 * \param[out] destination Name of the the variable to store the data in. 
 * This macro works on plain variables as output argument rather than on pointers.
 */
#define turag_locked_load(name, destination)	\
	TURAG_MACRO_BEGIN                          	\
		turag_mutex_lock(&(name).mutex);		\
		destination = (name).value;				\
		turag_mutex_unlock(&(name).mutex);      \
	TURAG_MACRO_END

/*!
 * \brief Assign a new value to a locked variable.
 * \param[in] name Name of the locked variable to store data in.
 * \param[in] new_value New data.
 */
#define turag_locked_store(name, new_value)		\
	TURAG_MACRO_BEGIN                          	\
		turag_mutex_lock(&(name).mutex);		\
		(name).value = new_value;				\
		turag_mutex_unlock(&(name).mutex);     	\
	TURAG_MACRO_END


#ifdef __cplusplus
} // extern "C"
#endif

		
#endif // __TINA_HELPERS_LOCKED_STRUCT__
