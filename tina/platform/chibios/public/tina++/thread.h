#ifndef TINAPP_CHIBIOS_THREAD_H
#define TINAPP_CHIBIOS_THREAD_H

#include <ch.h>
#include <cstddef>
#include <os_turag.h>

#include <tina++/tina.h>
#include <tina++/time.h>
#include <tina++/helper/scoped_lock.h>

/// when definied, than getStackUsage will give real values. But thread creating time will increase.
#define THREADS_STACK_MEASUREMENT

#define TURAG_ATTR_SECTION_CCMDATA ATTR_SECTION_CCMDATA

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

extern "C" {
extern void _turag_thread_entry(void* data);
}

std::size_t thread_get_stack_usage(const char* stack, std::size_t stack_size);

template<size_t size>
class thread_detail {
  NOT_COPYABLE(thread_detail);

public:
  // returns size of stack
  static constexpr std::size_t StackSize = size;

  _always_inline thread_detail() :
    thread_(nullptr)
  { }

  _always_inline
  void start(tprio_t priority, void (*entry) ()) {
    thread_ = chThdCreateStatic(working_area_, sizeof(working_area_),
                                NORMALPRIO + priority,
                                (tfunc_t)_turag_thread_entry,
                                (void*)entry);
  }

  /// returns maximal used stack size in bytes
  _always_inline std::size_t getStackUsage() const {
#ifdef CH_DBG_FILL_THREADS
    return thread_get_stack_usage(reinterpret_cast<const char*>(&working_area_), sizeof(working_area_));
#else
    return 0;
#endif
  }

  _always_inline
  bool shouldTerminate() {
    return false;
  }

private:
  Thread *thread_;
  WORKING_AREA(working_area_, size);
};

} // namespace detail

struct CurrentThread {
private:
  // von dieser Klasse gibt es keine Instanz
  CurrentThread();

public:
  /// lets the current thread sleeps for a time of ecos ticks
  static _always_inline void delay(SystemTime ticks) {
    chThdSleep(ticks.toTicks());
  }

  static _always_inline void setName(const char *name) {
    chRegSetThreadName(name);
  }

  static _always_inline void delayUntil(SystemTime time) {
      chThdSleepUntil(time.toTicks());
  }
};

/// lets the current thread sleeps for a time of ecos ticks
_always_inline void Thread_delay(SystemTime ticks) {
  chThdSleep(ticks.toTicks());
}

////////////////////////////////////////////////////////////////////////////////
// ConditionVariable

namespace detail {

template<class TMutex>
class ConditionVariable_detail {
  NOT_COPYABLE(ConditionVariable_detail);

public:
  typedef TMutex Mutex;

	class Lock : public ScopedLock<TMutex> {
	public:
		  Lock(ConditionVariable_detail& condvar) :
		      ScopedLock<TMutex>(condvar.getMutex()),
		      condvar_(condvar)
		  { }

		  _always_inline bool wait() {
		      return condvar_.wait();
		  }

#ifdef CH_USE_CONDVARS_TIMEOUT

		  _always_inline bool waitFor(SystemTime timeout) {
              return condvar_.waitFor(timeout);
		  }

		  _always_inline bool waitUntil(SystemTime timeout) {
              return condvar_.waitUntil(timeout);
		  }

#endif

	private:
		  ConditionVariable_detail& condvar_;
	};

public:
  constexpr _always_inline
  ConditionVariable_detail(TMutex* mutex) :
    cond_(_CONDVAR_DATA(cond_)), mutex_(mutex)
  { }

  _always_inline bool wait() {
    return chCondWait(&cond_) != 0;
  }

#ifdef CH_USE_CONDVARS_TIMEOUT

// as opposed to the posix pthread_cond_timedwait-function the chibi function "chCondWaitTimeout"
// does not reaquire the associated mutex when returning due to timeout thus requiring us to do
// this manually

  bool waitFor(SystemTime timeout) {
	  msg_t result = chCondWaitTimeout(&cond_, timeout.toTicks());
	  if (result == RDY_TIMEOUT) mutex_->lock();
    return result != RDY_TIMEOUT;
  }

