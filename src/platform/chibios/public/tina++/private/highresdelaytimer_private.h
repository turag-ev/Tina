#ifndef PLATFORM_CHIBIOS3_PUBLIC_TINAPP_PRIVATE_HIGHRESDELAYTIMER_PRIVATE_H
#define PLATFORM_CHIBIOS3_PUBLIC_TINAPP_PRIVATE_HIGHRESDELAYTIMER_PRIVATE_H

#include <hal.h>
#include <ch.h>

#include <tina++/thread.h>

#include <cstddef>
#include <atomic>


#if HAL_USE_GPT == FALSE
# warning HAL_USE_GPT must be TRUE
#endif


#ifndef __DOXYGEN__

namespace TURAG {
	
	
struct GPTConfigExt {
	GPTConfig config;
    binary_semaphore_t sem; //we need to use I-class APIs on it :/
    volatile std::atomic_bool timerRunning;
};

// required for our pointer magic in the implementation
static_assert(offsetof(GPTConfigExt, config) == 0, "compiler invalidates implementation");


class HighResDelayTimerPrivate {
public:
    void doWait(uint32_t us, bool block);

	GPTDriver* driver;
	GPTConfigExt config;
    SystemTime waitUntil;
	uint16_t factor;
	uint16_t max_us;
};


} // namespace TURAG


#endif // __DOXYGEN__

#endif // PLATFORM_CHIBIOS3_PUBLIC_TINAPP_PRIVATE_HIGHRESDELAYTIMER_PRIVATE_H
