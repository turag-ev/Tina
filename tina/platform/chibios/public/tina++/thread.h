#ifndef PLATFORM_CHIBIOS3_PUBLIC_TINAPP_THREAD_H
#define PLATFORM_CHIBIOS3_PUBLIC_TINAPP_THREAD_H

#include <ch.h>

#include <cstddef>
#include <atomic>

#include <tina++/tina.h>
#include <tina++/time.h>
#include <tina++/helper/scoped_lock.h>

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// Thread

class Runnable {
public:
    virtual void run() = 0;
};

class ThreadImpl
{
    ThreadImpl(const ThreadImpl&) = delete;
    ThreadImpl& operator=(const ThreadImpl&) = delete;
    ThreadImpl(ThreadImpl&&) = delete;
    ThreadImpl& operator=(ThreadImpl&&) = delete;

protected:
    // should only be created through Thread
    explicit ThreadImpl(void* working_area, size_t size) :
        working_area_(working_area), stack_size_(size), 
        thread_(nullptr), terminate_request_(false) { }

public:
    using EntryFunction = void(*)();

    ///
    /// \brief Start thread using a C style function
    /// \param priority Thread priority
    /// \param entry C style function pointer
    ///
    void start(int priority, EntryFunction entry);

    ///
    /// \brief Start thread using a C++ object
    /// \param priority Thread priority
    /// \param runnable Pointer to object implementing the Runnable interface
    ///
    void start(int priority, Runnable* runnable);

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
public:
    explicit Thread() :
        ThreadImpl(working_area_, size) { }

    // returns size of stack
    static constexpr std::size_t StackSize = size;

private:
    THD_WORKING_AREA(working_area_, size);
};

/// returns usage of main and process stacks on ARM Cortex-M
std::size_t getMainStackUsage();
std::size_t getProcessStackUsage();

struct CurrentThread {
private:
  // von dieser Klasse gibt es keine Instanz
  CurrentThread();

public:
  /// lets the current thread sleep for a specified time
  static TURAG_ALWAYS_INLINE void delay(SystemTime time) {
    chThdSleep(time.toTicks());
  }

  static TURAG_ALWAYS_INLINE void setName(const char *name) {
    chRegSetThreadName(name);
  }

  static TURAG_ALWAYS_INLINE void delayUntil(SystemTime time) {
      chThdSleepUntil(time.toTicks());
  }
};


////////////////////////////////////////////////////////////////////////////////
// Mutex
#if CH_CFG_USE_MUTEXES
/**
 * NOTE: use ScopedLock to automatic unlock mutex.
 */
class Mutex {
  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;

public:
  typedef ScopedLock<Mutex> Lock;

  constexpr TURAG_ALWAYS_INLINE
  Mutex() :
    mut_(_MUTEX_DATA(mut_))
  { }

  typedef mutex_t* NativeHandle;
  NativeHandle getNativeHandle() {return &mut_;}

protected:
  // not for end-user; use ScopedLock<Mutex> or Mutex::Lock!
  friend class ScopedLock<Mutex>;
  friend class ConditionVariable;

  // "lock", "tryLock" and "unlock" check for chThdGetSelfX() != 0,
  // to avoid triggering assertions when being run from static initializer code,
  // because in that case, there won't be a thread to assign the mutex to.
  // The proper solution would be to not use mutexes from initialization code,
  // but ¯\_(ツ)_/¯
  TURAG_ALWAYS_INLINE void lock() {
    if(chThdGetSelfX())
      chMtxLock(&mut_);
  }

  TURAG_ALWAYS_INLINE bool tryLock() {
    if(chThdGetSelfX())
      return chMtxTryLock(&mut_);
    else
      return true;
  }

  TURAG_ALWAYS_INLINE void unlock() {
    if(chThdGetSelfX())
      chMtxUnlock(&mut_);
  }

private:
  mutex_t mut_;
};
#endif


////////////////////////////////////////////////////////////////////////////////
// ConditionVariable
#if CH_CFG_USE_CONDVARS
class ConditionVariable {
  ConditionVariable(const ConditionVariable&) = delete;
  ConditionVariable& operator=(const ConditionVariable&) = delete;

public:
	class Lock : public ScopedLock<Mutex> {
	public:
		  Lock(ConditionVariable& condvar) :
		      ScopedLock<Mutex>(condvar.getMutex()),
		      condvar_(condvar)
		  { }

