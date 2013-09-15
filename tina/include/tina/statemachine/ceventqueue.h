#ifndef CEVENTQUEUE_H
#define CEVENTQUEUE_H

#include <utils/timetype.h>

#ifdef __cplusplus
extern "C" {
#endif

/// type for event ids
typedef uint32_t SCEventId;

/// function for event processing
typedef void (*SCEventMethod)(SCEventId id, void* data);

/// create a unsigned integer with the first three bytes filled with characters
/// to build a namespace. The last byte can be used for any value >= 0 and < 256.
/// example:
/// \code enum { event_x = SCnamespace('X', 'X', 'X') + 100 }; \endcode
#define SC_NAMESPACE(a,b,c) (((a) << 24) | ((b) << 16) | ((c) << 8))

/// Event
typedef struct {
  SCEventId      id;        ///< event id
  void*      params;    ///< parameters
  SCEventMethod  method;    ///< event method for async method calls or \a nullptr
} SCEvent;

/// Event with time information for timedelayed events
typedef struct {
  SCEvent    event;     ///< event to process
  SystemTicks time;      ///< time when event is due
} SCTimeEvent;

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
void sc_eventqueue_push(SCEventId id, void* params, SCEventMethod method);

/// Push an new event to the front of the event processing loop
/** Paramters the same as in push */
void sc_eventqueue_push_to_front(SCEventId id, void* params, SCEventMethod method);

/// Process event in a number of kernel ticks
/**
 * NOTE: timedelayed events are more privileged as normal events.
 * \param ticks  time from now in ecos ticks to process event
 * \param id     event id SHOULD NOT be event_null otherwise event will be ignored
 * \param params pointer to data for extra information and/or further processing
 * \param method function for processing event or nullptr. When nullptr is passed,
 *               event is given to the main action event function.
 */
void sc_eventqueue_push_timedelayed(SystemTicks ticks, SCEventId id, void* params, SCEventMethod method);

enum {
  sc_eventqueue_event_quit = -1,
  sc_eventqueue_event_null = 0, ///< \a EventId for do nothing
  sc_eventqueue_event_nonamed,  ///< \a EventId for do nothing
  sc_eventqueue_event_idle,     ///< \a EventId when nothing to do
  sc_eventqueue_event_watchdog  ///< \a EventId when watchdog get awake
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CEVENTQUEUE_H
