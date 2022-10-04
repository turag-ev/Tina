#include <unistd.h>
#include <limits.h>
#include <errno.h>

#ifdef _WIN32
# include <windows.h>
#else
# include <sys/prctl.h>
#endif

#include "public/tina++/thread.h"
#include "public/tina/thread.h"


namespace TURAG {

TURAG_HOT_FUNC
void CurrentThread::delay(SystemTime d /* [ticks] */) {
  if (d.toMsec()*1000 < UINT_MAX) {
    usleep(d.toMsec()*1000);
  } else {
#ifdef _WIN32
    Sleep(d.toMsec());
#else
    sleep(d.toSec());
#endif
  }
}

void CurrentThread::setName(const char *name)
{
#ifndef _WIN32
    prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name), 0, 0, 0);
#endif
}

extern "C"
void turag_thread_delay(TuragSystemTime d) {
  CurrentThread::delay(SystemTime{d.value});
}



namespace detail {

extern "C"
void turag_thread_start(TuragThread* thread, int, void (*entry) ()) {
    *thread = reinterpret_cast<TuragThread>(new std::thread(entry));
}

} // namespace detail

} // namespace TURAG
