/**
 * \file tina++/statemachine/eventqueue.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief EventQueue definition
 *
 */

#ifndef TINAPP_STATEMACHINE_EVENTQUEUE_H
#define TINAPP_STATEMACHINE_EVENTQUEUE_H

#include <tina++/thread.h>
#include <tina++/time.h>
#include <tina++/container/circular_buffer.h>
#include <tina++/container/array_buffer.h>
#include <tina++/statemachine/types.h>

namespace TURAG {

/// \addtogroup StateMaschines
/// \{

/// \defgroup Events Ereignisse [C++]
/// \brief Ereignisverarbeitung.
///
/// Ereignisse (Event, TimeEvent)
/// -----------------------------
///
/// Ereignisse stellen Zeitpunkte der Veränderung dar auf die reagiert werden kann.
/// So kann man auf das Ergebnis einer ausgelösten Aktion warten oder zeitversetzt
/// Aktionen durchführen.
///
/// Ein Ereignis kann nur durch eine Ereignisklasse erstellt werden. Die Klasse
/// ist durch ihre eindeutige Id charaterisiert. Diese Id wird durch die Benutzung von
/// \ref EventNamespace aus drei ASCII-Zeichen generiert. Zusätzlich kann jeder Klasse
/// von Ereignissen eine Zeichenkette mit der Beschreibung des Ereignisses mitgegeben werden.
/// Diese kann für Debuggingzwecke benutzt werden. So wird vor der
/// Verarbeitung eines Ereignisses in der Ereignisverarbeitung (\ref TURAG::EventQueue "EventQueue") die Beschreibung
/// ins Log (\ref Debug) geschrieben. Es ist möglich mehreren Ereignis-Klassen die gleiche
/// Id zu geben. Dadurch kann man einen Ereignis unterschiedliche Beschreibungen
/// mitgeben um die Herkunft des Ereignisses besser zu erkennen. Aus einer Ereignisklasse
/// kann ein Ereignis erstellt werden.
///
/// Eine Ereignisklasse wird erstellt durch das Makro \ref TURAG::DEFINE_EVENT_CLASS oder
/// \ref TURAG::DEFINE_EVENT_CLASS_EXTRA. Dessen erstes Argument ist der Bezeichner für
/// die Klasse, der später darf verwendet werden kann um die Ereignisklasse angeben zu können.
/// Der zweite Parameter ist die Id der Ereignisklasse. Aus der Id wird die Beschreibung generiert.
/// Will man eine detailiertere Beschreibung hinzufügen so ist das mit \ref DEFINE_EVENT_CLASS_EXTRA möglich, wo der
/// dritte Parameter der Beschreibung hinzugefügt wird.
///
/// Ereignisverarbeitung (EventQueue)
/// ---------------------------------
///
/// Die Ereignisverarbeitung wird über die Funktion \ref TURAG::EventQueue::main "main"
/// gestartet. Diese Funktion wartet auf neue Ereignisse bzw. wartet auf bis zeitversetzte
/// Ereignisse aufgelöst werden müssen. Die Funktion kehrt erst zurück, wenn das Event
/// \ref TURAG::EventQueue::event_quit "event_quit" ausgeführt wird, was in einer
/// Simulationumgebung genutzt werden kann, um die Ereignisverarbeitung zu beenden.
/// Die Funktion erwartet zwei Funtionszeiger als Parameter. Die erste Funktion
/// wird aufgerufen, wenn das Ereignis selber nicht explizit über \ref TURAG::Event::method "Event::method"
/// eine Funktion angibt. In dieser Funktion muss das Ereignis verarbeitet oder
/// weitergegeben werden. Die zweite Funktion wird aufgerufen bevor eine Event
/// verarbeitet wird, aber mindestens im Abstand von der Zeit die in \ref TURAG::EventQueue::max_tick_time
/// "EventQueue::max_tick_time" angegeben ist. So kann auf Variablen oder Zustände geprüft werden,
/// die bei einer Veränderung kein Ereignis nach sich ziehen.
///
/// Eine andere Möglichkeit
/// in bestimmten Abständen Variablen zu prüfen ist über die Funktion \ref TURAG::EventQueue::pushTimedelayed
/// "pushTimedelayed" über ein Ereignis regelmäßig eine Funktion aufzurufen. Acht zu geben ist
/// auf die Funktion \ref TURAG::EventQueue::clear "clear", da sie alle Ereignisse verwirft. Damit
/// wird auch das wiederholte Aufrufen der Funktion unterbunden.
///
/// \code{.cpp}
///   void handle_event(EventId id, EventArg data) {
///     // Event verarbeiten oder weiterleiten
///   }
///
///   void tick_event() {
///     // regelmäßig Prüfung durchführen
///   }
///
///   EventQueue eventqueue;
///   int main() {
///     eventqueue.main(handle_event);
///   }
/// \endcode
///
/// Die ankommenden Ereignisse werden in einer Liste chronologisch gesammelt und
/// entspricht im Normalfall einer Warteschlange (FIFO). Ereignisse werden über
/// \ref TURAG::EventQueue::push "push" zur möglichst zeitnahen Verarbeitung hinzugefügt.
/// Soll ein Event nicht hinten eingefügt werden sondern vor allen anderen, kann
/// man dies über \ref TURAG::EventQueue::pushToFront "pushToFront" machen. Für das Einfügen eines
/// neuen Ereignissen ist dessen Ereignisklasse nötig, daraus wird das eigentliche
/// Ereigniss erstellt. Optional kann ein Parameter mit übergeben werden. Dieser
/// ist entweder ein 32-Bit Integer oder ein anderer Typ der maximal 32-Bit groß ist.
/// Der Parametertyp und dessen mögliche Werte müssen in der Dokumentation zu der
/// Ereignisklasse festgelegt werden. Es erfolgt keine Prüfung über die Richtungkeit
/// des Types des übergebenes Objekts in der Ereignisverarbeitung!
///
/// Ereignis-Filterung
/// -----------------
///
/// ### globale Filterung
///
/// \code
///  void EventEntry(EventId id, EventData data) {
///   // filter über spezielle Id
///   if (first_byte(id) == '!') {
///     special_world.func(id, data);
///   }
///   else
///   {
///     main_action.func(id, data);
///   }
/// }
/// \endcode
///
/// Vorteile:
///  * wenig Fehleranfällig (alle Ereignisklasse muss nur spezielle Eigenschaften aufweisen)
///    auch für Ereignisse, die aus Quellen kommen, bei denen man Callback für Ereignis nicht einstellen kann
///  * extrem flexible Filterregeln und Änderungsregeln möglich
///
/// Nachteile:
///  * Overhead, weil jedes Ereignis überprüft werden muss (Deep Packet Inspection)
///  * Spezielle Eigenschaft muss reserviert werden (z.B. Byte von Ereignisnamensraum, evtl. Fehleranfällig bei fehlerender Absprache)
///  * starke Kopplung
///
/// ### aufgabenorientierte Filterung
///
/// Callback bei jeder Action angeben (Beispiel Pfadplannung in SC, leitet
/// Events von MC an eigene Funktion weiter). Für Weiterleitung an Aktionen
/// muss Ereignis neu zu EventQueue hinzugefügt werden.
/// \code
///
/// drive.driveRelative(-100_mm, null, mySpecialWorld);
///
/// void mySpecialWorld(EventId id, EventData data) {
///   special_world.func(id, data);
/// }
/// \endcode
///
/// Nachteile:
///  * entsprechendes Callback muss immer angeben werden, wenn es vergessen wird, landet Ereignis in der falschen Aktionshierachie (Fehleranfällig)
///  * nur für Ereignisse anwendbar bei denen sich Callback bestimmen lässt
/// Vorteil:
///  * wenig Overhead (1 Funktionsaufruf + Stack)
///  * unabhängig vom Rest programmierbar (keine Kopplung zu anderen Aktionen)
///
/// Erklärung in Code:
/// \code{.cpp}
///   // in anyaction.h
///
///   // Klasse, die Events generiert
///   class AnyAction {
///     enum Events {
///       event_something_happend = EventNamespace('#', 'A', 'y'),
///       event_something_else_happend,
///     };
///     // ...
///   };
///
///   // in anyaction.cpp
///
///   // Eventklasse aus Id erstellen
///   DEFINE_EVENT_CLASS(EventSomethingHappend, AnyAction::event_something_happend);
///
///   // Eventklasse mit zusätzlichen Debuginformationen erstellen
///   DEFINE_EVENT_CLASS_EXTRA(EventSomethingElseHappend, AnyAction::event_something_else_happend, "Es ist etwas passiert!");
///
///   // Anwendung mit \ref EventQueue:
///   int main() {
///     EventQueue queue;
///
///     // Ereignis ohne Argument oder spezielles Callback werfen
///     queue.push(&EventSomethingHappend);
///
///     // Ereignis mit Argument
///     queue.push(&EventSomethingElseHappend, 42);
///
///     // Ereignis von spezieller Funktion behandeln lassen:
///     queue.push(&EventSomethingHappend, handle_event);
///     queue.push(&EventSomethingElseHappend, 42, handle_event);
///
///     queue.main();
///  }
///
///   void handle_event(EventId id, EventArg data) {
///     switch (id) {
///     case AnyAction::event_something_happend:
///       // Etwas machen
///       break;
///
///     case AnyAction::event_something_else_happend:
///       // Etwas anderes mit Argument `data' machen
///       break;
///     }
///   }
///
/// \endcode
/// \{

////////////////////////////////////////////////////////////////////////////////
//     EventQueue
////////////////////////////////////////////////////////////////////////////////

/// Ereignisverarbeitung
class EventQueue {
public:
  /// Typ für Funktion zur Verarbeitung von Ereignis
  typedef void (*EventHandler)(EventId id, EventArg data);

