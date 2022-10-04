#ifndef TINAPP_DEBUG_BINARY_H
#define TINAPP_DEBUG_BINARY_H

#include <type_traits>
#include <utility>

#include <tina++/tina.h>
#include <tina/debug/binary.h>

namespace TURAG {

template<typename T, class Enable = void>
struct TURAG_PACKED DebugObject
{
	DebugObject() = default;

	template<typename...Args>
	DebugObject(Args&&...args) :
		version(T::Version), data(std::forward<Args>(args)...)
	{ }

	unsigned char version;
	T data;
};

// with empty data type
template<typename T>
struct TURAG_PACKED DebugObject<T, typename std::enable_if<std::is_empty<T>::value>::type>
{
	unsigned char version;
};

template<typename T> inline
void send(DebugObject<T>& obj)
{
	obj.version = T::Version;
	turag_binary_send(T::DebugObjectId, obj);
}

} // namespace TURAG

#endif // TINAPP_DEBUG_BINARY_H
