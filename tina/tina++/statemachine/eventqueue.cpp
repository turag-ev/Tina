/*
 * eventqueue.c
 *
 *  Created on: 15.12.2011
 *      Author: richard
 */

#define LOG_SOURCE "E"
//#define DEBUG_LEVEL 3

#include "../debug.h"

#include <algorithm>
#include <tina++/thread.h>
#include <tina++/time.h>
#include <tina/time.h>
#include "../container/array_buffer.h"
#include "../container/circular_buffer.h"
#include "../algorithm.h"
#include "eventqueue.h"
#include "state.h"

#ifdef NDEBUG
# undef EVENTQUEUE_USAGE_MEASUREMENT
#endif

namespace TURAG {

struct EventQueuePrivate {
  // Circular buffer for events
  CircularBuffer<Event, EventQueue::size> queue;

  // Mutex of the event queue
  Mutex mutex;

  // array buffer implementation for time event queue
  ArrayBuffer<TimeEvent, EventQueue::timequeue_size> timequeue;

  // Mutex of the time events
  Mutex timemutex;

  // Zeiger auf Hauptaktion
  Action* main_action;

#ifdef EVENTQUEUE_USAGE_MEASUREMENT
  size_t max_events;
  size_t max_timed_events;
#endif

  EventQueuePrivate() :
    mutex(),
    timequeue(), timemutex(),
    main_action(nullptr)
#ifdef EVENTQUEUE_USAGE_MEASUREMENT
    ,max_events(0),
    max_timed_events(0)
#endif
  { }
};

static EventQueuePrivate p;

_hot
void print_debug_info(const Event &e) {
  if (e.id >> 24 != 0) {
    infof("Event: %c%c%c%u 0x%zx",
          static_cast<char>(e.id >> 24),
          static_cast<char>(e.id >> 16),
          static_cast<char>(e.id >> 8),
          static_cast<unsigned>(e.id & 0xFF),
          reinterpret_cast<ptrdiff_t>(e.method));
  } else {
    infof("Event: %u 0x%zx",
          static_cast<unsigned>(e.id & 0xFF),
          reinterpret_cast<ptrdiff_t>(e.method));
  }
}

static _hot bool
eq_pop(Event* event) {
  Mutex::Lock lock(p.mutex);

  if (!p.queue.empty()) {
    *event = p.queue.front();
    p.queue.pop_front();
    return true;
  } else {
    return false;
  }
}

static _hot bool
eq_pop_timedelayed(Event* event) {
  Mutex::Lock lock(p.timemutex);

  if (p.timequeue.empty())
    return false;

  SystemTime t = get_current_tick();
  TimeEvent& first = p.timequeue.back();
  if (first.time <= t) {
    p.timequeue.pop_back();
    if (first.event.id != EventQueue::event_null) {
      *event = first.event;
      return true;
    }
  }

  return false;
}

void
EventQueue::main(Action& mainaction, EventMethod tick, EventMethod idle) {
  Event event(event_null, nullptr, nullptr);

  p.main_action = &mainaction;
  mainaction.start(nullptr);

  while (true) {
    // load next event
    if (!eq_pop_timedelayed(&event) && !eq_pop(&event)) {
      event.id = EventQueue::event_null;
    }

    tick(event.id, event.params);
    if (event.id == event_null) {
      idle(event_null, nullptr);
      continue;
    }

    if (event.id == event_quit)
      break;

    print_debug_info(event);

    if (event.method != nullptr) {
      (*event.method)(event.id, event.params);
    } else {
      mainaction.func(event.id, event.params);
    }
  }
}

bool
EventQueue::processEvent(EventId id, pointer params, EventMethod callback) {
  if (p.main_action != nullptr) {
    if (callback != nullptr) {
      (*callback)(id, params);
      return false;

    } else {
      return p.main_action->func(id, params);
    }
  }

  return false;
}

void
EventQueue::push(EventId id, pointer params, EventMethod method) {
  Mutex::Lock lock(p.mutex);
  p.queue.emplace_back(id, params, method);
}

void
EventQueue::pushToFront(EventId id, pointer params, EventMethod method) {
  Mutex::Lock lock(p.mutex);
  p.queue.emplace_front(id, params, method);
}

void
EventQueue::pushTimedelayed(SystemTime ticks, EventId id, pointer params,
                            EventMethod method)
{
  Mutex::Lock lock(p.timemutex);

  SystemTime t = get_current_tick() + ticks;
  // search for item to insert after
  auto iter = p.timequeue.rbegin();
  while (iter != p.timequeue.rend() && t > iter->time) {
    iter++;
  }
  /*if (iter == p.timequeue.rend())*/ iter--;
  p.timequeue.emplace(make_forward(iter), Event(id, params, method), t);

#ifdef EVENTQUEUE_USAGE_MEASUREMENT
  if (p.timequeue.size() > p.max_timed_events) {
    p.max_timed_events = p.timequeue.size();
  }
#endif
}

struct is_id {
  constexpr explicit
  is_id(EventId id) :
    value(id)
  { }

