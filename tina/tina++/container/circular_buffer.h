#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <functional>

#include "../tina.h"
#include "array_storage.h"
#include "../helper/integer.h"

#include <tina/debug/internal-debug.h>

namespace TURAG {

template<typename... _Cond>
using Require = typename std::enable_if<std::__and_<_Cond...>::value>::type;

template<typename Container, class Enable = void>
struct container_traits { };

template<typename Container>
struct container_traits<Container, typename std::enable_if<!std::is_const<Container>::value>::type> {
    typedef typename Container::value_type value_type;
    typedef typename Container::reference reference;
    typedef typename Container::pointer pointer;

    // iterator
    typedef typename Container::iterator iterator;
    typedef typename Container::reverse_iterator reverse_iterator;
};

template<typename Container>
struct container_traits<Container, typename std::enable_if<std::is_const<Container>::value>::type> {
    typedef const typename Container::value_type value_type;
    typedef typename Container::const_reference reference;
    typedef typename Container::const_pointer pointer;

    // iterator
    typedef typename Container::const_iterator iterator;
    typedef typename Container::const_reverse_iterator reverse_iterator;
};


template<typename Container>
class CircularBufferIterator : public std::iterator<std::random_access_iterator_tag, typename container_traits<Container>::value_type> {
private:
  typedef CircularBufferIterator self_type;
  typedef CircularBufferIterator<const Container> const_self_type;

  typedef typename container_traits<Container>::reference element_ref;
  typedef typename container_traits<Container>::pointer element_pointer;

public:

  // ctor
  constexpr
  CircularBufferIterator(Container& queue, size_t pos) :
    queue_(queue), pos_(pos)
  { }

  // convert from iterator to const_iterator
  template<typename OtherContainer>
  constexpr
  CircularBufferIterator(const CircularBufferIterator<OtherContainer> &other) :
    queue_(other.queue_), pos_(other.pos_)
  { }

  CircularBufferIterator& operator=(const CircularBufferIterator &other)
  {
      queue_ = other.queue_;
      pos_ = other.pos_;
      return *this;
  }

  template<typename OtherContainer>
  CircularBufferIterator& operator=(const CircularBufferIterator<OtherContainer> &other)
  {
      queue_ = other.queue_;
      pos_ = other.pos_;
      return *this;
  }

  _always_inline
  element_ref operator*() {
	return (queue_.get())[pos_];
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
  element_pointer operator->()  {
    return &(operator*());
  }

  self_type& operator++() {
    return (*this += 1);
  }

  const_self_type operator++(int) {
    const self_type old = *this;
    ++(*this);
    return old;
  }

  self_type& operator--() {
    return (*this -= 1);
  }

  const_self_type operator--(int) {
    const self_type old = *this;
    --(*this);
    return old;
  }

  constexpr
  bool operator==(const self_type& other) const {
	return &(queue_.get()) == &(other.queue_.get()) && pos_ == other.pos_;
  }

  constexpr
  bool operator!=(const self_type& other) const {
    return !(*this == other);
  }

  constexpr
  typename Container::difference_type index() const {
      return pos_;
  }

  constexpr
  const Container& container() const {
	  return queue_.get();
  }

private:
  template <class> friend class CircularBufferIterator;

  std::reference_wrapper<Container> queue_;
  std::size_t pos_;
};

template<typename T>
typename T::difference_type operator-(
         CircularBufferIterator<T> lhs,
         CircularBufferIterator<T> rhs)
{
    return lhs.index() - rhs.index();
}

#ifndef __DOXYGEN__

namespace detail {

#endif

/// \brief Ringbuffer
/// \ingroup Container
/// \code
///   -empty-     -2 elements-   -3 elements-
///  +-+-+-+-+     +-+-+-+-+      +-+-+-+-+
///  | | | | |     |1|2| | |      |3| |1|2|
///  +-+-+-+-+     +-+-+-+-+      +-+-+-+-+
///   ^             ^                  ^
///  first          first              first
/// \endcode
template<typename T, std::size_t N>
class CircularBuffer {
  static_assert(!(N & (N-1)), "detail::CircularBuffer capacity must be a power of 2");
  //static_assert(std::is_trivially_destructible<T>::value, "Es werde erstmal nur trivale Typen unterstützt.");

public:
  // types
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  // iterator
  typedef CircularBufferIterator<CircularBuffer> iterator;
  typedef CircularBufferIterator<const CircularBuffer> const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  // ctor/dtor
  constexpr
  explicit CircularBuffer() :
	first_(), length_(0), bytes_()
  { }


  CircularBuffer(const CircularBuffer& other) :
	first_(), length_(0), bytes_()
  { 
	copy(other);
  }

  CircularBuffer& operator=(const CircularBuffer& other)
  { 
	copy(other);
    return *this;
  }

  CircularBuffer(CircularBuffer&& other) :
	first_(), length_(0), bytes_()
  {
	move(other);
  }

  CircularBuffer& operator=(CircularBuffer&& other)
  {
	move(other);
	return *this;
  }

