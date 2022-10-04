/**
 * \file thread.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2011,2012
 *
 * \brief implementation of thread, mutex and scoped_lock
 *
 */

#ifndef PLATFORM_ROS_INCLUDE_TINAPP_THREAD_H
#define PLATFORM_ROS_INCLUDE_TINAPP_THREAD_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

#include <tina++/time.h>

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// Thread

class ThreadImpl
{
	ThreadImpl(const ThreadImpl&) = delete;
	ThreadImpl& operator=(const ThreadImpl&) = delete;

public:
	explicit ThreadImpl();
	~ThreadImpl();

	void join();
	void start(int, void (*entry) ());

	void terminate() {
	  terminate_request_ = true;
	}

	bool shouldTerminate() {
	  return terminate_request_;
	}

	static std::size_t getStackUsage() { return 0; }

private:
	std::unique_ptr<std::thread> thread_;

	std::atomic<bool> terminate_request_;
};


template<size_t size>
using Thread = ThreadImpl;

struct CurrentThread {
private:
  // von dieser Klasse gibt es keine Instanz
  CurrentThread();

public:
  /// lets the current thread sleeps for a time of system ticks
  static void delay(SystemTime ticks) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(ticks.toTicks()));
  }

  static void setName(const char *name);
};

namespace AllThreads {

void pause();
void resume();
void join();
void terminate();

} // namespace AllThreads

////////////////////////////////////////////////////////////////////////////////
// Mutex

class Mutex : public std::mutex {
public:
    typedef std::unique_lock<mutex> Lock;
};

////////////////////////////////////////////////////////////////////////////////
// ConditionVariable

class ConditionVariable {
    ConditionVariable(const ConditionVariable&) = delete;
    ConditionVariable& operator=(const ConditionVariable&) = delete;

    class Unlocker {
        Unlocker(const Unlocker&) = delete;
        Unlocker& operator=(const Unlocker&) = delete;
        Unlocker(Unlocker&&) = delete;
        Unlocker& operator=(Unlocker&&) = delete;

    public:
        TURAG_ALWAYS_INLINE explicit Unlocker(Mutex& m) :
            m_(m), locked_(false)
        {
            m_.unlock();
        }

        TURAG_ALWAYS_INLINE ~Unlocker() {
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

        TURAG_ALWAYS_INLINE bool wait() {
            return condvar_.wait(*this);
        }

        TURAG_ALWAYS_INLINE bool waitFor(SystemTime timeout) {
            return condvar_.waitFor(*this, timeout);
        }

    private:
        ConditionVariable& condvar_;
    };

public:
    TURAG_ALWAYS_INLINE ConditionVariable(Mutex* mut) :
        cond_(), workaround_mut_(), mut_(mut)
    { }

    TURAG_ALWAYS_INLINE bool wait() {
        std::unique_lock<std::mutex> lock(workaround_mut_);
        Unlocker unlock(*mut_);
        cond_.wait(lock);
        return true;
    }

    TURAG_ALWAYS_INLINE bool wait(std::unique_lock<std::mutex>& lock) {
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

    TURAG_ALWAYS_INLINE void signal() {
        cond_.notify_one();
    }

    TURAG_ALWAYS_INLINE void broadcast() {
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
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

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

#endif // PLATFORM_ROS_INCLUDE_TINAPP_THREAD_H
