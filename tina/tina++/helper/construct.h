#ifndef TINAPP_HELPER_CONSTRUCT_H
#define TINAPP_HELPER_CONSTRUCT_H

#include <type_traits>
#include <iterator>

#include "normalize.h"
#include "macros.h"

namespace TURAG {

template<typename T>
#if GCC_VERSION < 40800 && !defined(__clang__)
struct is_trivially_destructible : public std::has_trivial_destructor<T> { };
#else
struct is_trivially_destructible : public std::is_trivially_destructible<T> { };
#endif


/// construct instance of type in memory pointed by ptr with arguments
template<typename T, typename ...Args> _always_inline
void construct(T* ptr, Args&&... args) {
  ::new(ptr) T(std::forward<Args>(args)...);
}

/// destroy instance by pointer
template<typename T, REQUIRES(is_trivially_destructible<T>)> _always_inline
void destruct(T*) { }

template<typename T, REQUIRES(!is_trivially_destructible<T>)> _always_inline
void destruct(T* ptr) {
  ptr->~T();
}

/// destroy a range of type instances
template<typename ForwardIterator, REQUIRES(is_trivially_destructible<typename std::iterator_traits<ForwardIterator>::value_type>)> _always_inline
void destruct(ForwardIterator, ForwardIterator) { }

template<typename ForwardIterator, REQUIRES(!is_trivially_destructible<typename std::iterator_traits<ForwardIterator>::value_type>)>
void destruct(ForwardIterator first, ForwardIterator last) {
  for (; first != last; ++first) {
    destruct(std::addressof(*first));
  }
}

} // namespace TURAG

#endif // TINAPP_HELPER_CONSTRUCT_H
