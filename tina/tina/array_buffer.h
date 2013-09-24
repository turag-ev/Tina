#ifndef TINA_ARRAY_H
#define TINA_ARRAY_H

#include <cstddef>
#include <iterator>
#include <cassert>

#include "normalize.h"
#include "debug.h"
#include "array_storage.h"

namespace TURAG {

template<typename ArrayBufferType>
struct _ArrayBufferHelper {
  typedef typename ArrayBufferType::iterator iterator;
  typedef typename ArrayBufferType::const_iterator const_iterator;

#ifdef NDEBUG
  static bool prepare_for_insert(iterator position, iterator end);

#else
  static bool prepare_for_insert(iterator position, iterator end, const_iterator max_end);
  static bool is_full(const_iterator end, const_iterator max_end);
#endif

};

template<typename T, size_t N>
struct ArrayBuffer {
  static_assert(N > 0, "ArrayBuffer size must be greater than null.");

public:
  typedef ArrayBuffer<T, N> self_type;

  // types
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  // iterator
  typedef pointer iterator;
  typedef const_pointer const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  // ctor
  constexpr explicit ArrayBuffer() :
    length_(0), bytes_()
  { }

  COPYABLE(ArrayBuffer);
  MOVABLE(ArrayBuffer);

  // iterators
  iterator begin() {
    return iterator(&bytes_[0]);
  }

  constexpr
  const_iterator begin() const {
    return const_iterator(&bytes_[0]);
  }

  constexpr
  const_iterator cbegin() const {
    return const_iterator(&bytes_[0]);
  }

  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  constexpr
  const_reverse_iterator rbegin() const {
    return reverse_iterator(end());
  }

  constexpr
  const_reverse_iterator crbegin() const {
    return reverse_iterator(end());
  }

  iterator end() {
    return iterator(&bytes_[size()]);
  }

  constexpr
  const_iterator end() const {
    return const_iterator(&bytes_[size()]);
  }

  constexpr
  const_iterator cend() const {
    return const_iterator(&bytes_[size()]);
  }

  reverse_iterator rend() {
    return reverse_iterator(begin());
  }

  constexpr
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  constexpr
  const_reverse_iterator crend() const {
    return const_reverse_iterator(begin());
  }

  // capacity
  constexpr
  bool empty() const {
    return length_ == 0;
  }

  constexpr
  size_type size() const {
    return length_;
  }

  constexpr
  size_type length() const {
    return length_;
  }

  constexpr
  size_type capacity() const {
    return N;
  }

  constexpr
  size_type max_size() const {
    return capacity();
  }

  // element access
  reference front() {
    return *begin();
  }

  reference back() {
    return *(end()-1);
  }

  constexpr
  const_reference front() const {
    return *begin();
  }

  constexpr
  const_reference back() const {
    return *(end()-1);
  }

  reference operator[](size_type n) {
    return bytes_[n];
  }

  constexpr
  const_reference operator[](size_type n) const {
    return bytes_[n];
  }

  // modifiers
  void pop_front() {
    erase(begin());
  }

  void pop_back() {
    if (!empty()) {
      length_--;
      bytes_.erase(length_);
    }
  }

  void push_back(const value_type& val) {
    if (!is_full()) {
      bytes_.emplace(length_, val);
      length_++;
    }
  }

  template<class... Args> _always_inline
  void emplace_back(Args&&... args) {
    if (!is_full()) {
      bytes_.emplace(length_, std::forward<Args>(args)...);
      length_++;
    }
  }

  void push_front(const value_type& val) {
    insert(begin(), val);
  }

  template<class... Args> _always_inline
  void emplace_front(Args&&... args) {
    if (prepare_for_insert(begin())) {
      length_++;
      bytes_.emplace(0, std::forward<Args>(args)...);
    }
  }

  void clear() {
    length_ = 0;
  }

  void insert(iterator position, const value_type& val) {
    value_type copy = val;
    if (prepare_for_insert(position)) {
      length_++;
      bytes_.emplace(position, std::move(copy));
    }
  }

  template<class... Args> _always_inline
  void emplace(iterator position, Args&&... args) {
    if (prepare_for_insert(position)) {
      length_++;
      bytes_.emplace(position, std::forward<Args>(args)...);
    }
  }

  void erase(iterator position);
  void erase(iterator first, iterator last);

private:
  // number of elements in array
  std::size_t length_;

  // bytes for array
  array_storage<T, N> bytes_;

  bool prepare_for_insert(iterator position) {
#ifdef NDEBUG
    return _ArrayBufferHelper<self_type>::prepare_for_insert(position, end());
#else
    return _ArrayBufferHelper<self_type>::prepare_for_insert(position, end(), const_iterator(&bytes_[N]));
#endif
  }

  bool is_full() {
#ifdef NDEBUG
    return false;
#else
    return _ArrayBufferHelper<self_type>::is_full(cend(), const_iterator(&bytes_[N]));
#endif
  }

  friend struct _ArrayBufferHelper<self_type>;
};

template<typename T, std::size_t N>
void ArrayBuffer<T, N>::erase(iterator position) {
  if (position != end()) {
    length_--;
    bytes_.erase(position - &bytes_[0]);
    for (iterator i = position; i < end(); ++i) {
      *i = *(i+1);
    }
  }
}

template<typename T, std::size_t N>
void ArrayBuffer<T, N>::erase(iterator first, iterator last) {
  assert(last >= first);

  size_t diff = last - first;
  // TODO bytes_.erase for diff elements
  length_ -= diff;
  if (!empty()) {
    for (iterator i = first; i < end(); ++i) {
      *i = *(i + diff);
    }
  } else {
    length_ = 0;
  }
}

#ifdef NDEBUG
template<typename ArrayBufferType>
bool _ArrayBufferHelper<ArrayBufferType>::prepare_for_insert(iterator position, iterator end) {
  for (iterator i = end; i > position; --i) {
    *i = *(i-1);
  }
  return true;
}
#else
template<typename ArrayBufferType>
bool _ArrayBufferHelper<ArrayBufferType>::prepare_for_insert(iterator position, iterator end, const_iterator max_end) {
  if (end == max_end) {
    error("ArrayBuffer overflow!");
    return false;
  }

  for (iterator i = end; i > position; --i) {
    *i = *(i-1);
  }
  return true;
}
#endif

#ifndef NDEBUG
template<typename ArrayBufferType>
bool _ArrayBufferHelper<ArrayBufferType>::is_full(const_iterator end, const_iterator max_end) {
  if (end == max_end) {
    error("ArrayBuffer overflow!");
    return true;
  }
  return false;
}
#endif

} // namespace TURAG

#endif // TINA_ARRAY_H
