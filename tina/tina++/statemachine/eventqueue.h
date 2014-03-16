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

#include <type_traits>

#include <tina++/thread.h>
#include <tina++/helper/packed.h>
#include <tina++/container/circular_buffer.h>
#include <tina++/container/array_buffer.h>
#include <tina++/statemachine/types.h>

namespace TURAG {

/// \addtogroup StateMaschines Zustandsmaschinen
/// \{

/// \defgroup Events Events
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
///   // vorher definiert: EventQueue queue;
///
///   // Event ohne Argument oder spezielles Callback werfen
///   queue.push(&EventSomethingHappend);
///
///   // Event mit Argument
///   queue.push(&EventSomethingElseHappend, 42);
///
///   // Event von spezieller Funktion behandeln lassen:
///   queue.push(&EventSomethingHappend, handle_event);
///   queue.push(&EventSomethingElseHappend, 42, handle_event);
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
//     EventClass
////////////////////////////////////////////////////////////////////////////////

/// \brief Klasse von Events, definiert durch Event-Id und Name
struct EventClass {
    /// Eventklasse erstellen
    /// \param name Beschreibung von Event
    /// \param id Id von Event
    constexpr
    EventClass(const char* name, EventId id) :
        name(name), id(id)
    { }

    /// Beschreibung von Events dieser Eventklasse (nur als Debuginformation benutzbar)
    const char* const name;

    /// Event-Id von Events dieser Eventklasse (zur Identifikation von Events)
    const EventId id;
};

#ifndef DOXYGEN
// for compatibility with old code
template<EventId id>
struct UnnamedEventClass {
static const EventClass event_class;
};

template<EventId id>
const EventClass UnnamedEventClass<id>::event_class("Unbekannt", id);
#endif

/// Eventklasse \ref TURAG::EventClass definieren mit Eventname als Debugbeschreibung
/// \param name Variablenname
/// \param event Event-Id für Event
///
/// \code{.cpp}
///   DEFINE_EVENT_CLASS(EventWon, WinAction::event_won);
///   infof("%s", EventWon->name); // gibt "WinAction::event_won" aus
/// \endcode
#define DEFINE_EVENT_CLASS(name,event) const EventClass name(#event, event)

/// Eventklasse \ref TURAG::EventClass definieren mit Eventname als Debugbeschreibung
/// \param name Variablenname
/// \param event Event-Id für Event
/// \param msg Zusätzliche Debuginformation zu Eventklasse
///
/// \code{.cpp}
///   DEFINE_EVENT_CLASS_EXTRA(EventWon, WinAction::event_won, "Wir haben gewonnen!");
///   infof("%s", EventWon->name); // gibt "WinAction::event_won: Wir haben gewonnen!" aus
/// \endcode
#define DEFINE_EVENT_CLASS_EXTRA(name,event,msg) const EventClass name(#event ": " msg, event)

////////////////////////////////////////////////////////////////////////////////
//     Event
////////////////////////////////////////////////////////////////////////////////

/// Event
struct Event {
  MOVABLE(Event);
  COPYABLE(Event);

  const EventClass* event_class; ///< event id
  EventArg          param;       ///< parameters
  EventMethod       method;      ///< event method for async method calls or \a nullptr

  constexpr
  Event(const EventClass* event_class, EventArg param, EventMethod method) :
    event_class(event_class), param(param), method(method)
  { }
};

/// Event with time information for timedelayed events
struct TimeEvent {
  MOVABLE(TimeEvent);
  COPYABLE(TimeEvent);

  Event     event;     ///< event to process
  SystemTime time;     ///< time when event is due

  constexpr
  TimeEvent(const Event& event, SystemTime time) :
    event(event), time(time)
  { }

  constexpr
  TimeEvent(const EventClass* event_class, EventArg param, EventMethod method, SystemTime time) :
    event(event_class, param, method), time(time)
  { }
};

////////////////////////////////////////////////////////////////////////////////
//     EventQueue
////////////////////////////////////////////////////////////////////////////////
/// Event processing queue
class EventQueue {
public:
  typedef bool (*EventHandler)(EventId id, EventArg data);
  typedef void (*TickHandler)();

  EventQueue();

  /// Start the event processing loop
  /**
   * \param handler Wenn keine Funktion in Event expliiz angegeben wurde, dann
   *                Event von dieser Funktionen verarbeiten lassen.
   * \param tick Funktion, die jedes mal aufgerufen wird, bevor Event verabeitet
   *             wird oder mindestens in \ref max_tick_time Abständen. Bei letztem ist
   *             der \a id Parameter EventQueue::event_null und der data Parameter 0.
   */
  void main(EventHandler handler, TickHandler tick);

  /// Event direkt ohne Pufferung verarbeiten
  /// \param id Event-Id von Event
  /// \param param Argument von Event, sonst Null
  /// \param callback explizite Angabe der Eventverarbeitungsfunktion, sonst \a nullptr
  bool processEvent(EventId id, EventArg param, EventMethod callback);

