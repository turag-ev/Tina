#ifndef TINAPP_CHIBIOS_THREAD_H
#define TINAPP_CHIBIOS_THREAD_H

#include <ch.h>
#include <cstddef>

#include <tina++/tina.h>
#include <tina++/time.h>
#include <tina++/helper/scoped_lock.h>

/// when definied, than getStackUsage will give real values. But thread creating time will increase.
#define THREADS_STACK_MEASUREMENT

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// Thread

/**
 * \class Thread
 * \brief Plattform independent thread
 *
 * \tparam size stack size of thread to create
 */
namespace detail {

msg_t thread_entry(void* data);
std::size_t thread_get_stack_usage(const char* stack, std::size_t stack_size);

template<size_t size>
class thread_detail {
  NOT_COPYABLE(thread_detail);

public:
  _always_inline thread_detail() :
    thread_(nullptr)
  { }

  _always_inline
  void start(tprio_t priority, void (*entry) (uint32_t)) {
    thread_ = chThdCreateStatic(working_area_, sizeof(working_area_),
                                NORMALPRIO + priority,
                                thread_entry,
                                (void*)entry);
  }

  /// returns maximal used stack size in bytes
  _always_inline unsigned getStackUsage() const {
#ifdef CH_DBG_FILL_THREADS
    return thread_get_stack_usage(reinterpret_cast<const char*>(&working_area_), sizeof(working_area_));
#else
    return 0;
#endif
  }

  // returns size of stack 
  static constexpr unsigned getStackSize() { return size; }

  /// lets the current thread sleeps for a time of ecos ticks
  static _always_inline void delay(SystemTime ticks) {
    chThdSleep(ticks.value);
  }

  static _always_inline void setName(const char *name) {
    chRegSetThreadName(name);
  }

private:
  Thread *thread_;
  WORKING_AREA(working_area_, size);
};

} // namespace detail

/// lets the current thread sleeps for a time of ecos ticks
_always_inline void Thread_delay(SystemTime ticks) {
  chThdSleep(ticks.value);
}

////////////////////////////////////////////////////////////////////////////////
// Mutex

/// Mutex
/**
 * NOTE: use ScopedLock to automatic unlock mutex.
 */

namespace detail {

class mutex_detail {
  NOT_COPYABLE(mutex_detail);

public:
  typedef ScopedLock<mutex_detail> Lock;

  constexpr _always_inline
  mutex_detail() :
    mut_(_MUTEX_DATA(mut_))
  { }
  
  typedef Mutex* NativeHandle;
  NativeHandle getNativeHandle() {return &mut_;}
  
protected:
  // not for end-user use ScopedLock<Mutex> or Mutex::Lock!
  friend class ScopedLock<mutex_detail>;

  _always_inline void lock() {
    chMtxLock(&mut_);
  }

  _always_inline bool tryLock() {
    return chMtxTryLock(&mut_) == TRUE;
  }

  _always_inline void unlock() {
    chMtxUnlock();
  }

private:
  Mutex mut_;
};

} // namespace detail

// work-around because chibios mutex structure is also named Mutex
typedef detail::mutex_detail Mutex;

////////////////////////////////////////////////////////////////////////////////
// ConditionVariable

class ConditionVariable {
  NOT_COPYABLE(ConditionVariable);

public:
  constexpr _always_inline
  ConditionVariable() :
    cond_(_CONDVAR_DATA(cond_))
  { }

  _always_inline bool wait() {
    return chCondWait(&cond_) != 0;
  }

#ifdef CH_USE_CONDVARS_TIMEOUT
  _always_inline bool waitWithTimeout(SystemTime timeout) {
    return chCondWaitTimeout(&cond_, timeout.value) != RDY_TIMEOUT;
  }
#endif

  _always_inline void signal() {
    chCondSignal(&cond_);
  }

  _always_inline void broadcast() {
    chCondBroadcast(&cond_);
  }

private:
  CondVar cond_;
};

// work-around because chibios thread structure is also named Thread
// and WORKING_AREA have problems with this.
template<size_t size = 0> class Thread : public detail::thread_detail<size> { };

} // namespace TURAG

#endif // TINAPP_CHIBIOS_THREAD_H
