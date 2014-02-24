/**
 * \file tina++/im/statemachine.h
 * \author Julius Hoffmann
 * \author Martin Oemus <martin@oemus.net>
 * \date 02.2014
 *
 * \brief Internal Mechanics state machine
 *
 * This file contains the classes required to define state machines that are aimed to be used for
 * controlling mechanical state flows.
 *
 */



#ifndef IM_STATEMACHINE_H
#define IM_STATEMACHINE_H

#include <tina++/tina.h>
#include <tina++/time.h>
#include <tina++/debug.h>
#include <tina++/statemachine.h>

namespace TURAG {
namespace IM {


class Statemachine;

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// ---- State classes
// -------------------------------------------------------------------
// -------------------------------------------------------------------
class State {
    friend class Statemachine;

private:
    bool hasSignal_;
    int32_t signal_;
    int32_t argument_;

    EventQueue* eventqueue_;

protected:
    const char* const name;

    void setEventQueue(EventQueue* eventqueue) { eventqueue_ = eventqueue; }

    bool hasSignal(void) const { return hasSignal_; }
    int32_t getSignal(void) const { return signal_; }
    void setSignal(int32_t signal = 0) { signal_ = signal; hasSignal_ = true; }
    void clearSignal(void) { hasSignal_ = false; }

    int32_t getArgument(void) const { return argument_; }
    void setArgument(int32_t argument) { argument_ = argument; }


    void emitEvent(const EventClass* event_class, EventArg params = 0) const {
        if (eventqueue_&& event_class && event_class->id != EventQueue::event_null) eventqueue_->push(event_class, params);
    }

    template<typename T, REQUIRES(!std::is_integral<T>)> _always_inline
    void push(const EventClass* event_class, T param) const {
      emitEvent(event_class, pack<EventArg>(param));
    }


    /** return true in a state function to proceed normally.
     * return false to stop statemachine
     */
    virtual bool state_function(void) = 0;

    /** return this in a transition_function to indicate that no statechange
     * should take place.
     *
     * return nullptr to indicate that statemachine handling the
     * current state(s) should be stopped due to an error.
     *
     * Return a valid State to indicate a Statechange to this State.
     *
     * return &redo_state to request executing the state function once again.
     *
     * return &finished_state to indicate that statemachine handling the
     * current state(s) should be stopped due to an external request or normal
     * state flow.
     */
    virtual State* transition_function(void) = 0;


public:
    State(const char* const name_) :
        hasSignal_(false), signal_(0), eventqueue_(nullptr), name(name_) {}
    virtual ~State(void) {}

};


class SpecialState: public State {
public:
    SpecialState() : State("") {}
    virtual bool state_function(void) { return true; }
    virtual State* transition_function(void) { return nullptr; }
};


class ImmediateAbort : public State {
public:
    ImmediateAbort() : State("Finished") {}
    virtual bool state_function(void) { return true; }
    virtual State* transition_function(void);
};



class DelayState : public State {
private:
    const SystemTime myDelay;
    SystemTime myStartTime;
    State* const myNextState;
public:
    DelayState(SystemTime delay, State* const nextState) : State("DelayState"), myDelay(delay), myStartTime(), myNextState(nextState) {}
    bool state_function(void) {
        myStartTime = SystemTime::now();
        return true;
    }
    State* transition_function(void);
};


// -------------------------------------------------------------------
// -------------------------------------------------------------------
// ---- Statemachine class
// -------------------------------------------------------------------
// -------------------------------------------------------------------
class Statemachine {
public:
    enum class Status {
        none,
        waiting_for_activation,
        running,
        running_and_initialized,
        running_and_waiting_for_deactivation,
        stopped_on_error,
        stopped_gracefully
    };

    static State* const restartState;
    static State* const finished;
    static State* const error;

    static State* const ImmediateAbort;

    Statemachine(
            const char* const pname,
            State* const pentrystate,
            State* const initState = nullptr,
            const EventClass* const eventOnSuccessfulInitialization = nullptr,
            State* const pabortstate = Statemachine::ImmediateAbort,
            const EventClass* const eventOnGracefulShutdown = nullptr,
            const EventClass* const eventOnErrorShutdown = nullptr);


    // thread-safe access functions
    void start(int32_t argument = 0);
    void start(EventQueue* eventqueue, int32_t argument = 0);
    void startSilent(int32_t argument = 0) { start(nullptr, argument); }
    void stop(void);
    bool sendSignal(int32_t signal = 0);
    bool isActive(void);
    bool isRunning(void);
    Status getStatus(void);
    SystemTime getRunningTime(void);
    static void setDefaultEventqueue(EventQueue* eventqueue);

    // call this from IM-thread
    static void doStatemachineProcessing(void);

protected:
    // doubly linked list to manage active statemachines
    static Statemachine* first_active_statemachine;
    Statemachine* next_active;
    Statemachine* last_active;

    // singly linked list for statemachines, that should be activated
    static Statemachine* first_to_be_activated_statemachine;
    Statemachine* next_to_be_activated;

    // singly linked list for statemachines, that should be stopped
    static Statemachine* first_to_be_stopped_statemachine;
    Statemachine* next_to_be_stopped;


    const char* const name;
    SystemTime startTime;
    Status status_;
    int32_t argument_;
    bool hasSignal_;
    int32_t signal_;
    State* pcurrent_state;
    State * const entrystate;
    State * const initializedState;
    State * const abortstate;

    const EventClass* const myEventOnSuccessfulInitialization;
    const EventClass* const myEventOnGracefulShutdown;
    const EventClass* const myEventOnErrorShutdown;

    static EventQueue* defaultEventqueue_;
    EventQueue* eventqueue_;


    bool change_state(State* next_state, ScopedLock<Mutex>* lock) ;

    void removeFromActiveList(void);

    bool isActiveInternal(void);
    bool isRunningInternal(void);
    Status getStatusInternal(void) { return status_; }

    void emitEvent(const EventClass* event_class, EventArg params = 0) const {
        if (eventqueue_ && event_class && event_class->id != EventQueue::event_null) eventqueue_->push(event_class, params);
    }
    template<typename T, REQUIRES(!std::is_integral<T>)> _always_inline
    void push(const EventClass* event_class, T param) const {
      emitEvent(event_class, pack<EventArg>(param));
    }
};


} // namespace IM
} // namespace TURAG

#endif //IM_STATEMACHINE_H
