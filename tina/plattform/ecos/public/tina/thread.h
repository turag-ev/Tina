#ifndef ECOS_CTHREAD_H
#define ECOS_CTHREAD_H

#include <cyg/kernel/kapi.h>

#include <tina/tina.h>
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char* stack;
  size_t stack_size;
  cyg_thread* thread;
  cyg_handle_t thread_handle;
} TuragThread;

#define TURAG_DEFINE_THREAD(name, stack_size) \
  static char CONCAT(name, _stack_) [stack_size] __attribute__((aligned(4))); \
  static cyg_thread CONCAT(name, _thread_); \
  TuragThread name = {(char*)&CONCAT(name, _stack_), stack_size, &CONCAT(name, _thread_), 0};

static _always_inline
void turag_thread_start(TuragThread* thread, cyg_addrword_t priority, void (*entry)(uint32_t)) {
  cyg_thread_create(31-priority, (void (*)(cyg_addrword_t))entry,
                    0, NULL, (void *)thread->stack,
                    thread->stack_size, &(thread->thread_handle), thread->thread);
  cyg_thread_resume(thread->thread_handle);
}

/// returns maximal used stack size in procent
static _always_inline
unsigned turag_thread_get_stack_usage(TuragThread* thread) {
#ifdef CYGFUN_KERNEL_THREADS_STACK_MEASUREMENT
  return 100 * cyg_thread_measure_stack_usage(thread->thread_handle) / thread->stack_size;
#else
  return 0;
#endif
}

/// lets the current thread sleeps for a time of ecos ticks
static _always_inline
void turag_thread_delay(TuragSystemTime ticks) {
  cyg_thread_delay(ticks.value);
}

/// sets the name of the current thread (only for chibios)
#define turag_thread_set_name(str)

////////////////////////////////////////////////////////////////////////////////
// C Interface Mutex

typedef cyg_mutex_t TuragMutex;

#define turag_mutex_init     cyg_mutex_init
#define turag_mutex_destroy  cyg_mutex_destroy
#define turag_mutex_lock     cyg_mutex_lock
#define turag_mutex_try_lock cyg_mutex_trylock
#define turag_mutex_unlock   cyg_mutex_unlock

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ECOS_CTHREAD_H
