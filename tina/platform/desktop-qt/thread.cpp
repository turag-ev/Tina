
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#ifdef WIN32
# include <windows.h>
#endif

#include "public/tina++/thread.h"
#include "public/tina/thread.h"
#include <tina/debug.h>

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

_hot
void Thread_delay(SystemTime d /* [ticks] */) {
  Mutex::Lock lock(p.mutex);
  while (p.paused) p.var.wait();
  lock.unlock();

  if (ticks_to_us(d) < UINT_MAX) {
    usleep(ticks_to_us(d));
  } else {
#ifdef WIN32
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

pthread_t ThreadDetail::createPThread(char* stack, std::size_t stack_size,
                                      int priority, void (*entry) ())
{
#if defined(THREADS_STACK_MEASUREMENT) && !defined(WIN32)
  UNUSED(priority);

  // write 0xDEADBEAF to stack
  uint32_t* dead_stack = reinterpret_cast<uint32_t*>(stack);
  for (std::size_t i = 0; i < stack_size / sizeof(uint32_t); i++) {
    dead_stack[i] = 0xDEADBEEF;
  }
#endif

  pthread_t thread;
  pthread_attr_t thread_attr;

  pthread_attr_init(&thread_attr);
#ifndef WIN32
  pthread_attr_setstack(&thread_attr, stack, stack_size);
  pthread_attr_setschedpolicy(&thread_attr, priority);
#else
  UNUSED(stack);
  UNUSED(stack_size);
  UNUSED(priority);
#endif
  pthread_create(&thread, &thread_attr, thread_entry, reinterpret_cast<void *>(entry));
  pthread_attr_destroy(&thread_attr);

  return thread;
}

extern "C"
void turag_thread_start(TuragThread* thread, int priority, void (*entry) ()) {
  thread->thread = detail::ThreadDetail::createPThread(thread->stack, thread->stack_size, priority, entry);
}

std::size_t ThreadDetail::getStackUsage(const char* stack, std::size_t stack_size) {
#if defined(THREADS_STACK_MEASUREMENT) && !defined(WIN32)
  uint32_t* dead_stack = reinterpret_cast<uint32_t*>(stack);
  for (int i = 0; i < stack_size / sizeof(uint32_t); i++) {
    if (dead_stack[i] != 0xDEADBEEF)
      return stack_size - i * sizeof(uint32_t);
  }
  return 0;
#else
  UNUSED(stack);
  UNUSED(stack_size);
  return 0;
#endif
}

extern "C"
size_t turag_thread_get_stack_usage(const TuragThread* thread) {
  return detail::ThreadDetail::getStackUsage(thread->stack, thread->stack_size);
}

void* ThreadDetail::thread_entry(void* data) {
  reinterpret_cast<void (*)()>(data)();

  pthread_exit(nullptr);
}

} // namespace detail


bool ConditionVariable::waitFor(SystemTime timeout) {
#ifndef _WIN32
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  unsigned sec = ticks_to_s(timeout);
  time_t nsec = (ticks_to_us(timeout - s_to_ticks(sec))*1000 + ts.tv_nsec);
  ts.tv_nsec = nsec % (1000 * 1000 * 1000);
  ts.tv_sec += sec + nsec / (1000 * 1000 * 1000);
  return pthread_cond_timedwait(&cond_, mutptr_, &ts) != ETIMEDOUT;

#else
  // Windows
  struct timespec ts;
  struct _timeb systime;
  _ftime(&systime);
  unsigned sec = ticks_to_s(timeout);
  time_t msec = ticks_to_ms(timeout - s_to_ticks(sec)) + systime.millitm + 1;
  ts.tv_nsec = (msec % 1000) * 1000 * 1000;
  ts.tv_sec += sec + msec / 1000;
  return pthread_cond_timedwait(&cond_, mutptr_, &ts) != ETIMEDOUT;
#endif
}

} // namespace TURAG