  /// Typ für Tick-Funktion
  typedef void (*TickHandler)();

  /// Ereignisverarbeitung erstellen
  EventQueue();

  /** \brief Ereignisverarbeitung starten und erst zurückkehren, wenn \ref TURAG::EventQueue::quit aufgerufen wurde.
   * \param handler Wenn keine Funktion in Ereignis explizit angegeben wurde, dann
   *                Ereignis von dieser Funktionen verarbeiten lassen.
   * \param tick Funktion, die jedes mal aufgerufen wird, bevor Ereignis verabeitet
   *             wird oder mindestens in \ref max_tick_time Abständen. Bei letztem ist
   *             der \a id Parameter TURAG::EventQueue::event_null und der data Parameter 0.
   */
  void main(EventHandler handler, TickHandler tick);

  /// Ereignis direkt ohne Pufferung verarbeiten
  /// \param id Id von Ereignis
  /// \param param Argument von Ereignis, sonst Null
  /// \param callback explizite Angabe der Ereignisverarbeitungsfunktion, sonst <code>nullptr</code>, in diesem Fall Verarbeitung durch \ref handler_
  void processEvent(EventId id, EventArg param, EventMethod callback);

  /// Push quit event (\a EventQueue::event_quit) to front of event queue.
  /**
   * When event_quit is processed, EventQueue::main will return.
   * Only useful outside of embedded system.
   */
  void quit();

