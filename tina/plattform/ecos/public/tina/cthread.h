#ifndef ECOS_CTHREAD_H
#define ECOS_CTHREAD_H

#include <cyg/kernel/kapi.h>
#include <util/types.h>

#include <tina/tina.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// C Interface Thread

typedef struct {
  char* stack;
  size_t stack_size;
  cyg_thread* thread;
  cyg_handle_t thread_handle;
} CThread;

#define DEFINE_THREAD(name, stack_size) \
  static char CONCAT(name, _stack_) [stack_size] __attribute__((aligned(4))); \
  static cyg_thread CONCAT(name, _thread_); \
  CThread name = {(char*)&CONCAT(name, _stack_), stack_size, &CONCAT(name, _thread_), 0};

_always_inline void
thread_start(CThread* thread, cyg_addrword_t priority, void (*entry)(uint32_t)) {
  cyg_thread_create(31-priority, (void (*)(cyg_addrword_t))entry,
                    0, NULL, (void *)thread->stack,
                    thread->stack_size, &(thread->thread_handle), thread->thread);
  cyg_thread_resume(thread->thread_handle);
}

/// returns maximal used stack size in procent
_always_inline unsigned
thread_get_stack_usage(CThread* thread) {
#if defined(CYGFUN_KERNEL_THREADS_STACK_MEASUREMENT) && !defined(SIMULATION)
  return 100 * cyg_thread_measure_stack_usage(thread->thread_handle) / thread->stack_size;
#else
  return 0;
#endif
}

/// lets the current thread sleeps for a time of ecos ticks
_always_inline void
thread_delay(cyg_tick_count_t ticks) {
  cyg_thread_delay(ticks);
}

/// sets the name of the current thread (only for chibios)
#define thread_set_name(str)

////////////////////////////////////////////////////////////////////////////////
// C Interface Mutex

typedef cyg_mutex_t CMutex;

#define mutex_init     cyg_mutex_init
#define mutex_destroy  cyg_mutex_destroy
#define mutex_lock     cyg_mutex_lock
#define mutex_try_lock cyg_mutex_trylock
#define mutex_unlock   cyg_mutex_unlock

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ECOS_CTHREAD_H
