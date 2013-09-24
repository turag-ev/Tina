#ifndef CHIBIOS_CTHREAD_H
#define CHIBIOS_CTHREAD_H

#include <ch.h>
#include <stddef.h>

#include <tina/macros.h>
#include <tina/normalize.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// C Interface Thread

typedef struct {
  Thread* thread;
  void* stack;
  size_t stack_size;
} CThread;

#define DEFINE_THREAD(name, stack_size) \
  static WORKING_AREA(CONCAT(name, _stack_), stack_size); \
  CThread name = {0, &CONCAT(name, _stack_), sizeof(CONCAT(name, _stack_))}

msg_t cthread_entry(void* data);

_always_inline
void thread_start(CThread* thread, tprio_t priority, void (*entry) (uint32_t)) {
  thread->thread = chThdCreateStatic(thread->stack, thread->stack_size,
                                     NORMALPRIO + priority,
                                     cthread_entry,
                                     (void*)entry);
}

/// returns maximal used stack size in bytes
#ifdef CH_DBG_FILL_THREADS
size_t thread_get_stack_usage(const CThread* thread);
#else
# define thread_get_stack_usage(thread) (0)
#endif

/// lets the current thread sleeps for a time of ecos ticks
_always_inline
void thread_delay(systime_t ticks) {
  chThdSleep(ticks);
}

/// sets the name of the current thread (only for chibios)
_always_inline
void thread_set_name(const char* name) {
  chRegSetThreadName(name);
}
 
////////////////////////////////////////////////////////////////////////////////
// C Interface Mutex

typedef Mutex CMutex;

#define mutex_init            chMtxInit
#define mutex_destroy(mutex)
#define mutex_lock            chMtxLock
#define mutex_try_lock        chMtxTryLock
#define mutex_unlock(mutex)   chMtxUnlock()

#ifdef __cplusplus
}
#endif

#endif // CHIBIOS_CTHREAD_H
