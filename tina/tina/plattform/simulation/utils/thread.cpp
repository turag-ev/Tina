
#include <unistd.h>
#include <limits.h>
#ifdef WIN32
# include <windows.h>
#endif

#include "thread.h"
#include "cthread.h"
#include <utils/debug.h>

#ifdef NDEBUG
# undef THREADS_STACK_MEASUREMENT
#endif

namespace SystemControl {

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
void thread_delay(SystemTicks d) {
  Thread_delay(SystemTime{d});
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
                                      int priority, void (*entry) (uint32_t))
{
#if defined(THREADS_STACK_MEASUREMENT) && !defined(WIN32)
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
#endif
  pthread_create(&thread, &thread_attr, thread_entry, reinterpret_cast<void *>(entry));
  pthread_attr_destroy(&thread_attr);

  return thread;
}

extern "C"
void thread_start(CThread* thread, int priority, void (*entry) (uint32_t)) {
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
size_t thread_get_stack_usage(const CThread* thread) {
  return detail::ThreadDetail::getStackUsage(thread->stack, thread->stack_size);
}

void* ThreadDetail::thread_entry(void* data) {
  reinterpret_cast<void (*)(uint32_t)>(data)(0);

  pthread_exit(nullptr);
  return nullptr;
}

} // namespace detail

} // namespace SystemControl
