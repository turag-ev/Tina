#ifndef TINAPP_CONTAINER_THREADFIFO_H 
#define TINAPP_CONTAINER_THREADFIFO_H

#include <tina++/tina.h>
#include <tina++/thread.h>
#include <tina++/container/circular_buffer.h>

namespace TURAG {
	
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
        empty_sem_(buffer_.capacity()), full_sem_(0)
    { }

    void post(const T& mail) {
        empty_sem_.wait();
        Mutex::Lock lock(buffer_mutex_);
        buffer_.emplace_back(mail);
        full_sem_.signal();
    }

    bool fetch(T* mail, SystemTime time) {
        if (!full_sem_.wait(time)) {
            return false;
        }
        Mutex::Lock lock(buffer_mutex_);
        *mail = buffer_.front();
        buffer_.pop_front();
        empty_sem_.signal();
        return true;
    }

    void fetch(T* mail) {
        full_sem_.wait();
        Mutex::Lock lock(buffer_mutex_);
        *mail = buffer_.front();
        buffer_.pop_front();
        empty_sem_.signal();
    }
	
};
	
}

#endif // TINAPP_CONTAINER_THREADFIFO_H
