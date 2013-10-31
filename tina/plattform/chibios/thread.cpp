#include "public/tina++/thread.h"
#include "public/tina/thread.h"
#include <tina/debug.h>
#include <cstdint>

#include <tina/debug.h>
#include <stackusage.h>

typedef Thread ChThread;

namespace TURAG {

namespace detail {

msg_t thread_entry(void* data) {
  reinterpret_cast<void (*)(uint32_t)>(data)(0);
  return 0;
}

extern "C"
msg_t _turag_thread_entry(void* data) {
  ((void (*)(uint32_t))data)(0);
  return 0;
}

#ifdef CH_DBG_FILL_THREADS

std::size_t thread_get_stack_usage(const char* stack, std::size_t stack_size) {
    return su_get_stack_usage(stack, stack_size);
}

extern "C"
size_t turag_thread_get_stack_usage(const TuragThread* thread) {
  return su_get_stack_usage(static_cast<const char*>(thread->stack),
                                thread->stack_size);
}

#endif

} // namespace detail

} /* namespace TURAG */
