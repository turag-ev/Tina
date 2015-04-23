
#define TURAG_DEBUG_LOG_SOURCE "~"

#include <tina++/tina.h>
#include <tina++/utils/highresdelaytimer.h>
#include <tina++/debug.h>
#include <tina++/thread.h>
#include <tina++/time.h>
#include <tina++/algorithm.h>

#include <hal.h>
#include <mcuconf.h>

#include <atomic>


#if TURAG_USE_TINA_HIGH_RES_DELAY_TIMER

#ifndef TURAG_HIGHRESDELAY_TIMER1_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER1_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER2_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER2_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER3_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER3_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER4_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER4_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER5_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER5_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER6_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER6_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER7_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER7_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER8_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER8_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER9_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER9_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER11_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER11_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER12_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER12_FREQUENCY 0
#endif
#ifndef TURAG_HIGHRESDELAY_TIMER14_FREQUENCY
#define TURAG_HIGHRESDELAY_TIMER14_FREQUENCY 0
#endif

#if !STM32_GPT_USE_TIM1 && TURAG_HIGHRESDELAY_TIMER1_FREQUENCY
# error STM32_GPT_USE_TIM1 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM2 && TURAG_HIGHRESDELAY_TIMER2_FREQUENCY
# error STM32_GPT_USE_TIM2 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM3 && TURAG_HIGHRESDELAY_TIMER3_FREQUENCY
# error STM32_GPT_USE_TIM3 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM4 && TURAG_HIGHRESDELAY_TIMER4_FREQUENCY
# error STM32_GPT_USE_TIM4 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM5 && TURAG_HIGHRESDELAY_TIMER5_FREQUENCY
# error STM32_GPT_USE_TIM5 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM6 && TURAG_HIGHRESDELAY_TIMER6_FREQUENCY
# error STM32_GPT_USE_TIM6 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM7 && TURAG_HIGHRESDELAY_TIMER7_FREQUENCY
# error STM32_GPT_USE_TIM7 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM8 && TURAG_HIGHRESDELAY_TIMER8_FREQUENCY
# error STM32_GPT_USE_TIM8 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM9 && TURAG_HIGHRESDELAY_TIMER9_FREQUENCY
# error STM32_GPT_USE_TIM9 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM11 && TURAG_HIGHRESDELAY_TIMER11_FREQUENCY
# error STM32_GPT_USE_TIM11 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM12 && TURAG_HIGHRESDELAY_TIMER12_FREQUENCY
# error STM32_GPT_USE_TIM12 must be defined to TRUE
#endif
#if !STM32_GPT_USE_TIM14 && TURAG_HIGHRESDELAY_TIMER14_FREQUENCY
# error STM32_GPT_USE_TIM14 must be defined to TRUE
#endif

#if !TURAG_HIGHRESDELAY_TIMER1_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER2_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER3_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER4_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER5_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER6_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER7_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER8_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER9_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER11_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER12_FREQUENCY && \
	!TURAG_HIGHRESDELAY_TIMER14_FREQUENCY
# error TURAG_USE_TINA_HIGH_RES_DELAY_TIMER activated, but all frequencies are zero
#endif

