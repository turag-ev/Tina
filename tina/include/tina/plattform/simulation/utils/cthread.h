#ifndef SIMULATION_CTHREAD_H
#define SIMULATION_CTHREAD_H

#include <pthread.h>
#include <unistd.h>
#include <limits.h>

#include <util/types.h>
#include "utils/extra/macro-helper.h"
#include "utils/extra/normalize.h"
#include "utils/timetype.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// C Interface Thread

typedef struct {
  pthread_t thread;
  char* const stack;
  size_t stack_size;
} CThread;

#define DEFINE_THREAD(name, stack_size) \
  static char CONCAT(name, _stack_) [stack_size];\
  CThread name = {0, CONCAT(name, _stack_), stack_size}

void thread_start(CThread* thread, int priority, void (*entry) (uint32_t));

/// returns maximal used stack size in bytes
size_t thread_get_stack_usage(const CThread* thread);

/// lets the current thread sleeps for a time of ecos ticks
void thread_delay(SystemTicks d);

/// sets the name of the current thread (only for chibios)
#define thread_set_name(str)

////////////////////////////////////////////////////////////////////////////////
// C Interface Mutex

typedef pthread_mutex_t CMutex;

_always_inline void
mutex_init(CMutex* mutex) {
  pthread_mutex_init(mutex, NULL);
}

_always_inline bool
mutex_try_lock(CMutex* mutex) {
  return pthread_mutex_trylock(mutex) == 0;
}

#define mutex_destroy  pthread_mutex_destroy
#define mutex_lock     pthread_mutex_lock
#define mutex_unlock   pthread_mutex_unlock

#ifdef __cplusplus
}
#endif

#endif // SIMULATION_CTHREAD_H
