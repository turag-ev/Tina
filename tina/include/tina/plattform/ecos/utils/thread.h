/**
 * \file thread.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2011,2012
 *
 * \brief implementation of thread, mutex and scoped_lock
 *
 */

#ifndef ECOS_THREAD_H_
#define ECOS_THREAD_H_

#include <cyg/kernel/kapi.h>

#include "utils/types.h"
#include "utils/time.h"
#include "utils/extra/macro-helper.h"
#include "utils/extra/normalize.h"
#include "utils/extra/scoped_lock.h"

namespace SystemControl {

////////////////////////////////////////////////////////////////////////////////
// Thread

/**
 * \class Thread
 * \brief Plattform independent thread
 *
 * \tparam size stack size of thread to create
 */
template<size_t size = 0>
class Thread {
  NOT_COPYABLE(Thread);
  
public:
  explicit Thread() { }

  /// create and start thread
  /**
   * \param priority priority of the thread
   * \param entry    function called in newly created thread
   */
  void start(cyg_addrword_t priority, void (*entry) (uint32_t)) {
    cyg_thread_create(31-priority, (cyg_thread_entry_t*) entry, 0, nullptr, (void *)stack_, sizeof(stack_), &thread_handle_, &thread_);
    cyg_thread_resume(thread_handle_);
  }

  /// returns maximal used stack size
  _always_inline unsigned getStackUsage() {
#ifdef CYGFUN_KERNEL_THREADS_STACK_MEASUREMENT
    return cyg_thread_measure_stack_usage(thread_handle_);
#else
    return 0;
#endif
  }

  /// lets the current thread sleeps for a time of ecos ticks
  static _always_inline void delay(cyg_tick_count_t ticks) {
    cyg_thread_delay(ticks);
  }

  static _always_inline void setName(const char *) { }

private:
  cyg_thread thread_;
  cyg_handle_t thread_handle_;
  char stack_[size] __attribute__((aligned(4)));
};

/// lets the current thread sleeps for a time of ecos ticks
_always_inline void Thread_delay(cyg_tick_count_t ticks) {
  cyg_thread_delay(ticks);
}

////////////////////////////////////////////////////////////////////////////////
// Mutex

/// Mutex
/**
 * NOTE: use ScopedLock to automatic unlock mutex.
 */
class Mutex {
  NOT_COPYABLE(Mutex);

public:
  typedef ScopedLock<Mutex> Lock;

  _always_inline explicit Mutex() {
    cyg_mutex_init(&mut_);
  }

  _always_inline ~Mutex() {
    cyg_mutex_destroy(&mut_);
  }

  _always_inline void lock() {
    cyg_mutex_lock(&mut_);
  }

  _always_inline bool tryLock() {
    return cyg_mutex_trylock(&mut_);
  }

  _always_inline void unlock() {
    cyg_mutex_unlock(&mut_);
  }

  typedef cyg_mutex_t* NativeHandle;
  NativeHandle getNativeHandle() {return &mut_;}

private:
  cyg_mutex_t mut_;
};

////////////////////////////////////////////////////////////////////////////////
// ConditionVariable

class ConditionVariable {
  NOT_COPYABLE(ConditionVariable);

public:
  typedef ScopedLock<Mutex> Lock;

  _always_inline ConditionVariable(Mutex* mutex) {
    cyg_cond_init(&cond_, mutex->getNativeHandle());
  }

  _always_inline ~ConditionVariable() {
    cyg_cond_destroy(&cond_);
  }

  _always_inline bool wait() {
    return cyg_cond_wait(&cond_);
  }

  _always_inline bool waitWithTimeout(Time timeout) {
    return cyg_cond_timed_wait(&cond_, get_current_tick() + timeout);
  }

  _always_inline void signal() {
    cyg_cond_signal(&cond_);
  }

  _always_inline void broadcast() {
    cyg_cond_broadcast(&cond_);
  }

private:
  cyg_cond_t cond_;
};

} // namespace SystemControl

#endif // ECOS_THREAD_H_
