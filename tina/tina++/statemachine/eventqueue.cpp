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

static _hot
void print_debug_info(const Event& e) {
#if TURAG_DEBUG_LEVEL > 3
  EventId id = e.event_class->id;
  const char* name = (e.event_class->name) ? e.event_class->name : "";

  if ((id >> 8) != 0) {
      // erstes Zeichen ist Null -> nicht loggen
      if ((id >> 24) == 0) return;

      turag_infof("Event: %s (id: %c%c%c%u param: %" PRIu32 " method: 0x%" PRIxPTR ")",
          name,
          static_cast<char>(id >> 24),
          static_cast<char>(id >> 16),
          static_cast<char>(id >> 8),
          static_cast<unsigned>(id & 0xFF),
          e.param,
          reinterpret_cast<std::size_t>(e.method));
  } else {
    turag_infof("Event: %s (id: %u param: %" PRIu32 " method: 0x%" PRIxPTR ")",
          name,
          static_cast<unsigned>(id),
          e.param,
          reinterpret_cast<std::size_t>(e.method));
  }
#endif
}

_hot
bool EventQueue::loadEvent(Event* event) {
  if (!timequeue_.empty()) {
    SystemTime t = SystemTime::now();
    const TimeEvent& first = timequeue_.back();
    if (first.time <= t) {
      if (first.event.event_class != nullptr)
      {
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
    return event->event_class != nullptr;
  }

  return false;
}

SystemTime EventQueue::getTimeToNextEvent() const {
  if (!queue_.empty()) {
      return SystemTime(0);
  }

  if (!timequeue_.empty()) {
    const TimeEvent& first = timequeue_.back();
    int diff = first.time.toTicks() - SystemTime::now().toTicks();
    if (diff <= 0) {         
      return SystemTime(0);
    } else {
        return SystemTime(std::min(TuragSystemTicks(diff), max_tick_time.toTicks()));
    }
  }

  return max_tick_time;
}

void EventQueue::main(EventHandler handler, TickHandler tick) {
  Event event(nullptr, 0, nullptr);

  handler_ = handler;

  // execute tick() once. This is to avoid the case where the eventqueue is started
  // with events pending in the queue - the would be executed before tick() ran
  // for the first time.
  tick();

  while (true) {      

      // this block introduces the thread delay. If there are pending events,
      // the are handled immediately. If there are only time-delayed events,
      // we will wait for this event, execute tick(), and handle this event.
      // Anyway, we will wait at most for max_tick_time. We also wait for this period
      // of time if there is currently no event.
      // If an event happens to arrive while waiting, we execute tick() and handle
      // the event without any further delay.
      while (true) {
          ConditionVariable::Lock lock(var_);
          if (loadEvent(&event)) break;
          SystemTime wait_time = getTimeToNextEvent();
          if (wait_time.toTicks() == 0) continue;

          // warte auf Event
          lock.waitFor(wait_time);

          lock.unlock();
          tick();
      }

      if (event.event_class->id == event_quit) {
          break;
      }

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
  SystemTime t = SystemTime::now() + ticks;
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

  remove_if(timequeue_, [&](const TimeEvent& tevent) {
      return tevent.event.event_class && tevent.event.event_class->id == id;
  });
}

void EventQueue::removeEvent(EventId id) {
  Mutex::Lock lock(mutex_);

  for (auto& event : queue_) {
    if (event.event_class && event.event_class->id == id)
        event.event_class = nullptr;
  }

  remove_if(timequeue_, [&](const TimeEvent& tevent) {
      return tevent.event.event_class && tevent.event.event_class->id == id;
  });
}

void EventQueue::removeCallback(EventMethod method) {
  Mutex::Lock lock(mutex_);

  for (auto& event : queue_) {
    if (event.method == method) event.event_class = nullptr;
  }

  remove_if(timequeue_, [&](const TimeEvent& tevent) { return tevent.event.method == method;});
}

#ifndef NDEBUG
void EventQueue::printTimeQueue() {
    Mutex::Lock lock(mutex_);

    turag_infof("Timed Events:\n");
    turag_infof("  current time: %u ms\n", SystemTime::now().toMsec());

    if (!timequeue_.empty()) {
        for (const auto& tevent : timequeue_) {
            if (tevent.event.event_class) {
                EventId id = tevent.event.event_class->id;

                turag_infof("  @%u ms %c%c%c%u %" PRIxPTR "\n",
                            tevent.time.toMsec(),
                            static_cast<char>(id >> 24),
                            static_cast<char>(id >> 16),
                            static_cast<char>(id >> 8),
                            static_cast<unsigned>(id & 0xFF),
                            reinterpret_cast<ptrdiff_t>(tevent.event.method));
            }
        }
    } else {
        turag_info("  timed event queue is empty\n");
    }
}

void
EventQueue::printQueue() {
  Mutex::Lock lock(mutex_);

  turag_info("EventQueue Debug Information:");
  turag_info(" - Events:");
  if (!queue_.empty()) {
    for (const auto& event : queue_) {
      print_debug_info(event);
    }
  } else {
    turag_info("  event queue is empty");
  }
}

void
EventQueue::printDebugInfo() {
  printTimeQueue();
  printQueue();

  turag_info ("Stack:\n");
  turag_infof("  size of EventQueue: %" PRIuPTR " bytes\n", sizeof(EventQueue));
  turag_infof("  number of queue elements: %" PRIuPTR "\n", size);
  turag_infof("  number of timed queue elements: %" PRIuPTR "\n", timequeue_size);
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
