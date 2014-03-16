#ifndef TINAPP_STATEMACHINE_TYPES_H
#define TINAPP_STATEMACHINE_TYPES_H

#include <tina/statemachine/eventqueue.h>

namespace TURAG {

/// \defgroup StateMaschines Zustandsmaschinen
/// \{

/// \defgroup Events Events
/// \{

/// Typ für Event-IDs
typedef TuragEventId EventId;

/// Typ für Funktionen, die Events verarbeiten
typedef TuragEventMethod EventMethod;

/// Typ für Event-spezifische Parameter
typedef TuragEventArg EventArg;

/// \brief Erstellt Namesraum für Event-Ids
/// Die Funktion erstellt aus drei ASCII-Zeichen einen Namesraum für eine Event-ID
/// damit jede Event-ID einzigartig ist. Keine Zeichenfolge sollte doppelt verwendet werden.
///
/// Es wird ein 32-Bit \a unsigned \a integer erstellt, dabei werden die ersten 3 Byte mit den
/// ASCII-Zeichen gefüllt werden. Das letzte Byte kann dafür genutzt werden 255
/// Event-IDs für den Namesraum zu erstellen.
///
/// \code{.cpp}
/// enum {
///     event_zero = EventNamespace('A', 'B', 'C'), // Event mit Namespace ABC mit Index 0
///     event_one,  // Event mit Namespace ABC mit Index 1
/// };
/// \endcode
///
/// \par Anwendungsrichtlinien (SystemControl):
///   - '#' als erstes Zeichen für Events von Aktionen
///   - '*' als erstes Zeichen für Events von Strategien
///   - essentielle Klassen starten mit Großbuchstaben
///   - alle weiteren Zeichen sollten so gewählt werden das Klassename erkennbar ist
///   - niemals zwei gleiche Zeichenfolge benutzen, außer man addiert Wert hinzu
///
/// \param a ASCII-Zeichen 1
/// \param b ASCII-Zeichen 2
/// \param c ASCII-Zeichen 3
/// \returns Event-ID-Namesraum (als erste ID in Namesraum verwendbar)
constexpr
EventId EventNamespace(char a, char b, char c) {
  return (a << 24) | (b << 16) | (c << 8);
}

/// \}
/// \}

} // namespace TURAG

#endif // TINAPP_STATEMACHINE_TYPES_H
