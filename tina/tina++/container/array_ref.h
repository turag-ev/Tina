#ifndef ARRAYREF_H
#define ARRAYREF_H

#include <tina++/tina.h>

namespace TURAG {

template<typename T>
class ArrayRef {
public:
	template<std::size_t N>
	explicit constexpr ArrayRef(T (&a)[N]) :
		ptr_(a), size_(N)
	{ }

	explicit constexpr ArrayRef() :
		ptr_(nullptr), size_(0)
	{ }

	T* begin() { return ptr_; }
	constexpr const T* begin() const { return ptr_; }
	constexpr const T* cbegin() const { return ptr_; }
	T* end() { return ptr_ + size_; }
	constexpr const T* end() const { return ptr_ + size_; }
	constexpr const T* cend() const { return ptr_ + size_; }

	constexpr std::size_t size() const { return size_; }
	constexpr std::size_t length() const { return size_; }

	T& operator[](std::size_t index) {
		return *(ptr_ + index);
	}

	constexpr const T& operator[](std::size_t index) const {
		return *(ptr_ + index);
	}

private:
	T* ptr_;
	std::size_t size_;
};

} // namespace TURAG

#endif // ARRAYREF_H
