#ifndef TINAPP_HELPER_NORMALIZE_TYPE_TRAITS_H
#define TINAPP_HELPER_NORMALIZE_TYPE_TRAITS_H

#include <type_traits>

namespace std {

#ifndef __DOXYGEN__

#if GCC_VERSION < 40800 && !defined(__clang__)
template<typename T>
using is_trivially_destructible = has_trivial_destructor<T>;
#endif

#if GCC_VERSION < 50100 && !defined(__clang__)
template<typename T>
struct is_trivially_copyable : public
		integral_constant<bool,
			is_trivially_destructible<T>::value &&
			has_trivial_copy_assign<T>::value &&
			has_trivial_copy_constructor<T>::value
		>
{ };
#endif

#endif // __DOXYGEN__

} // namespace std

#endif // TINAPP_HELPER_NORMALIZE_TYPE_TRAITS_H
