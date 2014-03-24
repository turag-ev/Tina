#include <unistd.h>
#include <limits.h>
#include <errno.h>
#ifdef _WIN32
# include <windows.h>
#endif

#include "public/tina++/thread.h"
#include "public/tina/thread.h"

#ifdef NDEBUG
# undef THREADS_STACK_MEASUREMENT
#endif

namespace TURAG {

struct ThreadPrivate {
  Mutex mutex;
  ConditionVariable var;
  bool paused;

  ThreadPrivate() :
    mutex(), var(&mutex), paused(false)
  { }
};

static ThreadPrivate p;

extern "C"
void sim_pause_checkpoint() {
    Mutex::Lock lock(p.mutex);
    while (p.paused) p.var.wait();
    lock.unlock();
}

_hot
void Thread_delay(SystemTime d /* [ticks] */) {
  sim_pause_checkpoint();

  if (ticks_to_ms(d)*1000 < UINT_MAX) {
    usleep(ticks_to_ms(d)*1000);
  } else {
#ifdef _WIN32
    Sleep(ticks_to_ms(d));
#else
    sleep(ticks_to_s(d));
#endif
  }
}

extern "C"
void turag_thread_delay(TuragSystemTime d) {
  Thread_delay(SystemTime{d.value});
}

void Thread_pause_all() {
  Mutex::Lock lock(p.mutex);
  p.paused = true;
}

void Thread_resume_all() {
  Mutex::Lock lock(p.mutex);
  p.paused = false;
  p.var.broadcast();
}

namespace detail {

extern "C"
void turag_thread_start(TuragThread* thread, int, void (*entry) ()) {
    *thread = reinterpret_cast<TuragThread>(new std::thread(entry));
}

} // namespace detail

} // namespace TURAG
