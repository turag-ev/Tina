#ifndef TINAPP_CONTAINER_STACK_H
#define TINAPP_CONTAINER_STACK_H

#include "array_buffer.h"

namespace TURAG {

template<typename T, std::size_t N>
using Stack = ArrayBuffer<T, N>;

} // namespace TURAG

#endif // TINAPP_CONTAINER_STACK_H

