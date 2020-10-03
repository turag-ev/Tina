#ifndef TINAPP_CONTAINER_ROLLING_BUFFER_H
#define TINAPP_CONTAINER_ROLLING_BUFFER_H

#include "circular_buffer.h"

namespace TURAG {

template<typename T, std::size_t N>
class RollingBuffer : private CircularBuffer<T,N> {
	using base_type = CircularBuffer<T,N>;
public:
	// types
	typedef typename base_type::value_type value_type;
	typedef typename base_type::reference reference;
	typedef typename base_type::const_reference const_reference;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::const_pointer const_pointer;
	typedef typename base_type::size_type size_type;
	typedef typename base_type::difference_type difference_type;

	// iterator
	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;
	typedef typename base_type::reverse_iterator reverse_iterator;
	typedef typename base_type::const_reverse_iterator const_reverse_iterator;

	// ctor/dtor
	constexpr
	explicit RollingBuffer() :
	  base_type()
	{ }

	// iterators
	iterator begin() {
	  return base_type::begin();
	}

	constexpr
	const_iterator begin() const {
	  return base_type::begin();
	}

	constexpr
	const_iterator cbegin() const {
	  return base_type::cbegin();
	}

	reverse_iterator rbegin() {
	  return base_type::rbegin();
	}

	constexpr
	const_reverse_iterator rbegin() const {
	  return base_type::rbegin();
	}

	constexpr
	const_reverse_iterator crbegin() const {
	  return base_type::crbegin();
	}

	iterator end() {
	  return base_type::end();
	}

	constexpr
	const_iterator end() const {
	  return base_type::end();
	}

	constexpr
	const_iterator cend() const {
	  return base_type::cend();
	}

	reverse_iterator rend() {
	  return base_type::rend();
	}

	constexpr
	const_reverse_iterator rend() const {
	  return base_type::rend();
	}

	constexpr
	const_reverse_iterator crend() const {
	  return base_type::crend();
	}

	// capacity
	constexpr
	bool empty() const {
	  return base_type::empty();
	}

	constexpr
	bool full() const {
	  return base_type::full();
	}

	constexpr
	size_type size() const {
	  return base_type::size();
	}

	constexpr
	size_type length() const {
	  return base_type::length();
	}

	constexpr
	size_type capacity() const {
	  return base_type::capacity();
	}

	constexpr
	size_type max_size() const {
	  return base_type::max_size();
	}

	// element access
	reference front() {
	  return base_type::front();
	}

	reference back() {
	  return base_type::back();
	}

	constexpr
	const_reference front() const {
	  return base_type::front();
	}

	constexpr
	const_reference back() const {
	  return base_type::back();
	}

	reference operator[](size_type n) {
	  return base_type::operator[](n);
	}

	constexpr
	const_reference operator[](size_type n) const {
	  return base_type::operator[](n);
	}

	// modifiers
	void pop_back() {
	  base_type::pop_back();
	}

	void push_back(const T& x) {
		while (size() >= N) base_type::pop_front();
	  base_type::push_back(x);
	}

	template< class... Args >
	void emplace_back(Args&&... args) {
		while (size() >= N) base_type::pop_front();
	  base_type::emplace_back(args...);
	}

	void clear() {
	  base_type::clear();
	}
};

} // namespace TURAG

#endif // TINAPP_CONTAINER_ROLLING_BUFFER_H
