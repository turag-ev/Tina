/**
 * \file thread.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2011,2012
 *
 * \brief implementation of thread, mutex and scoped_lock
 *
 */

#ifndef SIMULATION_THREAD_H_
#define SIMULATION_THREAD_H_

#include <pthread.h>

#include <utils/types.h>
#include <utils/extra/macro-helper.h>
#include <utils/extra/normalize.h>
#include <utils/extra/scoped_lock.h>
#include <cstddef>

#include "utils/time.h"

/// when definied, than getStackUsage will give real values. But thread creating time will increase.
//#define THREADS_STACK_MEASUREMENT

namespace SystemControl {

////////////////////////////////////////////////////////////////////////////////
// Thread

void Thread_delay(SystemTime d /* [ticks] */);
void Thread_pause_all();
void Thread_resume_all();

namespace detail {
  class ThreadDetail {
  public:
    _always_inline
    void join() {
      pthread_join(thread_, nullptr);
    }

    static void setName(const char *) { }
    static void delay(SystemTime d) { Thread_delay(d); }

    static pthread_t createPThread(char* stack, std::size_t stack_size, int priority, void (*entry) (uint32_t));
    static std::size_t getStackUsage(const char* stack, std::size_t stack_size);
  
  protected:
    _always_inline
    void create(char* stack, std::size_t stack_size, int priority, void (*entry) (uint32_t)) {
      thread_ = createPThread(stack, stack_size, priority, entry);
    }
    
  private:
    pthread_t thread_;
 
    static void* thread_entry(void* data);
  };
}

template<size_t size = 0>
class Thread : public detail::ThreadDetail {
  NOT_COPYABLE(Thread);

public:
  explicit Thread() :
    stack_()
  { }

  _always_inline
  void start(int priority, void (*entry) (uint32_t)) {
    create(stack_, size, priority, entry);
  }

  // get max bytes used in stack (may more than really used)
  _always_inline
  std::size_t getStackUsage() {
    return ThreadDetail::getStackUsage(stack_, size);
  }

private:
  char stack_[size];
};

////////////////////////////////////////////////////////////////////////////////
// Mutex

/*
 * reference mutex implementation on POSIX
 * from http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2406.html
 */
class Mutex {
  NOT_COPYABLE(Mutex);

public:
  typedef ScopedLock<Mutex> Lock;

  explicit constexpr
  Mutex() :
    mut_(PTHREAD_MUTEX_INITIALIZER)
  { }

  _always_inline ~Mutex() {
    pthread_mutex_destroy(&mut_);
  }

  typedef pthread_mutex_t* NativeHandle;
  NativeHandle getNativeHandle() {return &mut_;}

protected:
  // not for end-user use Mutex::Lock!
  friend class ScopedLock<Mutex>;

  _always_inline void lock() {
    pthread_mutex_lock(&mut_);
  }

  _always_inline bool tryLock() {
    return pthread_mutex_trylock(&mut_) == 0;
  }

  _always_inline void unlock() {
    pthread_mutex_unlock(&mut_);
  }

private:
  pthread_mutex_t mut_;
};

////////////////////////////////////////////////////////////////////////////////
// ConditionVariable

class ConditionVariable {
  NOT_COPYABLE(ConditionVariable);

public:
  _always_inline ConditionVariable(Mutex* mutex) :
    cond_(),
    mutptr_(mutex->getNativeHandle())
  {
    pthread_cond_init(&cond_, NULL);
  }

  _always_inline ~ConditionVariable() {
    pthread_cond_destroy(&cond_);
  }

  _always_inline bool wait() {
    return pthread_cond_wait(&cond_, mutptr_);
  }

  bool waitWithTimeout(SystemTime timeout) {
#ifndef _WIN32
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += ticks_to_us(timeout) * 1000;
    ts.tv_sec += ticks_to_s(timeout);
    return pthread_cond_timedwait(&cond_, mutptr_, &ts);
#else
    UNUSED(timeout);
    return pthread_cond_wait(&cond_, mutptr_);
#endif
  }

  _always_inline void signal() {
    pthread_cond_signal(&cond_);
  }

  _always_inline void broadcast() {
    pthread_cond_broadcast(&cond_);
  }

private:
  pthread_cond_t cond_;
  pthread_mutex_t* mutptr_;
};

} // namespace SystemControl

#endif // SIMULATION_THREAD_H_
