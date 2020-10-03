#ifndef PLATFORM_CHIBIOS3_PUBLIC_TINA_THREAD_H
#define PLATFORM_CHIBIOS3_PUBLIC_TINA_THREAD_H

#include <ch.h>
#include <stddef.h>

#include <tina/tina.h>
//#include <os_turag.h>
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  thread_t* thread;
  void* stack;
  size_t stack_size;
} TuragThread;

#define TURAG_DEFINE_THREAD(name, stack_size) \
  static THD_WORKING_AREA(TURAG_CONCAT(name, _stack_), stack_size); \
  TuragThread name = {0, &TURAG_CONCAT(name, _stack_), sizeof(TURAG_CONCAT(name, _stack_))}

extern void turag_thread_entry(void* data);

static TURAG_ALWAYS_INLINE
void turag_thread_start(TuragThread* thread, tprio_t priority, void (*entry) (void)) {
  thread->thread = chThdCreateStatic(thread->stack, thread->stack_size,
                                     NORMALPRIO + priority,
                                     (tfunc_t)turag_thread_entry,
                                     (void*)entry);
}

/// returns maximal used stack size in bytes
#ifdef CH_DBG_FILL_THREADS
size_t turag_thread_get_stack_usage(const TuragThread* thread);
#else
# define turag_thread_get_stack_usage(x) (0)
#endif

/// lets the current thread sleeps for a time of chibios ticks
TURAG_INLINE
void turag_thread_delay(TuragSystemTime ticks) {
  chThdSleep(ticks.value);
}

/// lets the current thread sleeps until a certain tick
TURAG_INLINE
void turag_thread_sleep_until(TuragSystemTime ticks) {
  chThdSleepUntil(ticks.value);
}

/// sets the name of the current thread (only for chibios)
TURAG_INLINE
void turag_thread_set_name(const char* name) {
  chRegSetThreadName(name);
}

////////////////////////////////////////////////////////////////////////////////
// C Interface Mutex

#if CH_CFG_USE_MUTEXES
typedef mutex_t TuragMutex;

#define turag_mutex_init            chMtxObjectInit
#define turag_mutex_destroy(mutex)
#define turag_mutex_lock            chMtxLock
#define turag_mutex_try_lock        chMtxTryLock
#define turag_mutex_unlock          chMtxUnlock
#endif


////////////////////////////////////////////////////////////////////////////////
// C Interface Semaphore

#if CH_CFG_USE_SEMAPHORES
typedef semaphore_t TuragSemaphore;

static inline void turag_semaphore_init(TuragSemaphore* sem, int32_t n) {
	chSemObjectInit(sem, n);
}

static inline void turag_semaphore_wait(TuragSemaphore* sem) {
	chSemWait(sem);
}

static inline bool turag_semaphore_timed_wait(TuragSemaphore* sem, TuragSystemTime timeout) {
	while (1) {
		msg_t result = chSemWaitTimeout(sem, timeout.value);

		if (result == MSG_OK) {
			return true;
		} else if (result == MSG_TIMEOUT) {
			return false;
		}
	}
}

static inline bool turag_semaphore_try_wait(TuragSemaphore* sem) {
	return chSemWaitTimeout(sem, TIME_IMMEDIATE) == MSG_OK;
}

static inline void turag_semaphore_signal(TuragSemaphore* sem) {
	chSemSignal(sem);
}
#endif


////////////////////////////////////////////////////////////////////////////////
// C Interface Binary Semaphore

#if CH_CFG_USE_SEMAPHORES
typedef binary_semaphore_t TuragBinarySemaphore;

static inline void turag_binary_semaphore_init(TuragBinarySemaphore* sem, bool taken) {
	chBSemObjectInit(sem, taken);
}

static inline void turag_binary_semaphore_wait(TuragBinarySemaphore* sem) {
	chBSemWait(sem);
}

static inline bool turag_binary_semaphore_timed_wait(TuragBinarySemaphore* sem, TuragSystemTime timeout) {
	while (1) {
		msg_t result = chBSemWaitTimeout(sem, timeout.value);

		if (result == MSG_OK) {
			return true;
		} else if (result == MSG_TIMEOUT) {
			return false;
		}
	}
}

static inline bool turag_binary_semaphore_try_wait(TuragBinarySemaphore* sem) {
	return chBSemWaitTimeout(sem, TIME_IMMEDIATE) == MSG_OK;
}

static inline void turag_binary_semaphore_signal(TuragBinarySemaphore* sem) {
	chBSemSignal(sem);
}
#endif


////////////////////////////////////////////////////////////////////////////////
// C Interface Condition Variable

#if CH_CFG_USE_CONDVARS
typedef struct {
	condition_variable_t condvar;
	TuragMutex* mutex;
} TuragConditionVariable;

static inline void turag_condition_variable_init(TuragConditionVariable* cond, TuragMutex* mutex_) {
	cond->mutex = mutex_;
	chCondObjectInit(&cond->condvar);
}

static inline void turag_condition_variable_wait(TuragConditionVariable* cond) {
	chCondWait(&cond->condvar);
}

static inline bool turag_condition_variable_timed_wait(TuragConditionVariable* cond, TuragSystemTime timeout) {
	msg_t result = chCondWaitTimeout(&cond->condvar, timeout.value);
	if (result == MSG_TIMEOUT) turag_mutex_lock(cond->mutex);
    return result != MSG_TIMEOUT;
}

static inline void turag_condition_variable_signal(TuragConditionVariable* cond) {
	chCondSignal(&cond->condvar);
}

static inline void turag_condition_variable_broadcast(TuragConditionVariable* cond) {
	chCondBroadcast(&cond->condvar);
}
#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif // PLATFORM_CHIBIOS3_PUBLIC_TINA_THREAD_H