  /// Push quit event (\a EventQueue::event_quit) to front of event queue.
  /**
   * When event_quit is processed, EventQueue::main will return.
   * Only useful outside of embedded system.
   */
  _always_inline void quit() {
    pushToFront<event_quit>(0);
  }

  /// Push an new event to the event processing loop
  /**
   * \param event_class Event-Klasse aus dem Event erstellt werden soll.
   * \param params pointer to data for extra information and/or further processing
   * \param method function for processing event or nullptr. When nullptr is passed,
   *               event is given to the main action event function.
   */
  void push(const EventClass* event_class, EventArg params = 0, EventMethod method = nullptr);

  template<typename T, REQUIRES(!std::is_integral<T>)> _always_inline
  void push(const EventClass* event_class, T param, EventMethod method = nullptr) {
    push(event_class, pack<EventArg>(param), method);
  }

  // for compatibility with old code
  template<EventId id>
  void push(EventArg param = 0, EventMethod method = nullptr) {
    push(&UnnamedEventClass<id>::event_class, param, method);
  }
  // for compatibility with old code
  template<EventId id, typename T, REQUIRES(!std::is_integral<T>)> _always_inline
  void push(T param, EventMethod method = nullptr) {
    push(&UnnamedEventClass<id>::event_class, pack<EventArg>(param), method);
  }

  /// Push an new event to the front of the event processing loop
  /** Paramters the same as in push */
  void pushToFront(const EventClass* event_class, EventArg param = 0, EventMethod method = nullptr);

  template<typename T, REQUIRES(!std::is_integral<T>)> _always_inline
  void pushToFront(const EventClass* event_class, T param, EventMethod method = nullptr) {
    pushToFront(event_class, pack<EventArg>(param), method);
  }

  // for compatibility with old code
  template<EventId id>
  void pushToFront(EventArg param = 0, EventMethod method = nullptr) {
    pushToFront(&UnnamedEventClass<id>::event_class, param, method);
  }

  // for compatibility with old code
  template<EventId id, typename T, REQUIRES(!std::is_integral<T>)> _always_inline
  void pushToFront(T param, EventMethod method = nullptr) {
    pushToFront(&UnnamedEventClass<id>::event_class, pack<EventArg>(param), method);
  }

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

  template<typename T, REQUIRES(!std::is_integral<T>)> _always_inline
  void pushTimedelayed(SystemTime ticks, const EventClass* event_class, T param, EventMethod method = nullptr) {
    pushTimedelayed(ticks, event_class, pack<EventArg>(param), method);
  }

  // for compatibility with old code
  template<EventId id>
  void pushTimedelayed(SystemTime ticks, EventArg param = 0, EventMethod method = nullptr) {
    pushTimedelayed(ticks, &UnnamedEventClass<id>::event_class, param, method);
  }

  // for compatibility with old code
  template<EventId id, typename T, REQUIRES(!std::is_integral<T>)> _always_inline
  void pushTimedelayed(SystemTime ticks, T param, EventMethod method = nullptr) {
    pushTimedelayed(ticks, &UnnamedEventClass<id>::event_class, pack<EventArg>(param), method);
  }

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

  /// Remove all events with specific callback.
  /**
   * \param method event function that is searched
   */
  void removeCallback(EventMethod method);

  /// Clear event queue.
  void clear();

  /// Clear event queue for normal events (not timedelayed events).
  void discardEvents();

#ifndef NDEBUG
  /// Print debug information for the event queue.
  void printTimeQueue();
  void printQueue();
  void printDebugInfo();
#endif

  /// Number of the maximum events + 1 (must be a power of two)
  static const size_t size = 1 << 5; // 32
  static_assert(!(size & (size-1)), "EventQueue::size must be a power of 2");

  enum {
    event_null = 0, ///< \a EventId for do nothing
    event_nonamed,  ///< \a EventId for do nothing
    event_idle,     ///< \a EventId when nothing to do
    event_watchdog  ///< \a EventId when watchdog get awake
  };

  /// \a EventId for quit the event processing loop
  static const EventId event_quit = -1;

  /// Number of the maximum time events
  static const size_t timequeue_size = 16;

  /// Maximum time between two calls of tick function
  static constexpr SystemTime max_tick_time = ms_to_ticks(10);

private:
  mutable Mutex mutex_;
  ConditionVariable var_;

  // Circular buffer for events
  CircularBuffer<Event, EventQueue::size> queue_;

  ArrayBuffer<TimeEvent, EventQueue::timequeue_size> timequeue_;

  EventHandler handler_;

  bool loadEvent(Event* event);
  SystemTime getTimeToNextEvent() const;
};

/// \}
/// \}

} // namespace TURAG

#endif // TINAPP_STATEMACHINE_EVENTQUEUE_H


