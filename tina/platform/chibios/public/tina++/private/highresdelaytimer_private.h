#ifndef TINAPP_CHIBIOS_HIGHRESDELAYTIMER_PRIVATE_H
#define TINAPP_CHIBIOS_HIGHRESDELAYTIMER_PRIVATE_H

#include <hal.h>
#include <ch.h>

#include <tina++/thread.h>

#include <cstddef>

#if HAL_USE_GPT == FALSE
# error HAL_USE_GPT must be TRUE
#endif


#ifndef __DOXYGEN__

namespace TURAG {
	
	
struct GPTConfigExt {
	GPTConfig config;
	BinarySemaphore sem;
};

// required for our pointer magic in the implemantation
static_assert(offsetof(GPTConfigExt, config) == 0, "compiler invalidates implementation");


class HighResDelayTimerPrivate {
public:
	GPTDriver* driver;
	GPTConfigExt config;
	uint16_t factor;
	uint16_t max_us;
};


} // namespace TURAG


#endif // __DOXYGEN__

#endif // TINAPP_CHIBIOS_HIGHRESDELAYTIMER_PRIVATE_H
