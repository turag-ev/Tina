#ifndef TINAPP_CONTAINER_ARRAY_REF_H
#define TINAPP_CONTAINER_ARRAY_REF_H

#include <tina++/tina.h>
#include <type_traits>

namespace std {
template<typename T, std::size_t N> class array;
template<typename T, typename Alloc> class vector;
} // namespace std

namespace TURAG {

template<typename T>
class ArrayRef {
	using T_nonconst = typename std::remove_const<T>::type;

public:
	template<std::size_t N>
	explicit constexpr ArrayRef(const T_nonconst (&a)[N]) :
		ptr_(a), size_(N)
	{ }

	template<std::size_t N>
	explicit constexpr ArrayRef(T_nonconst (&a)[N]) :
		ptr_(a), size_(N)
	{ }

	template<std::size_t N>
	explicit constexpr ArrayRef(const std::array<const T_nonconst,N>& a) :
		ptr_(a.data()), size_(a.size())
	{ }

	template<std::size_t N>
	explicit constexpr ArrayRef(const std::array<T_nonconst,N>& a) :
		ptr_(a.data()), size_(a.size())
	{ }

	template<typename Alloc>
	explicit constexpr ArrayRef(const std::vector<const T_nonconst,Alloc>& a) :
		ptr_(a.data()), size_(a.size())
	{ }

	template<typename Alloc>
	explicit constexpr ArrayRef(const std::vector<T_nonconst,Alloc>& a) :
		ptr_(a.data()), size_(a.size())
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

#endif // TINAPP_CONTAINER_ARRAY_REF_H
