#ifndef TINAPP_CHIBIOS_THREAD_H
#define TINAPP_CHIBIOS_THREAD_H

#include <ch.h>
#include <cstddef>
//#include <os_turag.h>

#include <tina++/tina.h>
#include <tina++/time.h>
#include <tina++/helper/scoped_lock.h>

#include <atomic>


namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// Thread
extern "C" void _turag_thread_entry(void* data);


class ThreadImpl
{
    NOT_COPYABLE(ThreadImpl);

protected:
    // should only be created through Thread
    explicit ThreadImpl(void* working_area, size_t size) :
        working_area_(working_area), stack_size_(size), 
        thread_(nullptr), terminate_request_(false) { }

public:
    ~ThreadImpl() {}

    void start(int priority, void (*entry) ()) {
        thread_ = chThdCreateStatic(working_area_, stack_size_,
                                NORMALPRIO + priority,
                                (tfunc_t)_turag_thread_entry,
                                (void*)entry);
    }

    void terminate() {
      terminate_request_ = true;
    }

    bool shouldTerminate() const {
      return terminate_request_;
    }

    /// returns maximal used stack size in bytes
    std::size_t getStackUsage() const;
    
private:
    void* working_area_;
    size_t stack_size_;
    
    thread_t *thread_;

    std::atomic<bool> terminate_request_;
};    
    
    
    
/**
 * \class Thread
 * \brief Plattform independent thread
 *
 * \tparam size stack size of thread to create
 */
template<size_t size>
class Thread : public ThreadImpl {
    NOT_COPYABLE(Thread);

public:
    explicit Thread() :
        ThreadImpl(working_area_, size) { }

    // returns size of stack
    static constexpr std::size_t StackSize = size;

private:
    THD_WORKING_AREA(working_area_, size);
};


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
// Mutex
/**
 * NOTE: use ScopedLock to automatic unlock mutex.
 */

class Mutex {
  NOT_COPYABLE(Mutex);

public:
  typedef ScopedLock<Mutex> Lock;

  constexpr _always_inline
  Mutex() :
    mut_(_MUTEX_DATA(mut_))
  { }

  typedef mutex_t* NativeHandle;
  NativeHandle getNativeHandle() {return &mut_;}

protected:
  // not for end-user; use ScopedLock<Mutex> or Mutex::Lock!
  friend class ScopedLock<Mutex>;
  friend class ConditionVariable;

  _always_inline void lock() {
    chMtxLock(&mut_);
  }

  _always_inline bool tryLock() {
    return chMtxTryLock(&mut_) == TRUE;
  }

  _always_inline void unlock() {
    chMtxUnlock(&mut_);
  }

private:
  mutex_t mut_;
};


////////////////////////////////////////////////////////////////////////////////
// ConditionVariable

class ConditionVariable {
  NOT_COPYABLE(ConditionVariable);

public:
	class Lock : public ScopedLock<Mutex> {
	public:
		  Lock(ConditionVariable& condvar) :
		      ScopedLock<Mutex>(condvar.getMutex()),
		      condvar_(condvar)
		  { }

		  _always_inline bool wait() {
		      return condvar_.wait();
		  }

#if defined(CH_USE_CONDVARS_TIMEOUT) || defined(CH_CFG_USE_CONDVARS_TIMEOUT)

		  _always_inline bool waitFor(SystemTime timeout) {
              return condvar_.waitFor(timeout);
		  }

		  _always_inline bool waitUntil(SystemTime timeout) {
              return condvar_.waitUntil(timeout);
		  }

#endif

	private:
		  ConditionVariable& condvar_;
	};

public:
  constexpr _always_inline
  ConditionVariable(Mutex* mutex) :
    cond_(_CONDVAR_DATA(cond_)), mutex_(mutex)
  { }

  _always_inline bool wait() {
    return chCondWait(&cond_) != 0;
  }

#if defined(CH_USE_CONDVARS_TIMEOUT) || defined(CH_CFG_USE_CONDVARS_TIMEOUT)

// as opposed to the posix pthread_cond_timedwait-function the chibi function "chCondWaitTimeout"
// does not reaquire the associated mutex when returning due to timeout thus requiring us to do
// this manually

  bool waitFor(SystemTime timeout) {
	  msg_t result = chCondWaitTimeout(&cond_, timeout.toTicks());
	  if (result == MSG_TIMEOUT) mutex_->lock();
    return result != MSG_TIMEOUT;
  }

  bool waitUntil(SystemTime timeout) {
      msg_t result = chCondWaitTimeout(&cond_, (timeout - SystemTime::now()).toTicks());
	  if (result == MSG_TIMEOUT) mutex_->lock();
	  return result != MSG_TIMEOUT;
  }

#endif

  _always_inline void signal() {
    chCondSignal(&cond_);
  }

  _always_inline void broadcast() {
    chCondBroadcast(&cond_);
  }

  Mutex& getMutex() {
    return *mutex_;
  }

private:
   condition_variable_t cond_;
  Mutex* mutex_;
};



////////////////////////////////////////////////////////////////////////////////
// Mailbox
#ifdef CH_USE_MAILBOXES

template<std::size_t size>
class Mailbox {

private:
    mailbox_t mailbox_;
    msg_t pointer_buffer_[size];

public:
    explicit Mailbox() :
        mailbox_(_MAILBOX_DATA(mailbox_, pointer_buffer_, size)) { }

    bool post(void* msg, SystemTime time) {
        if (chMBPost(&mailbox_, static_cast<msg_t>(msg), time.value) == MSG_OK) {
            return true;
        } else {
            return false;
        }
    }
    bool post(void* msg) { return post(msg, TIME_INFINITE); }

    bool postAhead(void* msg, SystemTime time) {
        if (chMBPostAhead(&mailbox_, static_cast<msg_t>(msg), time.value) == MSG_OK) {
            return true;
        } else {
            return false;
        }
    }
    bool postAhead(void* msg) { return postAhead(msg, TIME_INFINITE); }

    void* fetch(SystemTime time) {
        msg_t msg;

        if (chMBFetch(&mailbox_, &msg, time.value) == MSG_OK) {
            return static_cast<void*>(msg);
        } else {
            return nullptr;
        }
    }
    void* fetch(void) { return fetch(TIME_INFINITE); }

};

#endif // CH_USE_MAILBOXES



////////////////////////////////////////////////////////////////////////////////
// Semaphore


class Semaphore {
  NOT_COPYABLE(Semaphore);

private:
  semaphore_t sem_;

public:
  explicit Semaphore(int count = 0) :
      sem_(_SEMAPHORE_DATA(sem_, count)) { }

  void wait(void) { while (chSemWait(&sem_) == MSG_RESET); }
  bool wait(SystemTime time);
  void signal(void) { chSemSignal(&sem_); }
};



////////////////////////////////////////////////////////////////////////////////
// BinarySemaphore

class BinarySemaphore {
  NOT_COPYABLE(BinarySemaphore);

private:
  binary_semaphore_t sem_;

public:
  explicit BinarySemaphore(bool taken = true) :
      sem_(_BSEMAPHORE_DATA(sem_, taken)) { }

  void wait(void) { while (chBSemWait(&sem_) == MSG_RESET); }
  bool wait(SystemTime time);
  void signal(void) { chBSemSignal(&sem_); }
};



} // namespace TURAG

#endif // TINAPP_CHIBIOS_THREAD_H
