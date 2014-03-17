#ifndef TINAPP_RANGE_ALGORITHM_H
#define TINAPP_RANGE_ALGORITHM_H

#include <algorithm>

#include "../tina.h"

namespace TURAG {

/// \addtogroup Ranges
/// \{

////////////////////////////////////////////////////////////////////////////////
// predicates for std algorithm e.x. std::find, ...

#ifndef DOXYGEN
template<typename T>
struct is_equal_predicate {
  typedef T type;

  template<typename U>
  constexpr explicit
  is_equal_predicate(U&& t) :
    value(std::move(t))
  { }

  constexpr
  bool operator() (const T& t) {
    return t == value;
  }

  T value;
};
#endif

template<typename T> constexpr
is_equal_predicate<typename std::remove_reference<T>::type> is_equal(T&& t) {
  return is_equal_predicate<typename std::remove_reference<T>::type>(std::forward<T>(t));
}

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

template <typename Range, typename T>
bool any_of_value(const Range& t, const T& val) {
  return std::find(std::begin(t), std::end(t), val) != std::end(t);
}

template <typename Range, typename Pred>
bool any_of(const Range& t, Pred pred) {
  return std::any_of(std::begin(t), std::end(t), pred);
}

template <typename Range, typename T>
bool none_of_value(const Range& t, const T& val) {
  return std::find(std::begin(t), std::end(t), val) == std::end(t);
}

template <typename Range, typename Pred>
bool none_of(const Range& t, Pred pred) {
  return std::none_of(std::begin(t), std::end(t), pred);
}


/// get length of container
template <typename Container>
std::size_t length(const Container& c) {
  return c.size();
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

/// \}

} // namespace TURAG

#endif // TINAPP_RANGE_ALGORITHM_H