		  TURAG_ALWAYS_INLINE bool wait() {
		      return condvar_.wait();
		  }

#if defined(CH_USE_CONDVARS_TIMEOUT) || defined(CH_CFG_USE_CONDVARS_TIMEOUT)

		  TURAG_ALWAYS_INLINE bool waitFor(SystemTime timeout) {
              return condvar_.waitFor(timeout);
		  }

		  TURAG_ALWAYS_INLINE bool waitUntil(SystemTime timeout) {
              return condvar_.waitUntil(timeout);
		  }

#endif

	private:
		  ConditionVariable& condvar_;
	};

public:
  constexpr TURAG_ALWAYS_INLINE
  ConditionVariable(Mutex* mutex) :
    cond_(_CONDVAR_DATA(cond_)), mutex_(mutex)
  { }

  TURAG_ALWAYS_INLINE bool wait() {
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

  TURAG_ALWAYS_INLINE void signal() {
    chCondSignal(&cond_);
  }

  TURAG_ALWAYS_INLINE void broadcast() {
    chCondBroadcast(&cond_);
  }

  Mutex& getMutex() {
    return *mutex_;
  }

private:
   condition_variable_t cond_;
  Mutex* mutex_;
};
#endif


////////////////////////////////////////////////////////////////////////////////
// Mailbox
#if CH_CFG_USE_MAILBOXES
template<std::size_t size>
class Mailbox {

private:
    mailbox_t mailbox_;
    msg_t pointer_buffer_[size];

public:
    explicit Mailbox() :
        mailbox_(_MAILBOX_DATA(mailbox_, pointer_buffer_, size)) { }

    bool post(void* msg, SystemTime time) {
        if (chMBPostTimeout(&mailbox_, static_cast<msg_t>(msg), time.value) == MSG_OK) {
            return true;
        } else {
            return false;
        }
    }
    bool post(void* msg) { return post(msg, TIME_INFINITE); }

    bool postAhead(void* msg, SystemTime time) {
        if (chMBPostAheadTimeout(&mailbox_, static_cast<msg_t>(msg), time.value) == MSG_OK) {
            return true;
        } else {
            return false;
        }
    }
    bool postAhead(void* msg) { return postAhead(msg, TIME_INFINITE); }

    void* fetch(SystemTime time) {
        msg_t msg;

        if (chMBFetchTimeout(&mailbox_, &msg, time.value) == MSG_OK) {
            return static_cast<void*>(msg);
        } else {
            return nullptr;
        }
    }
    void* fetch(void) { return fetch(TIME_INFINITE); }

};

#endif // CH_CFG_USE_MAILBOXES


////////////////////////////////////////////////////////////////////////////////
// Semaphore
#if CH_CFG_USE_SEMAPHORES
class Semaphore {
  Semaphore(const Semaphore&) = delete;
  Semaphore& operator=(const Semaphore&) = delete;

private:
  semaphore_t sem_;

public:
  explicit Semaphore(int count = 0) :
      sem_(_SEMAPHORE_DATA(sem_, count)) { }

  void wait(void) { while (chSemWait(&sem_) == MSG_RESET); }
  bool wait(SystemTime time);
  void signal(void) { chSemSignal(&sem_); }
};
#endif


////////////////////////////////////////////////////////////////////////////////
// BinarySemaphore
#if CH_CFG_USE_SEMAPHORES
class BinarySemaphore {
  BinarySemaphore(const BinarySemaphore&) = delete;
  BinarySemaphore& operator=(const BinarySemaphore&) = delete;

private:
  binary_semaphore_t sem_;

public:
  explicit BinarySemaphore(bool taken = true) :
      sem_(_BSEMAPHORE_DATA(sem_, taken)) { }

  void wait(void) { while (chBSemWait(&sem_) == MSG_RESET); }
  bool wait(SystemTime time);
  void signal(void) { chBSemSignal(&sem_); }
};
#endif


} // namespace TURAG

#endif // PLATFORM_CHIBIOS3_PUBLIC_TINAPP_THREAD_H
