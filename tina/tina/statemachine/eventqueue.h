#ifndef TINA_STATEMACHINE_EVENTQUEUE_H
#define TINA_STATEMACHINE_EVENTQUEUE_H

#include <tina/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/// type for event ids
typedef int32_t TuragEventId;

typedef int32_t TuragEventArg;

/// function for event processing
typedef void (*TuragEventMethod)(TuragEventId id, TuragEventArg data);

/// create a unsigned integer with the first three bytes filled with characters
/// to build a namespace. The last byte can be used for any value >= 0 and < 256.
/// example:
/// \code enum { event_x = TURAG_EVENT_NAMESPACE('X', 'X', 'X') + 100 }; \endcode
#define TURAG_EVENT_NAMESPACE(a,b,c) (((a) << 24) | ((b) << 16) | ((c) << 8))

typedef struct {
  const char* name;
  TuragEventId id;
} TuragEventClass;

/// Event
typedef struct {
  TuragEventId      id;        ///< event id
  TuragEventArg     params;    ///< parameters
  TuragEventMethod  method;    ///< event method for async method calls or \a nullptr
} TuragEvent;

/// Event with time information for timedelayed events
typedef struct {
  TuragEvent    event;     ///< event to process
  TuragSystemTime time;      ///< time when event is due
} TuragTimeEvent;

struct _TuragEventQueue;
typedef struct _TuragEventQueue TuragEventQueue;

////////////////////////////////////////////////////////////////////////////////
//     EventQueue
////////////////////////////////////////////////////////////////////////////////
/// Push an new event to the event processing loop
/**
 * \param id     event id SHOULD NOT be event_null otherwise event will be ignored
 * \param params pointer to data for extra information and/or further processing
 * \param method function for processing event or nullptr. When nullptr is passed,
 *               event is given to the main action event function.
 */
void turag_eventqueue_push(TuragEventQueue* queue,
                           const TuragEventClass* event_class, TuragEventArg params,
                           TuragEventMethod method);

/// Push an new event to the front of the event processing loop
/** Paramters the same as in push */
void turag_eventqueue_push_to_front(TuragEventQueue* queue,
                                    const TuragEventClass* event_class,
                                    TuragEventArg params, TuragEventMethod method);

/// Process event in a number of kernel ticks
/**
 * NOTE: timedelayed events are more privileged as normal events.
 * \param ticks  time from now in ecos ticks to process event
 * \param id     event id SHOULD NOT be event_null otherwise event will be ignored
 * \param params pointer to data for extra information and/or further processing
 * \param method function for processing event or nullptr. When nullptr is passed,
 *               event is given to the main action event function.
 */
void turag_eventqueue_push_timedelayed(TuragEventQueue* queue,
                                       TuragSystemTime ticks,
                                       const TuragEventClass* event_class,
                                       TuragEventArg param, TuragEventMethod method);

enum {
  turag_eventqueue_event_quit = -1,
  turag_eventqueue_event_null = 0, ///< \a EventId for do nothing
  turag_eventqueue_event_nonamed,  ///< \a EventId for do nothing
  turag_eventqueue_event_idle,     ///< \a EventId when nothing to do
  turag_eventqueue_event_watchdog  ///< \a EventId when watchdog get awake
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_STATEMACHINE_EVENTQUEUE_H
