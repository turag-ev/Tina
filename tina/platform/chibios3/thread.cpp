#define TURAG_DEBUG_LOG_SOURCE "_"
#include <ch.h>
#include <cstdint>
#include "public/tina++/thread.h"
#include "public/tina/thread.h"

namespace TURAG {

extern "C"
void _turag_thread_entry(void* data) {
  void (*entry)(void) = (void (*)(void))data;
  entry();

  chThdExit(1);
}

#ifdef CH_DBG_FILL_THREADS

std::size_t get_stack_usage(thread_t* t, std::size_t stack_size) {
    const char* end = reinterpret_cast<const char*>(t->wabase);
    const char* begin = end + stack_size;
    for (const char* ptr = end; ptr != begin; ptr++) {
        if (*ptr != 0x55) { // ChibiOS fills stack with 0x55 on initialization
            return (begin - ptr) * sizeof(char);
        }
    }
    return 0;
}

std::size_t ThreadImpl::getStackUsage() const {
    return get_stack_usage(thread_, stack_size_);
}

extern "C"
size_t turag_thread_get_stack_usage(const TuragThread* thread) {
  return get_stack_usage(thread->thread, thread->stack_size);
}
#else // CH_DBG_FILL_THREADS

std::size_t ThreadImpl::getStackUsage() const {
    return 0;
}

extern "C"
size_t turag_thread_get_stack_usage(const TuragThread* thread) {
  return 0;
}

#endif // CH_DBG_FILL_THREADS



bool Semaphore::wait(SystemTime time) {
	while (1) {
		msg_t result = chSemWaitTimeout(&sem_, time.toTicks());

		if (result == MSG_OK) {
			return true;
		} else if (result == MSG_TIMEOUT) {
			return false;
		}
	}
}

bool BinarySemaphore::wait(SystemTime time) {
    while (1) {
        msg_t result = chBSemWaitTimeout(&sem_, time.toTicks());

        if (result == MSG_OK) {
            return true;
        } else if (result == MSG_TIMEOUT) {
            return false;
        }
    }
}

} /* namespace TURAG */
