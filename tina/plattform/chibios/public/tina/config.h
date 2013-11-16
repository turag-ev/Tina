#ifndef TINA_CHIBIOS_CONFIG_H
#define TINA_CHIBIOS_CONFIG_H

#include <tina/tina.h>

// Attribute für Thread-Startfunktion
#define TURAG_THREAD_ENTRY _noreturn

// Wenn definiert:
// Nach Aufruf von EventQueue::main() nicht mehr zurückkehren
// ! dadurch wird EventQueue::event_quit ignoriert
#define TURAG_STATEMACHINE_FOREVER

#endif // TINA_CHIBIOS_CONFIG_H
