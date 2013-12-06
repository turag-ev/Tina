#ifndef TINA_SIM_THREAD_H
#define TINA_SIM_THREAD_H

#include <pthread.h>
#include <unistd.h>
#include <limits.h>

#include <tina/tina.h>
#include <tina/timetype.h>
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// C Interface Thread

typedef struct {
  pthread_t thread;
  char* const stack;
  size_t stack_size;
} TuragThread;

#define TURAG_DEFINE_THREAD(name, stack_size) \
  static char CONCAT(name, _stack_) [stack_size];\
  TuragThread name = {0, CONCAT(name, _stack_), stack_size}
  
void turag_thread_start(TuragThread* thread, int priority, void (*entry) (void));

/// returns maximal used stack size in bytes
size_t turag_thread_get_stack_usage(const TuragThread* thread);

/// lets the current thread sleeps for a time of ecos ticks
void turag_thread_delay(TuragSystemTime d);

/// sets the name of the current thread (only for chibios)
#define turag_thread_set_name(str)

////////////////////////////////////////////////////////////////////////////////
// C Interface Mutex

typedef pthread_mutex_t TuragMutex;

static _always_inline void
turag_mutex_init(TuragMutex* mutex) {
  pthread_mutex_init(mutex, NULL);
}

static _always_inline bool
turag_mutex_try_lock(TuragMutex* mutex) {
  return pthread_mutex_trylock(mutex) == 0;
}

#define turag_mutex_destroy  pthread_mutex_destroy
#define turag_mutex_lock     pthread_mutex_lock
#define turag_mutex_unlock   pthread_mutex_unlock

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_SIM_THREAD_H
