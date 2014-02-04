#ifndef TINA_SIM_CONFIG_H
#define TINA_SIM_CONFIG_H

#include <tina/tina.h>

/// Attribute für Thread-Startfunktion
#define TURAG_THREAD_ENTRY 

/// Wenn definiert:
/// Nach Aufruf von EventQueue::main() nicht mehr zurückkehren
/// ! dadurch wird EventQueue::event_quit ignoriert
//#undef TURAG_STATEMACHINE_FOREVER

/// ASCII-Zeichen für neue Zeile für Debugausgaben
#define TURAG_DEBUG_NEWLINE "\n"

#define TURAG_FELDBUS_AKTOR_STRUCTURED_OUTPUT_AVAILABLE

#endif // TINA_SIM_CONFIG_H
