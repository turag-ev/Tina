#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include <type_traits>
#include <iterator>

#include "normalize.h"
#include "macro-helper.h"

namespace extra {

template<typename T>
#if GCC_VERSION < 40800
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
void destruct(ForwardIterator first, ForwardIterator last) { }

template<typename ForwardIterator, REQUIRES(!is_trivially_destructible<typename std::iterator_traits<ForwardIterator>::value_type>)>
void destruct(ForwardIterator first, ForwardIterator last) {
  typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;

  for (; first != last; ++first) {
    destruct(std::addressof(*first));
  }
}

} // namespace extra

#endif // CONSTRUCT_H
