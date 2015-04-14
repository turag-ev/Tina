#ifndef TINAPP_STATEMACHINE_TYPES_H
#define TINAPP_STATEMACHINE_TYPES_H

#include <tina/statemachine/eventqueue.h>
#include <tina++/time.h>

namespace TURAG {

/// \defgroup StateMaschines Zustandsmaschinen [C++]
/// \{

/// \addtogroup Events
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

constexpr
EventId GetEventNamespace(EventId event) {
  return event & 0xFFFFFF00;
}

// /////////////////////////////////////////////////////////////////////////////
//     EventClass
// /////////////////////////////////////////////////////////////////////////////

/// \brief Ereignisklasse definiert durch Ereignis-Id und Beschreibung
struct EventClass {
	/// \brief Eventklasse erstellen
	/// \param name Beschreibung von Ereignis
	/// \param id Id von Ereignis
	constexpr
	EventClass(const char* name, EventId id) :
		name(name), id(id)
	{ }

	/// \brief Beschreibung von Events dieser Eventklasse (nur als Debuginformation benutzbar)
	const char* const name;

	/// \brief Ereignis-Id von Events dieser Eventklasse (zur Identifikation von Events)
	const EventId id;
};

#ifndef __DOXYGEN__
// for compatibility with old code
template<EventId id>
struct UnnamedEventClass {
static const EventClass event_class;
};

template<EventId id>
const EventClass UnnamedEventClass<id>::event_class("Unbekannt", id);
#endif

/// Ereignisklasse \ref TURAG::EventClass definieren mit Ereignisname als Debugbeschreibung
/// \param name Variablenname
/// \param event Ereignis-Id für Ereignis
///
/// \code{.cpp}
///   DEFINE_EVENT_CLASS(EventWon, WinAction::event_won);
///   turag_infof("%s", EventWon->name); // gibt "WinAction::event_won" aus
/// \endcode
#define DEFINE_EVENT_CLASS(name,event) const EventClass name(#event, event)

/// Ereignisklasse \ref TURAG::EventClass definieren mit Ereignisname und Zusatzinformation als Debugbeschreibung
/// \param name Variablenname
/// \param event Ereignis-Id für Ereignis
/// \param msg Zusätzliche Debuginformation zu Ereignistyp
///
/// \code{.cpp}
///   DEFINE_EVENT_CLASS_EXTRA(EventWon, WinAction::event_won, "Wir haben gewonnen!");
///   turag_infof("%s", EventWon->name); // gibt "WinAction::event_won: Wir haben gewonnen!" aus
/// \endcode
#define DEFINE_EVENT_CLASS_EXTRA(name,event,msg) const EventClass name(#event ": " msg, event)

// /////////////////////////////////////////////////////////////////////////////
//     Event
// /////////////////////////////////////////////////////////////////////////////

/// \brief Ereignis
struct Event {
  const EventClass* event_class; ///< zugehörige Ereignisklasse
  EventArg          param;       ///< optionaler Parameter

  /// \brief Callbackfunktion.
  /// Funktion, die bei Ereignisverarbeitung mit Ereignis aufgerufen werden soll oder
  /// <code>nullptr</code>, wenn Ereignis in Standard-Ereignisverarbeitungsfunktion übergeben werden soll.
  EventMethod       method;

  /// \brief Ereignis aus Ereignisklasse erstellen
  /// \note Ereignis sollte nie selber erstellt werden, sondern die Funktionen der
  /// \ref TURAG::EventQueue "EventQueue" benutzt werden.
  constexpr
  Event(const EventClass* event_class, EventArg param, EventMethod method) :
	event_class(event_class), param(param), method(method)
  { }

  /// Instanzen von Klasse sind verschiebbar.
  MOVABLE(Event);

  /// Instanzen von Klasse sind kopierbar.
  COPYABLE(Event);
};

/// \brief zeitverzögertes Ereignis
struct TimeEvent {
  MOVABLE(TimeEvent);
  COPYABLE(TimeEvent);

  /// \brief zugehöriges Ereignis was ausgelöst werden soll.
  Event     event;

  /// \brief absolute Zeit bei der Ereignis ausgeführt werden soll.
  SystemTime time;

  /// zeitverzögertes Ereignis erstellen
  /// \note Ereignis sollte nie selber erstellt werden, sondern die Funktionen der
  /// \ref TURAG::EventQueue "EventQueue" benutzt werden.
  constexpr
  TimeEvent(const Event& event, SystemTime time) :
	event(event), time(time)
  { }

  /// zeitverzögertes Ereignis erstellen
  /// \note Ereignis sollte nie selber erstellt werden, sondern die Funktionen der
  /// \ref TURAG::EventQueue "EventQueue" benutzt werden.
  constexpr
  TimeEvent(const EventClass* event_class, EventArg param, EventMethod method, SystemTime time) :
	event(event_class, param, method), time(time)
  { }
};

/// \}
/// \}

} // namespace TURAG

#endif // TINAPP_STATEMACHINE_TYPES_H