  constexpr
  bool operator() (TimeEvent& event) const {
    return event.event.id == value;
  }

  EventId value;
};

void
EventQueue::removeTimedelayed(EventId id) {
  Mutex::Lock lock(p.timemutex);
  remove_if(p.timequeue, is_id(id));
}

void
EventQueue::removeEvent(EventId id) {
  Mutex::Lock lock(p.mutex);

  for (auto& event : p.queue) {
    if (event.id == id) event.id = event_null;
  }
}

struct is_callback {
  constexpr explicit
  is_callback(EventMethod method) :
    value(method)
  { }

  constexpr
  bool operator() (TimeEvent& event) const {
    return event.event.method == value;
  }

  EventMethod value;
};

void
EventQueue::removeCallback(EventMethod method) {
//  { TODO
//    scoped_lock lock(self.mutex_);
//
//
//  }

  {
    Mutex::Lock lock(p.timemutex);
    remove_if(p.timequeue, is_callback(method));
  }
}

void
EventQueue::clear() {
  Mutex::Lock lock1(p.mutex);
  Mutex::Lock lock2(p.timemutex);

  p.queue.clear();
  p.timequeue.clear();
}

void EventQueue::discardEvents() {
  Mutex::Lock lock(p.mutex);
  p.queue.clear();
}


void
EventQueue::printTimeQueue() {
  Mutex::Lock lock(p.timemutex);

  infof("Timed Events:\n");
  infof("  current time: %u ms\n", ticks_to_ms(get_current_tick()));
  if (!p.timequeue.empty()) {
    for (auto i = p.timequeue.begin(); i != p.timequeue.end(); i++) {
      infof("  @%u ms %c%c%c%u %p\n",
            ticks_to_ms(i->time),
            static_cast<char>(i->event.id >> 24),
            static_cast<char>(i->event.id >> 16),
            static_cast<char>(i->event.id >> 8),
            static_cast<unsigned>(i->event.id & 0xFF),
            i->event.method);
    }
  } else {
    info("  timed event queue is empty\n");
  }
}

void
EventQueue::printQueue() {
  Mutex::Lock lock(p.mutex);

  info("EventQueue Debug Information:");
  info(" - Events:");
  if (!p.queue.empty()) {
    for (auto i = p.queue.begin(); i != p.queue.end(); i++) {
      print_debug_info(*i);
    }
  } else {
    info("  event queue is empty");
  }
}

void
EventQueue::printDebugInfo() {
  printTimeQueue();
  printQueue();

  info ("Stack:\n");
  infof("  size of EventQueue: %zu bytes\n", sizeof(EventQueuePrivate));
  infof("  number of queue elements: %zu\n", size);
  infof("  number of timed queue elements: %zu\n", timequeue_size);
#ifdef EVENTQUEUE_USAGE_MEASUREMENT
  info ("Usage:\n");
  infof("  max. number of queue elements: %zu\n", p.max_events);
  infof("  max. number of timed queue elements: %zu\n", p.max_timed_events);
#endif
}

// C Schnittstelle

extern "C"
void turag_eventqueue_push(EventId id, pointer params, EventMethod method) {
  EventQueue::push(id, params, method);
}

extern "C"
void turag_eventqueue_push_to_front(EventId id, pointer params, EventMethod method) {
  EventQueue::pushToFront(id, params, method);
}

extern "C"
void turag_eventqueue_push_timedelayed(TuragSystemTime ticks, EventId id, pointer params, EventMethod method) {
  EventQueue::pushTimedelayed(SystemTime{ticks.value}, id, params, method);
}

} // namespace TURAG
