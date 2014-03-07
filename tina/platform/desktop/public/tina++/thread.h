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

#include <tina++/tina.h>
#include <tina++/time.h>

/// when definied, than getStackUsage will give real values. But thread creating time will increase.
//#define THREADS_STACK_MEASUREMENT

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// Thread

void Thread_delay(SystemTime d /* [ticks] */);
void Thread_pause_all();
void Thread_resume_all();

template<size_t size = 0>
class Thread {
    NOT_COPYABLE(Thread);

public:
    explicit Thread()
    { }

    _always_inline
    void join() {
        if (thread_) {
            thread_->join();
        }
    }

    static void setName(const char *) { }
    static void delay(SystemTime d) { Thread_delay(d); }
    static std::size_t getStackUsage(const char*, std::size_t) { return 0; }

    _always_inline
    void start(int, void (*entry) ()) {
        thread_.reset(new std::thread(entry));
    }

private:
    std::unique_ptr<std::thread> thread_;
};

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
    _always_inline ConditionVariable(Mutex* mut) :
        cond_(), workaround_mut_(), mut_(mut)
    { }

    _always_inline bool wait() {
        std::unique_lock<std::mutex> lock(workaround_mut_);
        Unlocker unlock(*mut_);
        cond_.wait(lock);
        return true;
    }

    bool waitFor(SystemTime timeout) {
        std::unique_lock<std::mutex> lock(workaround_mut_);
        Unlocker unlock(*mut_);
        return cond_.wait_for(lock,
                              std::chrono::microseconds(ticks_to_ms(timeout)))
                == std::cv_status::no_timeout;
    }

    _always_inline void signal() {
        cond_.notify_one();
    }

    _always_inline void broadcast() {
        cond_.notify_all();
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

    void notify() {
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
                std::chrono::microseconds(ticks_to_ms(timeout)));
            if (status == std::cv_status::timeout)
                return false;
        }
        --count_;
        return true;
    }
};


} // namespace TURAG

#endif // TINAPP_SIM_THREAD_H
