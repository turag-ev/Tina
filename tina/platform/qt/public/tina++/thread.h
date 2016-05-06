/**
 * \file thread.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2011,2012
 *
 * \brief implementation of thread, mutex and scoped_lock
 *
 */

#ifndef TINAPP_SIM_THREAD_H
#define TINAPP_SIM_THREAD_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

#include <tina++/tina.h>
#include <tina++/time.h>

/// when definied, than getStackUsage will give real values. But thread creating time will increase.
//#define THREADS_STACK_MEASUREMENT

// stub, no CCM on PC
#define TURAG_ATTR_SECTION_CCMDATA

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// Thread

template<size_t size = 0>
class Thread {
    NOT_COPYABLE(Thread);

public:
    explicit Thread() :
      terminate_request_(false)
    { }

    _always_inline
    void join() {
        if (thread_) {
            thread_->join();
        }
    }

	static std::size_t getStackUsage() { return 0; }

    _always_inline
    void start(int, void (*entry) ()) {
        thread_.reset(new std::thread(entry));
    }

    _always_inline
    void terminate() {
      terminate_request_ = true;
    }

    _always_inline
    bool shouldTerminate() {
      return terminate_request_;
    }

private:
    std::unique_ptr<std::thread> thread_;

    std::atomic<bool> terminate_request_;
};

struct CurrentThread {
private:
  // von dieser Klasse gibt es keine Instanz
  CurrentThread();

public:
  /// lets the current thread sleeps for a time of ecos ticks
  static void delay(SystemTime ticks);

  static _always_inline void setName(const char *name) { }
};

inline void Thread_delay(SystemTime d /* [ticks] */) {
	CurrentThread::delay(d);
}

void Thread_pause_all();
void Thread_resume_all();

////////////////////////////////////////////////////////////////////////////////
// Mutex

class Mutex : public std::mutex {
public:
    typedef std::unique_lock<mutex> Lock;
};

////////////////////////////////////////////////////////////////////////////////
// ConditionVariable

class ConditionVariable {
    NOT_COPYABLE(ConditionVariable);

    class Unlocker {
        NOT_COPYABLE(Unlocker);
        NOT_MOVABLE(Unlocker);

    public:
        _always_inline explicit Unlocker(Mutex& m) :
            m_(m), locked_(false)
        {
            m_.unlock();
        }

        _always_inline ~Unlocker() {
            if (!locked_) {
                m_.lock();
            }
        }

    private:
        Mutex& m_;
        bool locked_;
    };

public:
    class Lock : public std::unique_lock<std::mutex> {
    public:
        Lock(ConditionVariable& condvar) :
            std::unique_lock<std::mutex>(condvar.getMutex()),
            condvar_(condvar)
        { }

        _always_inline bool wait() {
            return condvar_.wait(*this);
        }

        _always_inline bool waitFor(SystemTime timeout) {
            return condvar_.waitFor(*this, timeout);
        }

    private:
        ConditionVariable& condvar_;
    };

public:
    _always_inline ConditionVariable(Mutex* mut) :
        cond_(), workaround_mut_(), mut_(mut)
    { }

    _always_inline bool wait() {
        std::unique_lock<std::mutex> lock(workaround_mut_);
        Unlocker unlock(*mut_);
        cond_.wait(lock);
        return true;
    }

    _always_inline bool wait(std::unique_lock<std::mutex>& lock) {
        cond_.wait(lock);
        return true;
    }

    bool waitFor(SystemTime timeout) {
        std::unique_lock<std::mutex> lock(workaround_mut_);
        Unlocker unlock(*mut_);
        return cond_.wait_for(lock,
                              std::chrono::milliseconds(timeout.toMsec()))
                == std::cv_status::no_timeout;
    }

    bool waitFor(std::unique_lock<std::mutex>& lock, SystemTime timeout) {
        return cond_.wait_for(lock,
                              std::chrono::milliseconds(timeout.toMsec()))
                == std::cv_status::no_timeout;
    }

    _always_inline void signal() {
        cond_.notify_one();
    }

    _always_inline void broadcast() {
        cond_.notify_all();
    }

    Mutex& getMutex() {
        return *mut_;
    }

private:
    std::condition_variable cond_;
    std::mutex workaround_mut_;
    Mutex* mut_;
};


////////////////////////////////////////////////////////////////////////////////
// Semaphore

class Semaphore {
    NOT_COPYABLE(Semaphore);

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_;

public:
    Semaphore(int count = 0)
        : mutex_(), condition_(), count_(count)
    { }

    void signal() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!count_)
            condition_.wait(lock);
        --count_;
    }

    bool wait(SystemTime timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!count_) {
            std::cv_status status = condition_.wait_for(lock,
                std::chrono::milliseconds(timeout.toMsec()));
            if (status == std::cv_status::timeout)
                return false;
        }
        --count_;
        return true;
    }
};


} // namespace TURAG

#endif // TINAPP_SIM_THREAD_H
