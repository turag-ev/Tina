#ifndef TINAPP_CHIBIOS_THREADFIFO_H 
#define TINAPP_CHIBIOS_THREADFIFO_H


#include <ch.h>

#include <tina++/tina.h>
#include <tina++/thread.h>
#include <tina++/container/circular_buffer.h>


namespace TURAG {
	
/// size must bei multiple of 2, but you can only save size-1 elements.
template <typename T, std::size_t size>
class ThreadFifo {
	NOT_COPYABLE(ThreadFifo);

private:
	CircularBuffer<T, size> buffer_;
	Mutex buffer_mutex_;
    Semaphore empty_sem_;
    Semaphore full_sem_;

public:
    explicit ThreadFifo() :
        empty_sem_(size-1), full_sem_(0)
    { }

    void post(const T& mail) {
        empty_sem_.wait();
        Mutex::Lock lock(buffer_mutex_);
        buffer_.push_back(mail);
        full_sem_.signal();
        lock.unlock();
    }

    void fetch(T* mail) {
        full_sem_.wait();
        Mutex::Lock lock(buffer_mutex_);
        *mail = buffer_.front();
        buffer_.erase(buffer_.begin());
        empty_sem_.signal();
        lock.unlock();
    }
	
};
	
}

#endif // TINAPP_CHIBIOS_THREADFIFO_H
