#ifndef TMP_LENGTH_H
#define TMP_LENGTH_H

#include <type_traits>

namespace TURAG {
namespace TMP {

// Typ ohne statisch erkennbare Länge
template<typename T>
struct length { };

template<typename T, std::size_t N>
struct length<T[N]> : public std::integral_constant<std::size_t, N> { };

} // namespace TMP
} // namespace TURAG

#endif // TMP_LENGTH_H
