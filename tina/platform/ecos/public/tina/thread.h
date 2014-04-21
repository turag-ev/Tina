#ifndef TINA_ECOS_THREAD_H
#define TINA_ECOS_THREAD_H

#include <tina/tina.h>
#include "time.h"

#include <cyg/kernel/kapi.h>

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
  static char TURAG_CONCAT(name, _stack_) [stack_size] __attribute__((aligned(4))); \
  static cyg_thread TURAG_CONCAT(name, _thread_); \
  TuragThread name = {(char*)&TURAG_CONCAT(name, _stack_), stack_size, &TURAG_CONCAT(name, _thread_), 0};
  
  
extern void _turag_thread_entry(cyg_addrword_t data);
  
static _always_inline
void turag_thread_start(TuragThread* thread, cyg_addrword_t priority, void (*entry)(void)) {
  cyg_thread_create(31-priority, _turag_thread_entry,
                    (cyg_addrword_t)entry, NULL, (void *)thread->stack,
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


////////////////////////////////////////////////////////////////////////////////
// C Interface Semaphore

typedef cyg_sem_t TuragSemaphore;

static inline void turag_semaphore_init(TuragSemaphore* sem, int32_t n) {
	cyg_semaphore_init(sem, n);
}

static inline void turag_semaphore_wait(TuragSemaphore* sem) {
	cyg_semaphore_wait(sem); 
}

static inline bool turag_semaphore_timed_wait(TuragSemaphore* sem, TuragSystemTime timeout) {
	return cyg_semaphore_timed_wait(sem, cyg_current_time() + timeout.value);
}

static inline bool turag_semaphore_try_wait(TuragSemaphore* sem) {
	return cyg_semaphore_trywait(sem);
}

static inline void turag_semaphore_signal(TuragSemaphore* sem) {
	cyg_semaphore_post(sem);
}



////////////////////////////////////////////////////////////////////////////////
// C Interface Binary Semaphore

typedef cyg_binary_sem_t TuragBinarySemaphore;

static inline void turag_binary_semaphore_init(TuragBinarySemaphore* sem, bool taken) {
	cyg_binary_semaphore_init(sem, taken ? false : true);
}

static inline void turag_binary_semaphore_wait(TuragBinarySemaphore* sem) {
	cyg_binary_semaphore_wait(sem); 
}

static inline bool turag_binary_semaphore_timed_wait(TuragBinarySemaphore* sem, TuragSystemTime timeout) {
	return cyg_binary_semaphore_timed_wait(sem, cyg_current_time() + timeout.value);
}

static inline bool turag_binary_semaphore_try_wait(TuragBinarySemaphore* sem) {
	return cyg_binary_semaphore_trywait(sem);
}

static inline void turag_binary_semaphore_signal(TuragBinarySemaphore* sem) {
	cyg_binary_semaphore_post(sem);
}

////////////////////////////////////////////////////////////////////////////////
// C Interface Condition Variable

typedef cyg_cond_t TuragConditionVariable;

static inline void turag_condition_variable_init(TuragConditionVariable* condvar, TuragMutex* mutex) {
	cyg_cond_init(condvar, mutex);
}

static inline void turag_condition_variable_wait(TuragConditionVariable* condvar) {
	cyg_cond_wait(condvar);
}

static inline bool turag_condition_variable_timed_wait(TuragConditionVariable* condvar, TuragSystemTime timeout) {
	return cyg_cond_timed_wait(condvar, cyg_current_time() + timeout.value);
}

static inline void turag_condition_variable_signal(TuragConditionVariable* condvar) {
	cyg_cond_signal(condvar);
}

static inline void turag_condition_variable_broadcast(TuragConditionVariable* condvar) {
	cyg_cond_broadcast(condvar);
}


#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_ECOS_THREAD_H