static_assert(TURAG_HIGHRESDELAY_TIMER1_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER1_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER2_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER2_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER3_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER3_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER4_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER4_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER5_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER5_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER6_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER6_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER7_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER7_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER8_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER8_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER9_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER9_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER11_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER11_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER12_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER12_FREQUENCY must be a multiple of 1000000");
static_assert(TURAG_HIGHRESDELAY_TIMER14_FREQUENCY % 1000000 == 0, "TURAG_HIGHRESDELAY_TIMER14_FREQUENCY must be a multiple of 1000000");

namespace {
	
struct HighResDelayConfig {
	GPTDriver* driver;
	uint32_t frequency;
	const uint16_t factor;
	const uint16_t max_us;
};
	
HighResDelayConfig availableTimers[] =  {
	#if STM32_GPT_USE_TIM1 && TURAG_HIGHRESDELAY_TIMER1_FREQUENCY
		{
			.driver = &GPTD1,
			.frequency = TURAG_HIGHRESDELAY_TIMER1_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER1_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER1_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM2 && TURAG_HIGHRESDELAY_TIMER2_FREQUENCY
		{
			.driver = &GPTD2,
			.frequency = TURAG_HIGHRESDELAY_TIMER2_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER2_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER2_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM3 && TURAG_HIGHRESDELAY_TIMER3_FREQUENCY
		{
			.driver = &GPTD3,
			.frequency = TURAG_HIGHRESDELAY_TIMER3_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER3_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER3_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM4 && TURAG_HIGHRESDELAY_TIMER4_FREQUENCY
		{
			.driver = &GPTD4,
			.frequency = TURAG_HIGHRESDELAY_TIMER4_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER4_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER4_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM5 && TURAG_HIGHRESDELAY_TIMER5_FREQUENCY
		{
			.driver = &GPTD5,
			.frequency = TURAG_HIGHRESDELAY_TIMER5_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER5_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER5_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM6 && TURAG_HIGHRESDELAY_TIMER6_FREQUENCY
		{
			.driver = &GPTD6,
			.frequency = TURAG_HIGHRESDELAY_TIMER6_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER6_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER6_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM7 && TURAG_HIGHRESDELAY_TIMER7_FREQUENCY
		{
			.driver = &GPTD7,
			.frequency = TURAG_HIGHRESDELAY_TIMER7_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER7_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER7_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM8 && TURAG_HIGHRESDELAY_TIMER8_FREQUENCY
		{
			.driver = &GPTD8,
			.frequency = TURAG_HIGHRESDELAY_TIMER8_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER8_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER8_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM9 && TURAG_HIGHRESDELAY_TIMER9_FREQUENCY
		{
			.driver = &GPTD9,
			.frequency = TURAG_HIGHRESDELAY_TIMER9_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER9_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER9_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM11 && TURAG_HIGHRESDELAY_TIMER11_FREQUENCY
		{
			.driver = &GPTD11,
			.frequency = TURAG_HIGHRESDELAY_TIMER11_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER11_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER11_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM12 && TURAG_HIGHRESDELAY_TIMER12_FREQUENCY
		{
			.driver = &GPTD12,
			.frequency = TURAG_HIGHRESDELAY_TIMER12_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER12_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER12_FREQUENCY / 1000000)
		}
	#endif
	#if STM32_GPT_USE_TIM14 && TURAG_HIGHRESDELAY_TIMER14_FREQUENCY
		{
			.driver = &GPTD14,
			.frequency = TURAG_HIGHRESDELAY_TIMER14_FREQUENCY,
			.factor = TURAG_HIGHRESDELAY_TIMER14_FREQUENCY / 1000000,
			.max_us = 65535 / (TURAG_HIGHRESDELAY_TIMER14_FREQUENCY / 1000000)
		}
	#endif
};
	

std::atomic<int> usedTimerIndex(0);



} // namespace


namespace TURAG {
	
extern "C" {
static void gpt_callback(GPTDriver *gptp) {
	const GPTConfigExt* config = reinterpret_cast<const GPTConfigExt*>(gptp->config);
	chBSemSignal(const_cast<BinarySemaphore*>(&config->sem));
}
}


HighResDelayTimer::HighResDelayTimer() {
	unsigned myTimer = usedTimerIndex.fetch_add(1);

	if (myTimer < length(availableTimers)) {
		data.driver = availableTimers[myTimer].driver;
		data.factor = availableTimers[myTimer].factor;
		data.max_us = availableTimers[myTimer].max_us;
		
		chBSemInit(&data.config.sem, 0);
		data.config.config.frequency = availableTimers[myTimer].frequency;
		data.config.config.callback = &gpt_callback;
		gptStart(data.driver, &(data.config.config));
	} else {
		data.driver = nullptr;
		
		turag_critical("No hardware timer available for high resolution delays. "
			"Falling back to systick based delays which may result in longer delays than necessary.");
	}
}

void HighResDelayTimer::wait(uint32_t us) {
	if (us == 0) {
		return;
	}
	
	if (data.driver) {
		if (us > data.max_us) {
			// if we can't produce the requested delay, fallback to systick delay
			Thread_delay(SystemTime::fromUsec(us));
		} else {
			uint16_t ticks = static_cast<uint16_t>(us) * data.factor;
			
			gptStartOneShot(data.driver, ticks);
			chBSemWait(&data.config.sem);
		}
	} else {
		// if no hardware timer is available, rely on 
		// systick to make the shortest delay possible.
		// Relies on fromUsec() to round upwards to the
		// next full systick.
		Thread_delay(SystemTime::fromUsec(us));
	}
}

HighResDelayTimer::~HighResDelayTimer() {
}

} // namespace TURAG
 
#endif // TURAG_USE_TINA_HIGH_RES_DELAY_TIMER

