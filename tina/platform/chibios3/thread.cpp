#define TURAG_DEBUG_LOG_SOURCE "_"
#include "public/tina++/thread.h"
#include "public/tina/thread.h"
#include <cstdint>

#if defined(CH_DBG_FILL_THREADS) && CH_DBG_FILL_THREADS == TRUE
#include <stackusage.h>
#endif // CH_DBG_FILL_THREADS

namespace TURAG {


extern "C"
void _turag_thread_entry(void* data) {
  void (*entry)(void) = (void (*)(void))data;
  entry();

  chThdExit(1);
}

#if defined(CH_DBG_FILL_THREADS) && CH_DBG_FILL_THREADS == TRUE

std::size_t ThreadImpl::getStackUsage() const {
    return su_get_stack_usage(static_cast<char*>(working_area_), stack_size_);
}

extern "C"
size_t turag_thread_get_stack_usage(const TuragThread* thread) {
  return su_get_stack_usage(static_cast<const char*>(thread->stack),
                            thread->stack_size);
}
#else // CH_DBG_FILL_THREADS

std::size_t ThreadImpl::getStackUsage() const {
    return 0;
}

extern "C"
size_t turag_thread_get_stack_usage(const TuragThread* thread) {
    UNUSED(thread);
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

// bool BinarySemaphore::wait(SystemTime time) {
// 	while (1) {
// 		msg_t result = chBSemWaitTimeout(&sem_, time.value);
// 
// 		if (result == RDY_OK) {
// 			return true;
// 		} else if (result == RDY_TIMEOUT) {
// 			return false;
// 		}
// 	}
// }

} /* namespace TURAG */
