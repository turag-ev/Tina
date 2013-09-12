#ifndef ALGORITHM_H
#define ALGORITHM_H

//#include <utility>

#if GCC_VERSION >= 40400
#include <initializer_list>
#endif

#include <algorithm>

#include "normalize.h"

namespace extra {

////////////////////////////////////////////////////////////////////////////////
// std extention

template<typename Container, typename T>
Container& remove(Container& container, const T& val) {
  auto old_end = std::end(container);
  auto new_end = std::remove(std::begin(container), old_end, val);
  container.erase(new_end, old_end);
  return container;
}

template<typename Container, typename Predicate>
Container& remove_if(Container& container, Predicate pred) {
  auto old_end = std::end(container);
  auto new_end = std::remove_if(std::begin(container), old_end, pred);
  container.erase(new_end, old_end);
  return container;
}

////////////////////////////////////////////////////////////////////////////////
// useful extra funktions

template <typename Container, typename T>
bool any_of_value(const Container& t, const T& val) {
  return std::find(std::begin(t), std::end(t), val) != std::end(t);
}

/// get length of container
template <typename Container>
std::size_t length(const Container& c) {
  return c.length();
}

/// same as array_size
template <typename T, std::size_t N>
constexpr std::size_t length(const T (&)[N]) {
  return N;
}

////////////////////////////////////////////////////////////////////////////////
// iterator extentions

// result is undefined if passed container.rend()
template <class ReverseIterator>
typename ReverseIterator::iterator_type make_forward(ReverseIterator rit) {
  return rit.base() - 1;
}

} // namespace extra

#endif // ALGORITHM_H
