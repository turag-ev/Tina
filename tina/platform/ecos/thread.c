#include "public/tina/thread.h"

void _turag_thread_entry(cyg_addrword_t data) {
  void (*entry)(void) = (void (*)(void))data;
  entry();
}
