#include "public/tina++/thread.h"
#include "public/tina/thread.h"

namespace TURAG {

namespace detail {

extern "C" TURAG_THREAD_ENTRY
void _turag_thread_entry(cyg_addrword_t data) {
  TURAG_THREAD_ENTRY void (*entry)(void) = (void (*)(void))data;
  entry();
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

} // namespace TURAG