  /// Push an new event to the event processing loop
  /**
   * \param event_class Event-Klasse aus dem Event erstellt werden soll.
   * \param params pointer to data for extra information and/or further processing
   * \param method function for processing event or nullptr. When nullptr is passed,
   *               event is given to the main action event function.
   */
  void push(const EventClass* event_class, EventArg params = 0, EventMethod method = nullptr);

  /// Push an new event to the front of the event processing loop
  /** Paramters the same as in push */
  void pushToFront(const EventClass* event_class, EventArg param = 0, EventMethod method = nullptr);

  /// Process event in a number of kernel ticks
  /**
   * NOTE: timedelayed events are more privileged as normal events.
   * \param ticks  time from now in ecos ticks to process event
   * \param event_class Event-Klasse aus dem Event erstellt werden soll.
   * \param param  parameter for data for extra information and/or further processing
   * \param method function for processing event or nullptr. When nullptr is passed,
   *               event is given to the main action event function.
   */
  void pushTimedelayed(SystemTime ticks, const EventClass* event_class, EventArg param = 0, EventMethod method = nullptr);

  /// Remove all events from timedelayed event queue with that id.
  /**
   * \param id event id that is searched
   */
  void removeTimedelayed(EventId id);

  /// Remove all events from event queue with that id.
  /**
   * \param id event id that is searched
   */
  void removeEvent(EventId id);

