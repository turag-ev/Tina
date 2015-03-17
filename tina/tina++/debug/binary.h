#ifndef TINAPP_DEBUG_BINARY_H
#define TINAPP_DEBUG_BINARY_H

#include <type_traits>
#include <utility>

#include <tina++/tina.h>
#include <tina/debug/binary.h>

namespace TURAG {

template<typename T, class Enable = void>
struct DebugObject {
	enum : unsigned char { Version = T::Version };

	template<typename...Args>
	DebugObject(Args&&...args) :
		data(std::forward<Args>(args)...)
	{ }

	unsigned char version;
	T data;
} _packed;

// with empty data type
template<typename T>
struct DebugObject<T, typename std::enable_if<std::is_empty<T>::value>::type> {
	enum : unsigned char { Version = T::Version };

	unsigned char version;
};

} // namespace TURAG

#endif // TINAPP_DEBUG_BINARY_H
