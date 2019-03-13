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

std::size_t get_stack_usage(const char* working_area_base, std::size_t stack_size) {
    const char* end = working_area_base;
    const char* begin = end + stack_size;
    for (const char* ptr = end; ptr != begin; ptr++) {
        if (*ptr != 0x55) { // ChibiOS fills stack with 0x55 on initialization
            return (begin - ptr) * sizeof(char);
        }
    }
    return stack_size; //stack full
}

std::size_t ThreadImpl::getStackUsage() const {
    return get_stack_usage(reinterpret_cast<const char*>(thread_->wabase), stack_size_);
}

extern "C"
size_t turag_thread_get_stack_usage(const TuragThread* thread) {
  return get_stack_usage(reinterpret_cast<const char*>(thread->thread->wabase), thread->stack_size);
}


extern "C" const char __main_stack_base__, __main_stack_end__;
std::size_t getMainStackUsage() { 
    return get_stack_usage(&__main_stack_base__, (&__main_stack_end__) - (&__main_stack_base__));
}

extern "C" const char __process_stack_base__, __process_stack_end__;
std::size_t getProcessStackUsage() {
    return get_stack_usage(&__process_stack_base__, (&__process_stack_end__) - (&__process_stack_base__));
}

#else // CH_DBG_FILL_THREADS

std::size_t ThreadImpl::getStackUsage() const { return 0; }
extern "C" size_t turag_thread_get_stack_usage(const TuragThread* thread) { return 0; }

std::size_t getMainStackUsage() { return 0; }
std::size_t getProcessStackUsage() { return 0; }

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
