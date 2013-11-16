#include "public/tina++/thread.h"
#include "public/tina/thread.h"
#include <tina/debug.h>
#include <cstdint>

#include <tina/debug.h>
#include <stackusage.h>

typedef Thread ChThread;

namespace TURAG {

namespace detail {

TURAG_THREAD_ENTRY
msg_t thread_entry(void* data) {
  TURAG_THREAD_ENTRY void (*entry)(uint32_t) =
    reinterpret_cast<void (*)(uint32_t)>(data);
    
  entry(0);
}

extern "C" TURAG_THREAD_ENTRY
msg_t _turag_thread_entry(void* data) {
  TURAG_THREAD_ENTRY void (*entry)(uint32_t) = (void (*)(uint32_t))data;
  entry(0);
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
