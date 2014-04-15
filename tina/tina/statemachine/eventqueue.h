#ifndef TINA_STATEMACHINE_EVENTQUEUE_H
#define TINA_STATEMACHINE_EVENTQUEUE_H

#include <tina/time.h>

/// \defgroup EventQueue Events [C]
/// \brief C-Schnittstelle für die \ref TURAG::EventQueue
/// \{

#ifdef __cplusplus
extern "C" {
#endif

/// Typ für Event-Ids (gleichzeitig Standardtyp für C-Enums)
typedef int32_t TuragEventId;

/// Typ für Parameter von Events
typedef int32_t TuragEventArg;

/// Typ für Funktionen die Events verarbeiten
typedef void (*TuragEventMethod)(TuragEventId id, TuragEventArg data);

/// Makroversion von \ref TURAG::EventNamespace für C-Quellcode
#define TURAG_EVENT_NAMESPACE(a,b,c) (((a) << 24) | ((b) << 16) | ((c) << 8))

/// C-Typ für \ref TURAG::EventClass
typedef struct {
  const char* name;
  TuragEventId id;
} TuragEventClass;

/// C-Typ für \ref TURAG::Event
typedef struct {
  TuragEventId      id;        ///< event id
  TuragEventArg     params;    ///< parameters
  TuragEventMethod  method;    ///< event method for async method calls or \a nullptr
} TuragEvent;

/// C-Typ für \ref TURAG::TimeEvent
typedef struct {
  TuragEvent    event;     ///< event to process
  TuragSystemTime time;      ///< time when event is due
} TuragTimeEvent;

#ifndef DOXYGEN
struct _TuragEventQueue;
#endif

/// C-Dummy-Typ für Event-Schlangen
typedef struct _TuragEventQueue TuragEventQueue;

////////////////////////////////////////////////////////////////////////////////
//     EventQueue
////////////////////////////////////////////////////////////////////////////////
/// Push an new event to the event processing loop
void turag_eventqueue_push(TuragEventQueue* queue,
                           const TuragEventClass* event_class, TuragEventArg params,
                           TuragEventMethod method);

/// Push an new event to the front of the event processing loop
void turag_eventqueue_push_to_front(TuragEventQueue* queue,
                                    const TuragEventClass* event_class,
                                    TuragEventArg params, TuragEventMethod method);

/// Process event in a number of kernel ticks
void turag_eventqueue_push_timedelayed(TuragEventQueue* queue,
                                       TuragSystemTime ticks,
                                       const TuragEventClass* event_class,
                                       TuragEventArg param, TuragEventMethod method);

enum {
  turag_eventqueue_event_quit = -1, ///< \a EventId für Event-Schlangenverarbeitung beenden (nur in Desktopbereich verwenden)
  turag_eventqueue_event_null = 0, ///< \a EventId for do nothing
  turag_eventqueue_event_nonamed,  ///< \a EventId for do nothing
  turag_eventqueue_event_idle,     ///< \a EventId when nothing to do
  turag_eventqueue_event_watchdog  ///< \a EventId when watchdog get awake
};

#ifdef __cplusplus
} // extern "C"
#endif

/// \}

#endif // TINA_STATEMACHINE_EVENTQUEUE_H
