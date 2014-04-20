#include "public/tina++/thread.h"
#include "public/tina/thread.h"
#include <tina/debug.h>
#include <cstdint>

#include <tina/debug.h>
#include <stackusage.h>

typedef Thread ChThread;

namespace TURAG {

namespace detail {

extern "C"
void _turag_thread_entry(void* data) {
  void (*entry)(void) = (void (*)(void))data;
  entry();
  
  chThdExit(1);
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

bool Semaphore::wait(SystemTime time) {
	while (1) {
		msg_t result = chSemWaitTimeout(&sem_, time.value);

		if (result == RDY_OK) {
			return true;
		} else if (result == RDY_TIMEOUT) {
			return false;
		}
	}
}

bool BinarySemaphore::wait(SystemTime time) {
	while (1) {
		msg_t result = chBSemWaitTimeout(&sem_, time.value);

		if (result == RDY_OK) {
			return true;
		} else if (result == RDY_TIMEOUT) {
			return false;
		}
	}
}

} /* namespace TURAG */