  bool waitUntil(SystemTime timeout) {
	  msg_t result = chCondWaitTimeout(&cond_, timeout.toTicks() - chTimeNow());
	  if (result == RDY_TIMEOUT) mutex_->lock();
	  return result != RDY_TIMEOUT;
  }

#endif

  _always_inline void signal() {
    chCondSignal(&cond_);
  }

  _always_inline void broadcast() {
    chCondBroadcast(&cond_);
  }

  TMutex& getMutex() {
    return *mutex_;
  }

private:
  CondVar cond_;
  TMutex* mutex_;
};

} // namespace detail


////////////////////////////////////////////////////////////////////////////////
// Mailbox
namespace detail {

#ifdef CH_USE_MAILBOXES

template<std::size_t size>
class mailbox_detail {

private:
    Mailbox mailbox_;
    msg_t pointer_buffer_[size];

public:
    explicit mailbox_detail() :
        mailbox_(_MAILBOX_DATA(mailbox_, pointer_buffer_, size)) { }

    bool post(void* msg, SystemTime time) {
        if (chMBPost(&mailbox_, static_cast<msg_t>(msg), time.value) == RDY_OK) {
            return true;
        } else {
            return false;
        }
    }
    bool post(void* msg) { return post(msg, TIME_INFINITE); }

    bool postAhead(void* msg, SystemTime time) {
        if (chMBPostAhead(&mailbox_, static_cast<msg_t>(msg), time.value) == RDY_OK) {
            return true;
        } else {
            return false;
        }
    }
    bool postAhead(void* msg) { return postAhead(msg, TIME_INFINITE); }

    void* fetch(SystemTime time) {
        msg_t msg;

        if (chMBFetch(&mailbox_, &msg, time.value) == RDY_OK) {
            return static_cast<void*>(msg);
        } else {
            return nullptr;
        }
    }
    void* fetch(void) { return fetch(TIME_INFINITE); }

};

#endif // CH_USE_MAILBOXES

} // namespace detail


////////////////////////////////////////////////////////////////////////////////
// Mutex
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
  // not for end-user; use ScopedLock<Mutex> or Mutex::Lock!
  friend class ScopedLock<mutex_detail>;
  friend class ConditionVariable_detail<mutex_detail>;

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


////////////////////////////////////////////////////////////////////////////////
// Semaphore

namespace detail {

class semaphore_detail {
  NOT_COPYABLE(semaphore_detail);

private:
  Semaphore sem_;

public:
  explicit semaphore_detail(int count = 0) :
      sem_(_SEMAPHORE_DATA(sem_, count)) { }

  void wait(void) { while (chSemWait(&sem_) == RDY_RESET); }
  bool wait(SystemTime time);
  void signal(void) { chSemSignal(&sem_); }
};

} // namespace detail


////////////////////////////////////////////////////////////////////////////////
// BinarySemaphore

namespace detail {

class binary_semaphore_detail {
  NOT_COPYABLE(binary_semaphore_detail);

private:
  BinarySemaphore sem_;

public:
  explicit binary_semaphore_detail(bool taken = true) :
      sem_(_BSEMAPHORE_DATA(sem_, taken)) { }

  void wait(void) { while (chBSemWait(&sem_) == RDY_RESET); }
  bool wait(SystemTime time);
  void signal(void) { chBSemSignal(&sem_); }
};

} // namespace detail


// work-around because chibios mutex structure is also named Mutex
typedef detail::mutex_detail Mutex;

// work-around because chibios semaphore structure is also named Semaphore
typedef detail::semaphore_detail Semaphore;

// work-around because chibios binary semaphore structure is also named BinarySemaphore
//typedef detail::binary_semaphore_detail BinarySemaphore;

// work-around because chibios mailbox structure is also named Mailbox
template<std::size_t size> class Mailbox : public detail::mailbox_detail<size> { };

// work-around because chibios thread structure is also named Thread
// and WORKING_AREA have problems with this.
template<size_t size = 0> class Thread : public detail::thread_detail<size> { };

// typedef to get rid of template argument
typedef detail::ConditionVariable_detail<Mutex> ConditionVariable;

} // namespace TURAG

#endif // TINAPP_CHIBIOS_THREAD_H
