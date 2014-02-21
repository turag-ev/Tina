#ifndef TINA_SIM_CONFIG_H
#define TINA_SIM_CONFIG_H

#include <tina/tina.h>

// Attribute für Thread-Startfunktion
#define TURAG_THREAD_ENTRY 

// Wenn definiert:
// Nach Aufruf von EventQueue::main() nicht mehr zurückkehren
// ! dadurch wird EventQueue::event_quit ignoriert
//#undef TURAG_THREADS_RUN_FOREVER

#endif // TINA_SIM_CONFIG_H
