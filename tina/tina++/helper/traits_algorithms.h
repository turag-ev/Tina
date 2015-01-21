#ifndef TINAPP_HELPER_TRAITS_ALGORITHMS_H_
#define TINAPP_HELPER_TRAITS_ALGORITHMS_H_

#include <type_traits>

namespace TURAG {

//
// all_true

namespace detail {

template <typename... T>
struct all_true_impl : std::true_type {};

template <typename Head, typename... Tail>
struct all_true_impl<Head, Tail...> : std::conditional<Head::value, all_true_impl<Tail...>, std::false_type>::type {};

}

template <typename Head, typename... Tail>
struct all_true : detail::all_true_impl<Head, Tail...> {};

//
// all_false

namespace detail {

template <typename... T>
struct all_false_impl : std::true_type {};

template <typename Head, typename... Tail>
struct all_false_impl<Head, Tail...> : std::conditional<Head::value, std::false_type, all_false_impl<Tail...> >::type {};

}

template <typename Head, typename... Tail>
struct all_false : detail::all_false_impl<Head, Tail...> {};

//
// max_value

template<typename T>
constexpr
T max_value(T&&t)
{
  return std::forward<T>(t);
}

template<typename T0, typename T1, typename... Ts>
constexpr
typename std::common_type<
  T0, T1, Ts...
>::type max_value(T0&& val1, T1&& val2, Ts&&... vs)
{
  return (val2 < val1) ?
    max_value(val1, std::forward<Ts>(vs)...) :
    max_value(val2, std::forward<Ts>(vs)...);
}

template<typename T>
using get_type = typename T::type;

template<typename T>
using get_value_type = typename T::value_type;

template<typename Head, typename... Tail>
struct max_integral_constant : std::integral_constant<
	typename Head::value_type,
	max_value(Head::value, Tail::value...)>
{ };

//
// min_value

template<typename T>
constexpr
T min_value(T&&t)
{
  return std::forward<T>(t);
}

template<typename T0, typename T1, typename... Ts>
constexpr
typename std::common_type<
  T0, T1, Ts...
>::type min_value(T0&& val1, T1&& val2, Ts&&... vs)
{
  return (val2 < val1) ?
    min_value(val2, std::forward<Ts>(vs)...) :
    min_value(val1, std::forward<Ts>(vs)...);
}

template<typename Head, typename... Tail>
struct min_integral_constant : std::integral_constant<
	typename Head::value_type,
	min_value(Head::value, Tail::value...)>
{ };

} // namespace TURAG

#endif /* TINAPP_HELPER_TRAITS_ALGORITHMS_H_ */
