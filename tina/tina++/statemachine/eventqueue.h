/**
 * \file eventqueue.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief EventQueue definition
 *
 */

#ifndef TINAPP_STATEMACHINE_EVENTQUEUE_H
#define TINAPP_STATEMACHINE_EVENTQUEUE_H

#include <type_traits>

#include <tina++/time.h>
#include <tina++/helper/packed.h>
#include "types.h"

namespace TURAG {

class Action;

//#define EVENTQUEUE_USAGE_MEASUREMENT

////////////////////////////////////////////////////////////////////////////////
//     Event
////////////////////////////////////////////////////////////////////////////////

/// Event
struct Event {
  EventId      id;        ///< event id
  pointer      params;    ///< parameters
  EventMethod  method;    ///< event method for async method calls or \a nullptr

  constexpr
  Event(EventId id, pointer params, EventMethod method) :
    id(id), params(params), method(method)
  { }
};

/// Print debug information for given event
void print_debug_info(const Event& e);

/// Event with time information for timedelayed events
struct TimeEvent {
  Event      event;     ///< event to process
  SystemTime time;      ///< time when event is due

  constexpr
  TimeEvent(const Event& event, SystemTime time) :
    event(event), time(time)
  { }

  constexpr
  TimeEvent(EventId id, pointer params, EventMethod method, SystemTime time) :
    event(id, params, method), time(time)
  { }
};

////////////////////////////////////////////////////////////////////////////////
//     EventQueue
////////////////////////////////////////////////////////////////////////////////
/// Event processing queue
class EventQueue {
public:
  /// Start the event processing loop
  /**
   * \param mainaction Main action that is started, when event queue starts.
   *                   Events with no callback (method == nullptr) are passed to this
   *                   action.
   * \param tick Event function that is executed everytime a new event is
   *             searched. The found event is passed to the function. Is the
   *             event queue empty, EventQueue::event_null as id and nullptr as
   *             data parameter is passed as functon parameters.
   * \param idle Event function that is executed, when the event queue is empty.
   *             EventQueue::event_null as id and nullptr as data parameter is
   *             passed as function parameters.
   */
  static void main(Action& mainaction, EventMethod tick, EventMethod idle);

  static bool processEvent(EventId id, pointer params, EventMethod callback);

  /// Push quit event (\a EventQueue::event_quit) to front of event queue.
  /**
   * When event_quit is processed, EventQueue::main will return.
   * Only useful outside of embedded system.
   */
  static _always_inline void quit() {
    pushToFront(event_quit);
  }

  /// Push an new event to the event processing loop
  /**
   * \param id     event id SHOULD NOT be event_null otherwise event will be ignored
   * \param params pointer to data for extra information and/or further processing
   * \param method function for processing event or nullptr. When nullptr is passed,
   *               event is given to the main action event function.
   */
  static void push(EventId id, pointer params, EventMethod method = nullptr);

  template<typename T, REQUIRES(!std::is_pointer<T>)> _always_inline
  static void push(EventId id, T param, EventMethod method = nullptr) {
    push(id, pack<void*>(param), method);
  }

  /// Push an new event to the event processing loop
  /**
   * \param id     event id SHOULD NOT be event_null otherwise event will be ignored
   * \param method function for processing event or nullptr. When nullptr is passed,
   *               event is given to the main action event function.
   */
  static _always_inline void push(EventId id, EventMethod method = nullptr) {
    push(id, nullptr, method);
  }

  /// Push an new event to the front of the event processing loop
  /** Paramters the same as in push */
  static void pushToFront(EventId id, pointer params, EventMethod method = nullptr);

  template<typename T, REQUIRES(!std::is_pointer<T>)> _always_inline
  static void pushToFront(EventId id, T param, EventMethod method = nullptr) {
    pushToFront(id, pack<void*>(param), method);
  }

  /// Push an new event to the front of the event processing loop
  /** Paramters the same as in push */
  static _always_inline void pushToFront(EventId id, EventMethod method = nullptr) {
    pushToFront(id, nullptr, method);
  }

  /// Process event in a number of kernel ticks
  /**
   * NOTE: timedelayed events are more privileged as normal events.
   * \param ticks  time from now in ecos ticks to process event
   * \param id     event id SHOULD NOT be event_null otherwise event will be ignored
   * \param param  pointer to data for extra information and/or further processing
   * \param method function for processing event or nullptr. When nullptr is passed,
   *               event is given to the main action event function.
   */
  static void pushTimedelayed(SystemTime ticks, EventId id, pointer param, EventMethod method = nullptr);

  template<typename T, REQUIRES(!std::is_pointer<T>)> _always_inline
  static void pushTimedelayed(SystemTime ticks, EventId id, T param, EventMethod method = nullptr) {
    pushTimedelayed(ticks, id, pack<void*>(param), method);
  }

  /// Process event in a number of kernel ticks
  /**
   * NOTE: timedelayed events are more privileged as normal events.
   * \param ticks  time from now in ecos ticks to process event
   * \param id     event id SHOULD NOT be event_null otherwise event will be ignored
   * \param method function for processing event or nullptr. When nullptr is passed,
   *               event is given to the main action event function.
   */
  static _always_inline void pushTimedelayed(SystemTime ticks, EventId id, EventMethod method = nullptr) {
    pushTimedelayed(ticks, id, nullptr, method);
  }

  /// Remove all events from timedelayed event queue with that id.
  /**
   * \param id event id that is searched
   */
  static void removeTimedelayed(EventId id);

  /// Remove all events from event queue with that id.
  /**
   * \param id event id that is searched
   */
  static void removeEvent(EventId id);

  /// Remove all events with specific callback.
  /**
   * \param method event function that is searched
   */
  static void removeCallback(EventMethod method);

  /// Clear event queue.
  static void clear();

  /// Clear event queue for normal events (not timedelayed events).
  static void discardEvents();

#ifndef NDEBUG
  /// Print debug information for the event queue.
  static void printTimeQueue();
  static void printQueue();
  static void printDebugInfo();
#endif

  /// Number of the maximum events + 1 (must be a power of two)
  static const size_t size = 1 << 5; // 32
  static_assert(!(size & (size-1)), "EventQueue::size must be a power of 2");

  /// Number of the maximum time events
  static const size_t timequeue_size = 16;

  enum {
    event_null = 0, ///< \a EventId for do nothing
    event_nonamed,  ///< \a EventId for do nothing
    event_idle,     ///< \a EventId when nothing to do
    event_watchdog  ///< \a EventId when watchdog get awake
  };

  /// \a EventId for quit the event processing loop
  static const EventId event_quit = -1;
};

} // namespace TURAG

#endif // TINAPP_STATEMACHINE_EVENTQUEUE_H


