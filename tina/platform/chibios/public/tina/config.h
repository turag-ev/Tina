#ifndef TINA_CHIBIOS_CONFIG_H
#define TINA_CHIBIOS_CONFIG_H

#include <tina/tina.h>

/// Attribute für Thread-Startfunktion
#define TURAG_THREAD_ENTRY _noreturn

/// Wenn definiert:
/// Nach Aufruf von EventQueue::main() nicht mehr zurückkehren
/// ! dadurch wird EventQueue::event_quit ignoriert
#define TURAG_THREADS_RUN_FOREVER

/// ASCII-Zeichen für neue Zeile für Debugausgaben
#define TURAG_DEBUG_NEWLINE "\n"

#endif // TINA_CHIBIOS_CONFIG_H
