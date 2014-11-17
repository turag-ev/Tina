#ifndef STATIC_CONST_H
#define STATIC_CONST_H

namespace TURAG {
namespace _detail {

// To avoid ODR violations:
template<class T>
struct static_const
{
  static constexpr T value{};
};

template<class T>
constexpr T static_const<T>::value;

} // namespace _detail
} // namespace TURAG

#endif // STATIC_CONST_H