  /// \brief alle gespeicherten Ereignisse löschen
  ///
  /// Löscht Ereignisse von allen eingegangenen wartenden Ereignissen, die
  /// über die Funktionen \ref push, \ref pushToFront und \ref pushTimedelayed hinzugefügt wurden.
  ///
  /// \warning Ereignisse, die zeitverzögert ausgerufen werden um Aufgaben
  /// einmalig oder wiederkehrend auszuführen werden nicht mehr ausgeführt.
  /// Nur in wenigen Situationen sinnvoll, wenn alle Vorgänge hart beendet werden sollen.
  ///
  void clear();

  /// \brief Löscht alle auf die Ausführung wartenden Ereignisse.
  ///
  /// Es werden alle Ereignisse gelöscht, die mit den Funktionen \ref push und \ref pushToFront
  /// hinzugefügt wurden, aber noch auf die Ausführung warten.
  ///
  void discardEvents();

#ifndef NDEBUG
  /// \brief Debuginformationen zu zeitversetzten Ereignissen ausgeben
  void printTimeQueue();

  /// \brief Debuginformationen zu auf die Ausführung wartenden Ereignissen ausgeben
  void printQueue();

  /// \brief allgemeine Debuginformationen ausgeben
  void printDebugInfo();
#endif

  /// \brief Zahl der maximalen Anzahl an auf die Ausführung wartenden Ereignissen
  static const size_t size = 31;

  enum {
    event_null = 0, ///< \a Ereignis-Id für nichts. Wird verwendet, um zu Ereignis löschen.
    event_idle,     ///< \a Ereignis-Id wenn nichts zu machen ist.
  };

  /// \a Ereignis-Id zum Beenden der Ereignisverarbeitung in \ref main.
  static const EventId event_quit = -1;

  /// Kapazität an zeitverzögerten Ereignissen
  static const size_t timequeue_size = 32;

  /// maximale Zeit zwischen zwei Aufrufen der tick-Funktion (Parameter von \ref main Funktion)
  static constexpr SystemTime max_tick_time = SystemTime::fromMsec(100);

private:
  /// Mutex für thread-sichere Verarbeitung
  mutable Mutex mutex_;

  /// Bedingte Variable für warten auf neues Ereignis
  ConditionVariable var_;

  /// Ringbuffer für eingehene Ereignisse
  CircularBuffer<Event, EventQueue::size> queue_;

  /// Array mit zeitverzögerten Ereignissen
  ArrayBuffer<TimeEvent, EventQueue::timequeue_size> timequeue_;

  /// Funktion, die aufgerufen wird wenn in Ereignis nicht explizit eine Funktion zur Event verarbeitung angegeben ist.
  EventHandler handler_;

  /// nach neuen Ereignis suchen
  /// \param[out] event Speicherort für mögliches gefundenes Ereignis
  /// \retval true Ereignis gefunden
  /// \retval false kein Ereignis gefunden
  bool loadEvent(Event* event);

  /// Zeit zu nächsten aus zu führenden Event
  SystemTime getTimeToNextEvent() const;
};

/// \}
/// \}

extern const EventClass EventQuit;

} // namespace TURAG

#endif // TINAPP_STATEMACHINE_EVENTQUEUE_H