  ~CircularBuffer() {
      destruct(begin(), end());
  }

  // iterators
  iterator begin() {
    return iterator(*this, 0);
  }

  constexpr
  const_iterator begin() const {
    return const_iterator(*this, 0);
  }

  constexpr
  const_iterator cbegin() const {
    return const_iterator(*this, 0);
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
    return iterator(*this, size());
  }

  constexpr
  const_iterator end() const {
    return const_iterator(*this, size());
  }

  constexpr
  const_iterator cend() const {
    return const_iterator(*this, size());
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
  bool full() const {
	return length_ >= capacity();
  }

  constexpr
  size_type size() const {
	return length_;
  }

  constexpr
  size_type length() const {
    return size();
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
	return bytes_[first_.n_];
  }

  reference back() {
	return bytes_[first_ + (length_-1)];
  }

  constexpr
  const_reference front() const {
	return bytes_[first_.n_];
  }

  constexpr
  const_reference back() const {
	return bytes_[first_ + (length_-1)];
  }

  reference operator[](size_type n) {
	return bytes_[first_ + n];
  }

  constexpr
  const_reference operator[](size_type n) const {
	return bytes_[first_ + n];
  }

  // modifiers
  void pop_front() {
	  if (unlikely(empty())) {
		  turag_internal_error("CircularBuffer underflow!");
		  return ;
	  }

      bytes_.erase(first_.n_);
	  ++first_;
	  --length_;
  }

  void pop_back() {
	if (unlikely(empty())) {
		turag_internal_error("CircularBuffer underflow!");
		return ;
	}

	bytes_.erase(first_ + length_);
	--length_;
  }

  void push_back(const T& x) {
	if (unlikely(length_ >= capacity())) {
	  turag_internal_error("CircularBuffer overflow!");
      return ;
    }

	bytes_.construct(first_ + length_, x);
	++length_;
  }

  template< class... Args >
  void emplace_back(Args&&... args) {
	if (unlikely(length_ >= capacity())) {
	  turag_internal_error("CircularBuffer overflow!");
      return ;
    }

	bytes_.construct(first_ + length_, std::forward<Args>(args)...);
	++length_;
  }

  void push_front(const T& x) {
	  if (unlikely(length_ >= capacity())) {
		  turag_internal_error("CircularBuffer overflow!");
		  return ;
	  }

	  ++length_;
	  --first_;
	  bytes_.construct(first_.n_,  x);
  }

  template< class... Args >
  void emplace_front(Args&&... args) {
	  if (unlikely(length_ >= capacity())) {
		  turag_internal_error("CircularBuffer overflow!");
		  return ;
	  }

	  ++length_;
	  --first_;
	  bytes_.construct(first_.n_, std::forward<Args>(args)...);
  }

  void clear() {
    destruct(begin(), end());
	first_.n_ = length_ = 0;
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

    // Number of the maximum elements + 1 (must be a power of two)
    static const std::size_t size = N;

    // Bit mask for circular buffer indices
    static const std::size_t mask = size - 1;

    std::size_t n_;
  };

  // Index to first element of queue
  intern_iterator first_;

  // Index to last element of queue
  std::size_t length_;

  // bytes needed to work around the value ctor at construction
  ArrayStorage<T, N> bytes_;

  template<typename U, std::size_t M>
  void copy(const CircularBuffer<U, M>& other) {
	  if (unlikely(this == &other)) return;

	  std::size_t new_size = other.size();
	  if (M > N) {
		  if (unlikely(new_size > capacity())) {
			  turag_internal_error("CircularBuffer overflow!");
			  new_size = capacity();
		  }
	  }

	  if (new_size <= size()) {
		  auto new_end = TURAG::copy(other.begin(), other.end(), begin());
		  destruct(new_end, end());
	  } else {
		  auto other_mid = other.begin() + size();
		  auto self_end = TURAG::copy(other.begin(), other_mid, begin());
		  TURAG::uninitialized_copy(other_mid, other.end(), self_end);
	  }

	  length_ = new_size;
  }

  template<typename U, std::size_t M>
  void move(CircularBuffer<U, M>&& other) {
	  if (unlikely(this == &other)) return;

	  std::size_t new_size = other.size();
	  if (M > N) {
		  if (unlikely(new_size > capacity())) {
			  turag_internal_error("CircularBuffer overflow!");
			  new_size = capacity();
		  }
	  }

	  if (new_size <= size()) {
		  auto new_end = TURAG::move(other.begin(), other.end(), begin());
		  destruct(new_end, end());
	  } else {
		  auto other_mid = other.begin() + size();
		  auto self_end = TURAG::move(other.begin(), other_mid, begin());
		  TURAG::uninitialized_move(other_mid, other.end(), self_end);
	  }

	  length_ = new_size;
  }
};

#ifndef __DOXYGEN__

} // namespace detail

template<typename T, std::size_t N>
using CircularBuffer = detail::CircularBuffer<T, next_power_of_two(N)>;

#endif

} // namespace TURAG

#endif // CIRCULAR_BUFFER_H
