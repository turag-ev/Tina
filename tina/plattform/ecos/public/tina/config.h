#ifndef TINA_ECOS_CONFIG_H
#define TINA_ECOS_CONFIG_H

#include <tina/tina.h>

/// Attribute für Thread-Startfunktion
#define TURAG_THREAD_ENTRY _noreturn

/// Wenn definiert:
/// Nach Aufruf von EventQueue::main() nicht mehr zurückkehren
/// ! dadurch wird EventQueue::event_quit ignoriert
#define TURAG_STATEMACHINE_FOREVER

/// ASCII-Zeichen für neue Zeile für Debugausgaben
#define TURAG_DEBUG_NEWLINE "\r\n"

#endif // TINA_ECOS_CONFIG_H
