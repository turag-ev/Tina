/*
 * eventqueue.c
 *
 *  Created on: 15.12.2011
 *      Author: richard
 */

#define TURAG_DEBUG_LOG_SOURCE "E"
//#define DEBUG_LEVEL 3
#define __STDC_FORMAT_MACROS

#include "../debug.h"

#include <algorithm>
#include <tina++/thread.h>
#include <tina++/time.h>
#include <tina/time.h>
#include <inttypes.h>
#include <tina/statemachine/eventqueue.h>
#include "../container/array_buffer.h"
#include "../container/circular_buffer.h"
#include "../range/algorithm.h"
#include "eventqueue.h"
#include "state.h"

#ifdef NDEBUG
# undef EVENTQUEUE_USAGE_MEASUREMENT
#endif

namespace TURAG {

EventQueue::EventQueue() :
  mutex_(),
  var_(&mutex_),

  queue_(),
  handler_(nullptr)
{ }

constexpr SystemTime EventQueue::max_tick_time;

const EventClass EventNull("kein Event", EventQueue::event_null);

static _hot
void print_debug_info(const Event& e) {
#if TURAG_DEBUG_LEVEL > 3
  EventId id = e.event_class->id;
  const char* name = (e.event_class->name) ? e.event_class->name : "";

  if ((id >> 8) != 0) {
    infof("Event: %s (id: %c%c%c%u param: %" PRIu32 " method: 0x%zx)",
          name,
          static_cast<char>(id >> 24),
          static_cast<char>(id >> 16),
          static_cast<char>(id >> 8),
          static_cast<unsigned>(id & 0xFF),
          e.param,
          reinterpret_cast<ptrdiff_t>(e.method));
  } else {
    infof("Event: %s (id: %u param: %" PRIu32 " method: 0x%zx)",
          name,
          static_cast<unsigned>(id),
          e.param,
          reinterpret_cast<ptrdiff_t>(e.method));
  }
#endif
}

_hot
bool EventQueue::loadEvent(Event* event) {
  if (!timequeue_.empty()) {
    SystemTime t = get_current_tick();
    const TimeEvent& first = timequeue_.back();
    if (first.time <= t) {
      if (first.event.event_class->id != EventQueue::event_null) {
        *event = first.event;
        timequeue_.pop_back();
        return true;
      }
      timequeue_.pop_back();
    }
  }

  if (!queue_.empty()) {
    // lade nächstes Event
    *event = queue_.front();
    queue_.pop_front();
    return true;
  }

  return false;
}

SystemTime EventQueue::getTimeToNextEvent() const {
  if (!queue_.empty()) {
      return SystemTime(0);
  }

  if (!timequeue_.empty()) {
    const TimeEvent& first = timequeue_.back();
    int diff = first.time.value - get_current_tick().value;
    if (diff <= 0) {         
      return SystemTime(0);
    } else {
      return SystemTime(std::min(TuragSystemTicks(diff), max_tick_time.value));
    }
  }

  return max_tick_time;
}

#ifdef TURAG_STATEMACHINE_FOREVER
_noreturn
#endif
void EventQueue::main(EventHandler handler, TickHandler tick) {
  Event event(&EventNull, 0, nullptr);

  handler_ = handler;

  while (true) {      
      Mutex::Lock lock(mutex_);

      while (!loadEvent(&event)) {
          SystemTime wait_time = getTimeToNextEvent();
          if (wait_time.value == 0) continue;

          // warte auf Event
          var_.waitFor(wait_time);

          lock.unlock();
          tick();
          lock.lock();
      }

      lock.unlock();

#ifndef TURAG_STATEMACHINE_FOREVER
      if (event.event_class->id == event_quit)
          break;
#endif

      print_debug_info(event);

      if (event.method != nullptr) {
          (*event.method)(event.event_class->id, event.param);
      } else {
          handler_(event.event_class->id, event.param);
      }

  }
}

bool EventQueue::processEvent(EventId id, EventArg param, EventMethod callback) {
  if (handler_ != nullptr) {
    if (callback != nullptr) {
      (*callback)(id, param);
      return false;

    } else {
      return handler_(id, param);
    }
  }

  return false;
}

void EventQueue::push(const EventClass* event_class, EventArg param, EventMethod method) {
  Mutex::Lock lock(mutex_);
  queue_.emplace_back(event_class, param, method);
  var_.signal();
}

void EventQueue::pushToFront(const EventClass* event_class, EventArg param, EventMethod method) {
  Mutex::Lock lock(mutex_);
  queue_.emplace_front(event_class, param, method);
  var_.signal();
}

void EventQueue::pushTimedelayed(SystemTime ticks, const EventClass* event_class,
                                  EventArg param, EventMethod method)
{
  SystemTime t = get_current_tick() + ticks;
  Mutex::Lock lock(mutex_);

  // search for item to insert after
  auto iter = timequeue_.rbegin();
  while (iter != timequeue_.rend() && t > iter->time) {
    iter++;
  }
  /*if (iter == p.timequeue.rend())*/ iter--;
  timequeue_.emplace(make_forward(iter), Event(event_class, param, method), t);
#ifdef EVENTQUEUE_USAGE_MEASUREMENT
  if (timequeue_.size() > p.max_timed_events) {
    p.max_timed_events = timequeue_.size();
  }
#endif

  var_.signal();
}

void EventQueue::clear() {
  Mutex::Lock lock(mutex_);
  queue_.clear();
  timequeue_.clear();
}

void EventQueue::discardEvents() {
  Mutex::Lock lock(mutex_);
  queue_.clear();
}

void EventQueue::removeTimedelayed(EventId id) {
  Mutex::Lock lock(mutex_);

  remove_if(timequeue_, [&](const TimeEvent& tevent) { return tevent.event.event_class->id == id;});
}

void EventQueue::removeEvent(EventId id) {
  Mutex::Lock lock(mutex_);

  for (auto& event : queue_) {
    if (event.event_class->id == id) event.event_class = &EventNull;
  }

  remove_if(timequeue_, [&](const TimeEvent& tevent) { return tevent.event.event_class->id == id;});
}

void EventQueue::removeCallback(EventMethod method) {
  Mutex::Lock lock(mutex_);

  for (auto& event : queue_) {
    if (event.method == method) event.event_class = &EventNull;
  }

  remove_if(timequeue_, [&](const TimeEvent& tevent) { return tevent.event.method == method;});
}

#ifndef NDEBUG
void EventQueue::printTimeQueue() {
  Mutex::Lock lock(mutex_);

  infof("Timed Events:\n");
  infof("  current time: %u ms\n", ticks_to_ms(get_current_tick()));

  if (!timequeue_.empty()) {
    for (const auto& tevent : timequeue_) {
      EventId id = tevent.event.event_class->id;

      infof("  @%u ms %c%c%c%u %zx\n",
            ticks_to_ms(tevent.time),
            static_cast<char>(id >> 24),
            static_cast<char>(id >> 16),
            static_cast<char>(id >> 8),
            static_cast<unsigned>(id & 0xFF),
            reinterpret_cast<ptrdiff_t>(tevent.event.method));
    }
  } else {
    info("  timed event queue is empty\n");
  }
}

void
EventQueue::printQueue() {
  Mutex::Lock lock(mutex_);

  info("EventQueue Debug Information:");
  info(" - Events:");
  if (!queue_.empty()) {
    for (const auto& event : queue_) {
      print_debug_info(event);
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
  infof("  size of EventQueue: %zu bytes\n", sizeof(EventQueue));
  infof("  number of queue elements: %zu\n", size);
  infof("  number of timed queue elements: %zu\n", timequeue_size);
}
#endif // NDEBUG

// C Schnittstelle

extern "C"
void turag_eventqueue_push(TuragEventQueue* queue,
                           const TuragEventClass* event_class, TuragEventArg param,
                           TuragEventMethod method)
{
  auto q = reinterpret_cast<EventQueue*>(queue);
  q->push(reinterpret_cast<const EventClass*>(event_class), param, method);
}

extern "C"
void turag_eventqueue_push_to_front(TuragEventQueue* queue,
                                    const TuragEventClass* event_class,
                                    TuragEventArg param, TuragEventMethod method)
{
  auto q = reinterpret_cast<EventQueue*>(queue);
  q->pushToFront(reinterpret_cast<const EventClass*>(event_class), param,
                 method);
}

extern "C"
void turag_eventqueue_push_timedelayed(TuragEventQueue* queue,
                                       TuragSystemTime ticks,
                                       const TuragEventClass* event_class,
                                       TuragEventArg param, TuragEventMethod method)
{
  auto q = reinterpret_cast<EventQueue*>(queue);
  q->pushTimedelayed(ticks, reinterpret_cast<const EventClass*>(event_class),
                     param, method);
}

} // namespace TURAG
