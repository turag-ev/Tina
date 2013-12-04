#ifndef TINA_STD_CONFIG_H
#define TINA_STD_CONFIG_H

/// Attribute für Thread-Startfunktion
#define TURAG_THREAD_ENTRY

/// Wenn definiert:
/// Nach Aufruf von EventQueue::main() nicht mehr zurückkehren
/// ! dadurch wird EventQueue::event_quit ignoriert
//#undef TURAG_STATEMACHINE_FOREVER

/// ASCII-Zeichen für neue Zeile für Debugausgaben
#define TURAG_DEBUG_NEWLINE "\n"

#endif // TINA_STD_CONFIG_H
