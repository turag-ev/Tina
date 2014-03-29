#ifndef TINA_CHIBIOS_THREAD_H
#define TINA_CHIBIOS_THREAD_H

#include <ch.h>
#include <stddef.h>

#include <tina/tina.h>
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
  static WORKING_AREA(CONCAT(name, _stack_), stack_size); \
  TuragThread name = {0, &CONCAT(name, _stack_), sizeof(CONCAT(name, _stack_))}
  
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
_always_inline
void turag_thread_delay(TuragSystemTime ticks) {
  chThdSleep(ticks.value);
}

/// sets the name of the current thread (only for chibios)
_always_inline
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
	return chSemWaitTimeout(sem, timeout.value) == RDY_OK;
}

static inline bool turag_semaphore_try_wait(TuragSemaphore* sem) {
	return chSemWaitTimeout(sem, TIME_IMMEDIATE) == RDY_OK;
}

static inline void turag_semaphore_post(TuragSemaphore* sem) {
	chSemSignal(sem);
}




#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_CHIBIOS_THREAD_H
