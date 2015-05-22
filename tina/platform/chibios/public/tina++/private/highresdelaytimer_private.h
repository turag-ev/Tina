#ifndef TINAPP_CHIBIOS_HIGHRESDELAYTIMER_PRIVATE_H
#define TINAPP_CHIBIOS_HIGHRESDELAYTIMER_PRIVATE_H

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
	BinarySemaphore sem;
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

#endif // TINAPP_CHIBIOS_HIGHRESDELAYTIMER_PRIVATE_H
