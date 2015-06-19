#ifndef TINAPP_CONTAINER_QUEUE_H
#define TINAPP_CONTAINER_QUEUE_H

#include "circular_buffer.h"

namespace TURAG {

template<typename T, std::size_t N>
using Queue = CircularBuffer<T, N>;

} // namespace TURAG

#endif // TINAPP_CONTAINER_QUEUE_H

