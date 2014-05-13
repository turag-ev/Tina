#ifndef TINA_CHIBIOS_THREAD_H
#define TINA_CHIBIOS_THREAD_H

#include <ch.h>
#include <stddef.h>

#include <tina/tina.h>
#include <os_turag.h>
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  Thread* thread;
  void* stack;
  size_t stack_size;
} TuragThread;

#define TURAG_DEFINE_THREAD(name, stack_size) \
  static WORKING_AREA(TURAG_CONCAT(name, _stack_), stack_size); \
  TuragThread name = {0, &TURAG_CONCAT(name, _stack_), sizeof(TURAG_CONCAT(name, _stack_))}

#define TURAG_DEFINE_THREAD_CCM(name, stack_size) \
  static ATTRIB_SECTION_CCMSTACK WORKING_AREA(TURAG_CONCAT(name, _stack_), stack_size); \
  TuragThread name = {0, &TURAG_CONCAT(name, _stack_), sizeof(TURAG_CONCAT(name, _stack_))}

extern void _turag_thread_entry(void* data);

static _always_inline
void turag_thread_start(TuragThread* thread, tprio_t priority, void (*entry) (void)) {
  thread->thread = chThdCreateStatic(thread->stack, thread->stack_size,
                                     NORMALPRIO + priority,
                                     (tfunc_t)_turag_thread_entry,
                                     (void*)entry);
}

/// returns maximal used stack size in bytes
#ifdef CH_DBG_FILL_THREADS
size_t turag_thread_get_stack_usage(const TuragThread* thread);
#else
# define turag_thread_get_stack_usage(x) (0)
#endif

/// lets the current thread sleeps for a time of ecos ticks
TURAG_INLINE
void turag_thread_delay(TuragSystemTime ticks) {
  chThdSleep(ticks.value);
}

/// sets the name of the current thread (only for chibios)
TURAG_INLINE
void turag_thread_set_name(const char* name) {
  chRegSetThreadName(name);
}

////////////////////////////////////////////////////////////////////////////////
// C Interface Mutex

typedef Mutex TuragMutex;

#define turag_mutex_init            chMtxInit
#define turag_mutex_destroy(mutex)
#define turag_mutex_lock            chMtxLock
#define turag_mutex_try_lock        chMtxTryLock
#define turag_mutex_unlock(mutex)   chMtxUnlock()


////////////////////////////////////////////////////////////////////////////////
// C Interface Semaphore

typedef Semaphore TuragSemaphore;

static inline void turag_semaphore_init(TuragSemaphore* sem, int32_t n) {
	chSemInit(sem, n);
}

static inline void turag_semaphore_wait(TuragSemaphore* sem) {
	chSemWait(sem);
}

static inline bool turag_semaphore_timed_wait(TuragSemaphore* sem, TuragSystemTime timeout) {
	while (1) {
		msg_t result = chSemWaitTimeout(sem, timeout.value);

		if (result == RDY_OK) {
			return true;
		} else if (result == RDY_TIMEOUT) {
			return false;
		}
	}
}

static inline bool turag_semaphore_try_wait(TuragSemaphore* sem) {
	return chSemWaitTimeout(sem, TIME_IMMEDIATE) == RDY_OK;
}

static inline void turag_semaphore_signal(TuragSemaphore* sem) {
	chSemSignal(sem);
}



////////////////////////////////////////////////////////////////////////////////
// C Interface Binary Semaphore

typedef BinarySemaphore TuragBinarySemaphore;

static inline void turag_binary_semaphore_init(TuragBinarySemaphore* sem, bool taken) {
	chBSemInit(sem, taken);
}

static inline void turag_binary_semaphore_wait(TuragBinarySemaphore* sem) {
	chBSemWait(sem);
}

static inline bool turag_binary_semaphore_timed_wait(TuragBinarySemaphore* sem, TuragSystemTime timeout) {
	while (1) {
		msg_t result = chBSemWaitTimeout(sem, timeout.value);

		if (result == RDY_OK) {
			return true;
		} else if (result == RDY_TIMEOUT) {
			return false;
		}
	}
}

static inline bool turag_binary_semaphore_try_wait(TuragBinarySemaphore* sem) {
	return chBSemWaitTimeout(sem, TIME_IMMEDIATE) == RDY_OK;
}

static inline void turag_binary_semaphore_signal(TuragBinarySemaphore* sem) {
	chBSemSignal(sem);
}


////////////////////////////////////////////////////////////////////////////////
// C Interface Condition Variable
typedef struct {
	CondVar condvar;
	TuragMutex* mutex;
} TuragConditionVariable;

static inline void turag_condition_variable_init(TuragConditionVariable* cond, TuragMutex* mutex_) {
	cond->mutex = mutex_;
	chCondInit(&cond->condvar);
}

static inline void turag_condition_variable_wait(TuragConditionVariable* cond) {
	chCondWait(&cond->condvar);
}

static inline bool turag_condition_variable_timed_wait(TuragConditionVariable* cond, TuragSystemTime timeout) {
	msg_t result = chCondWaitTimeout(&cond->condvar, timeout.value);
	if (result == RDY_TIMEOUT) turag_mutex_lock(cond->mutex);
    return result != RDY_TIMEOUT;
}

static inline void turag_condition_variable_signal(TuragConditionVariable* cond) {
	chCondSignal(&cond->condvar);
}

static inline void turag_condition_variable_broadcast(TuragConditionVariable* cond) {
	chCondBroadcast(&cond->condvar);
}


#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_CHIBIOS_THREAD_H
