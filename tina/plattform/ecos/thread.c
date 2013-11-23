#include "public/tina/thread.h"

TURAG_THREAD_ENTRY
void _turag_thread_entry(cyg_addrword_t data) {
  TURAG_THREAD_ENTRY void (*entry)(void) = (void (*)(void))data;
  entry();
}
