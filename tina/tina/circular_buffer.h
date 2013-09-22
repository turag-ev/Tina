#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstddef>
#include <iterator>

#include "normalize.h"
#include "debug.h"
#include "macros.h"
#include "array_storage.h"

namespace TURAG {

/// circular buffer implementation
///   -empty-     -2 elements-   -3 elements-
///  +-+-+-+-+     +-+-+-+-+      +-+-+-+-+
///  | | | | |     | |1|2| |      |3| |1|2|
///  +-+-+-+-+     +-+-+-+-+      +-+-+-+-+
///   ^             ^   ^          ^ ^
///  first & last   |   last       | first
///                first          last
template<typename T, typename T_nonconst, typename E = typename T::value_type>
class CircularBufferIterator : public std::iterator<std::random_access_iterator_tag, typename T::value_type> {
public:
  typedef CircularBufferIterator self_type;

  // ctor
  constexpr
  CircularBufferIterator(T* queue, size_t pos) :
    queue_(*queue), pos_(pos)
  { }

  // convert from iterator to const_iterator
  constexpr
  CircularBufferIterator(
    const CircularBufferIterator<
      T_nonconst, T_nonconst,
      typename T_nonconst::value_type
    > &other) :
    queue_(other.queue_), pos_(other.pos_)
  { }

  friend class CircularBufferIterator<
    const T, T, const E
  >;

  _always_inline
  E& operator*() {
    return queue_[pos_];
  }

  CircularBufferIterator& operator+=(std::size_t n) {
    pos_ += n;
    return *this;
  }

  self_type& operator-=(std::size_t n) {
    pos_ -= n;
    return *this;
  }

  constexpr
  self_type operator+(std::size_t n) const {
    return self_type(queue_, pos_ + n);
  }

  constexpr
  self_type operator-(std::size_t n) const {
    return self_type(queue_, pos_ - n);
  }

  // function below are using only function above
  _always_inline
  E* operator->()  {
    return &(operator*());
  }

  self_type& operator++() {
    return (*this += 1);
  }

  const self_type operator++(int) {
    const self_type old = *this;
    ++(*this);
    return old;
  }

  self_type& operator--() {
    return (*this -= 1);
  }

  const self_type operator--(int) {
    const self_type old = *this;
    --(*this);
    return old;
  }

  constexpr
  bool operator==(const self_type& other) const {
    return pos_ == other.pos_;
  }

  constexpr
  bool operator!=(const self_type& other) const {
    return pos_ != other.pos_;
  }

private:
  T& queue_;
  std::size_t pos_;
};


template<typename T, std::size_t N>
class CircularBuffer {
  static_assert(!(N & (N-1)), "CircularBuffer size must be a power of 2");

public:
  COPYABLE(CircularBuffer);
  MOVABLE(CircularBuffer);

  // types
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  // iterator
  typedef CircularBufferIterator<CircularBuffer,CircularBuffer,T> iterator;
  typedef CircularBufferIterator<const CircularBuffer,CircularBuffer,const T> const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  // ctor/dtor
  constexpr
  explicit CircularBuffer() :
    first_(), last_(), bytes_()
  { }

  // iterators
  iterator begin() {
    return iterator(this, 0);
  }

  constexpr
  const_iterator begin() const {
    return const_iterator(this, 0);
  }

  constexpr
  const_iterator cbegin() const {
    return const_iterator(this, 0);
  }

  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }

  constexpr
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  constexpr
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(end());
  }

  iterator end() {
    return iterator(this, size());
  }

  constexpr
  const_iterator end() const {
    return const_iterator(this, size());
  }

  constexpr
  const_iterator cend() const {
    return const_iterator(this, size());
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
    return first_.n_ == last_.n_;
  }

  constexpr
  size_type size() const {
    return (first_.n_ <= last_.n_)
        ? last_.n_ - first_.n_
        : N - first_.n_ + last_.n_;
  }

  constexpr
  size_type length() const {
    return size();
  }

  constexpr
  size_type capacity() const {
    return N - 1;
  }

  constexpr
  size_type max_size() const {
    return capacity();
  }

  // element access
  reference front() {
    return bytes_[first_ + 1];
  }

  reference back() {
    return bytes_[last_.n_];
  }

  constexpr
  const_reference front() const {
    return bytes_[first_ + 1];
  }

  constexpr
  const_reference back() const {
    return bytes_[last_.n_];
  }

  reference operator[](size_type n) {
    return bytes_[first_ + n + 1];
  }

  constexpr
  const_reference operator[](size_type n) const {
    return bytes_[first_ + n + 1];
  }

  // modifiers
  void pop_front() {
    if (!empty()) {
      ++first_;
      bytes_.erase(first_.n_);
    }
  }

  void pop_back() {
    if (!empty()) {
      bytes_.erase(last_.n_);
      --last_;
    }
  }

  void push_back(const T& x) {
    ++last_;

#ifndef NDEBUG
    if (first_ == last_) {
      error("CircularBuffer overflow!");
      --last_;
      return ;
    }
#endif

    bytes_[last_.n_] = x;
  }

  template< class... Args >
  void emplace_back(Args&&... args) {
    ++last_;

#ifndef NDEBUG
    if (first_.n_ == last_.n_) {
      error("CircularBuffer overflow!");
      --last_;
      return ;
    }
#endif

    bytes_.emplace(last_.n_, std::forward<Args>(args)...);
  }

  void push_front(const T& x) {
    element(first_.n_) = x;
    --first_;

#ifndef NDEBUG
    if (last_ == first_) {
      error("CircularBuffer overflow!");
      ++first_;
      return ;
    }
#endif
  }

  template< class... Args >
  void emplace_front(Args&&... args) {
    bytes_.emplace(first_.n_, std::forward<Args>(args)...);
    --first_;

#ifndef NDEBUG
    if (last_.n_ == first_.n_) {
      error("CircularBuffer overflow!");
      ++first_;
      return ;
    }
#endif
  }

  void clear() {
// TODO: for not trival destructable types
//    for (size_t i = (first_.n_+1)&intern_iterator::mask; i != last_.n_; i = (i+1)&intern_iterator::mask) {
//      bytes_.erase(i);
//    }
    first_.n_ = last_.n_ = 0;
  }

private:
  // intern_iterator
  class intern_iterator {
  public:
    constexpr
    explicit intern_iterator() :
      n_(0)
    { }

    // prefix ++
    intern_iterator& operator++() {
      return (*this += 1);
    }

    // prefix --
    intern_iterator& operator--() {
      return (*this -= 1);
    }

    intern_iterator& operator+=(size_t n) {
      n_ = (n_ + n) & mask;
      return *this;
    }

    intern_iterator& operator-=(size_t n) {
      n_ = (n_ - n) & mask;
      return *this;
    }

    constexpr
    size_t operator+(int n) const {
      return (n_ + n) & mask;
    }

    constexpr
    size_t operator-(int n) const {
      return (n_ - n) & mask;
    }

    constexpr
    bool operator==(const intern_iterator& other) const {
      return n_ == other.n_;
    }

    constexpr
    bool operator!=(const intern_iterator& other) const {
      return n_ != other.n_;
    }

    // Number of the maximum events + 1 (must be a power of two)
    static const std::size_t size = N;

    // Bit mask for circular buffer indices
    static const std::size_t mask = size - 1;

    std::size_t n_;
  };

  // Index to first event of queue
  intern_iterator first_;

  // Index to last event of queue
  intern_iterator last_;

  // bytes needed to work a round the value ctor at construction
  array_storage<T, N> bytes_;
};

} // namespace TURAG

#endif // CIRCULAR_BUFFER_H
