
#define TURAG_DEBUG_LOG_SOURCE "J"

#include "statemachine.h"
#include <tina++/thread.h>
#include <tina/debug.h>

namespace TURAG {
namespace IM {

Mutex interface_mutex;


namespace {

RedoState redo_state;
FinishedState finished_state;


}

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// ---- Statemachine class
// -------------------------------------------------------------------
// -------------------------------------------------------------------
Statemachine* Statemachine::first_active_statemachine = nullptr;
Statemachine* Statemachine::first_to_be_activated_statemachine = nullptr;
Statemachine* Statemachine::last_to_be_activated_statemachine = nullptr;
Statemachine* Statemachine::first_to_be_stopped_statemachine = nullptr;

EventQueue* Statemachine::defaultEventqueue_ = nullptr;

State* const Statemachine::restartState = &redo_state;
State* const Statemachine::finished = &finished_state;
State* const Statemachine::error = nullptr;


Statemachine::Statemachine(
        const char* const pname,
        State* const pentrystate,
        State* const initState,
        const EventClass* const eventOnSuccessfulInitialization,
        State* const pabortstate,
        const EventClass* const eventOnGracefulShutdown,
        const EventClass* const eventOnErrorShutdown):
	next_active(nullptr),
	last_active(nullptr),
	next_to_be_activated(nullptr),
    previous_to_be_activated(nullptr),
	next_to_be_stopped(nullptr),
    name(pname),
    startTime(0),
    status_(Status::none),
    argument_(0),
	sendSignal_(false),
	clearSignal_(false),
    signal_(0),
    pcurrent_state(nullptr),
    entrystate(pentrystate),
    initializedState(initState),
    abortstate(pabortstate),
    myEventOnSuccessfulInitialization(eventOnSuccessfulInitialization),
    myEventOnGracefulShutdown(eventOnGracefulShutdown),
    myEventOnErrorShutdown(eventOnErrorShutdown),
    myEventOnGracefulShutdownOverride(nullptr),
    myEventOnErrorShutdownOverride(nullptr),
    eventqueue_(nullptr)
{ }

void Statemachine::start(uintptr_t argument, bool supressStatechangeDebugMessages) {
    start(defaultEventqueue_, argument, supressStatechangeDebugMessages);
}

void Statemachine::start(EventQueue* eventqueue, uintptr_t argument, bool supressStatechangeDebugMessages_) {
    Mutex::Lock lock(interface_mutex);

    argument_ = argument;
    eventqueue_ = eventqueue;

    if (getStatusInternal() == Status::waiting_for_activation) {
        turag_infof("%s: not added: already in activation queue", name);
    } else if (getStatusInternal() == Status::running_and_waiting_for_deactivation) {
        turag_criticalf("%s: not added: waiting to be deactivated", name);
    } else if (getStatusInternal() == Status::running) {
        turag_infof("%s: not added: already running", name);
    } else if(getStatusInternal() == Status::running_and_initialized) {
        turag_infof("%s: not added: already running", name);
        // send initialized event but only if we are already initialized
        emitEvent(myEventOnSuccessfulInitialization);
    } else {
        if (Statemachine::last_to_be_activated_statemachine) {
            previous_to_be_activated = Statemachine::last_to_be_activated_statemachine;
            previous_to_be_activated->next_to_be_activated = this;
            Statemachine::last_to_be_activated_statemachine = this;
        } else {
            Statemachine::first_to_be_activated_statemachine = this;
            Statemachine::last_to_be_activated_statemachine = this;
        }
        next_to_be_activated = nullptr;

        // reset event overrides
        myEventOnGracefulShutdownOverride = nullptr;
        myEventOnErrorShutdownOverride = nullptr;
        
        supressStatechangeDebugMessages = supressStatechangeDebugMessages_;
        status_ = Status::waiting_for_activation;
		sendSignal_ = false;
		clearSignal_ = false;
    }
}

void Statemachine::stop(void) {
    Mutex::Lock lock(interface_mutex);

    if (getStatusInternal() == Status::running_and_waiting_for_deactivation) {
        turag_infof("%s: not added: already in deactivation queue", name);
    } else if (getStatusInternal() == Status::waiting_for_activation) {
        turag_criticalf("%s: not stopped: already in activation queue", name);
    } else if (getStatusInternal() == Status::running_and_initialized || getStatusInternal() == Status::running) {
        next_to_be_stopped =  Statemachine::first_to_be_stopped_statemachine;
        Statemachine::first_to_be_stopped_statemachine = this;
        status_ = Status::running_and_waiting_for_deactivation;
    } else {
        turag_infof("%s: not stopped: wasn't running", name);
        // send shutdown event nonetheless
        emitEvent(myEventOnSuccessfulInitialization);
    }
}


bool Statemachine::sendSignal(uintptr_t signal) {
    Mutex::Lock lock(interface_mutex);

    if (isActiveInternal()) {
		sendSignal_ = true;
		clearSignal_ = false;
		signal_ = signal;
        return true;
    } else {
        return false;
    }
}

bool Statemachine::clearSignal(void) {
    Mutex::Lock lock(interface_mutex);

    if (isActiveInternal()) {
		sendSignal_ = false;
		clearSignal_ = true;
		return true;
    } else {
        return false;
    }
}


bool Statemachine::isActive(void) {
  Mutex::Lock lock(interface_mutex);
  return isActiveInternal();
}

bool Statemachine::isRunning(void) {
    Mutex::Lock lock(interface_mutex);
    return isRunningInternal();
}

Statemachine::Status Statemachine::getStatus(void) {
    Mutex::Lock lock(interface_mutex);
    return getStatusInternal();
}

SystemTime Statemachine::getRunningTime(void) {
    Mutex::Lock lock(interface_mutex);

    if (isRunningInternal()) {
        return SystemTime::now() - startTime;
    } else {
        return SystemTime(0);
    }
}

void Statemachine::setDefaultEventqueue(EventQueue* eventqueue) {
    Mutex::Lock lock(interface_mutex);
    defaultEventqueue_ = eventqueue;
}

void Statemachine::doStatemachineProcessing(void) {
    ScopedLock<Mutex> lock(interface_mutex);

    // activate statemachines that are in activation queue
    Statemachine* sm = Statemachine::first_to_be_activated_statemachine;
    while (sm != nullptr) {
        if (sm->change_state(sm->entrystate, &lock)) {
            sm->startTime = SystemTime::now();
            sm->status_ = Status::running;

            sm->next_active = Statemachine::first_active_statemachine;
            sm->last_active = nullptr;
            if (sm->next_active != nullptr)
                sm->next_active->last_active = sm;
            Statemachine::first_active_statemachine = sm;

            if (!sm->supressStatechangeDebugMessages) {
                turag_infof("%s activated", sm->name);
            } else {
                turag_infof("%s activated; output of state change debug messages supressed", sm->name);
            }
        } else {
            sm->status_ = Status::stopped_on_error;
            sm->emitEvent(sm->myEventOnErrorShutdown);

            turag_criticalf("%s: couldn't be activated", sm->name);
        }
        sm = sm->next_to_be_activated;
    }
    Statemachine::first_to_be_activated_statemachine = nullptr;
    Statemachine::last_to_be_activated_statemachine = nullptr;

    // deactivate statemachines that are in deactivation queue
    sm = Statemachine::first_to_be_stopped_statemachine;
    while (sm != nullptr) {
        if (sm->status_ == Status::running_and_waiting_for_deactivation) {
            if (!sm->change_state(sm->abortstate, &lock)) {
                sm->removeFromActiveList();
                sm->status_ = Status::stopped_on_error;
                sm->emitEvent(sm->myEventOnErrorShutdown);

                turag_infof("%s: couldn't enter abortstate -> cancelled", sm->name);
            }
        }
        sm = sm->next_to_be_stopped;
    }
    Statemachine::first_to_be_stopped_statemachine = nullptr;


    // execute all active state machines
    sm = Statemachine::first_active_statemachine;
    while (sm != nullptr) {
		// update the signal status of the state
		if (sm->sendSignal_) {
			sm->pcurrent_state->sendSignal(sm->signal_);
			sm->sendSignal_ = false;
		} else if (sm->clearSignal_){
            sm->pcurrent_state->clearSignal();
			sm->clearSignal_ = false;
        }

		// call the transition function of the currently active state.
		// This will return a pointer to the next state.
		// we unlock the mutex because this function might take
		// a longer while
		lock.unlock();
        State *state = sm->pcurrent_state->transition_function();
        lock.lock();

        // Save event overrides and clear them. This keeps the state instances clean.
        if (sm->pcurrent_state->getEventOnGracefulShutdownOverride()) {
            sm->myEventOnGracefulShutdownOverride = sm->pcurrent_state->getEventOnGracefulShutdownOverride();
        }
        if (sm->pcurrent_state->getEventOnErrorShutdownOverride()) {
            sm->myEventOnErrorShutdownOverride = sm->pcurrent_state->getEventOnErrorShutdownOverride();
        }
        sm->pcurrent_state->clearEventOverrides();


        // execute appropriate actions, depending on new state
        // don't do anything, if state == sm->pcurrent_state
        if (state == Statemachine::error) {
            sm->removeFromActiveList();
            sm->status_ = Status::stopped_on_error;

            if (sm->myEventOnErrorShutdownOverride) {
                turag_infof("%s: emitting overriden shutdown on error event", sm->name);
                sm->emitEvent(sm->myEventOnErrorShutdownOverride);
            } else {
                sm->emitEvent(sm->myEventOnErrorShutdown);
            }
            turag_infof("%s cancelled on error", sm->name);
        } else if (state == Statemachine::finished) {
            sm->removeFromActiveList();
            sm->status_ = Status::stopped_gracefully;

            if (sm->myEventOnGracefulShutdownOverride) {
                turag_infof("%s: emitting overriden graceful shutdown event", sm->name);
                sm->emitEvent(sm->myEventOnGracefulShutdownOverride);
            } else {
                sm->emitEvent(sm->myEventOnGracefulShutdown);
            }
            turag_infof("%s finished", sm->name);
        } else if (state == Statemachine::restartState) {
            turag_warningf("%s: Do Statefunc again", sm->name);
            lock.unlock();
            bool success = sm->pcurrent_state->state_function();
            lock.lock();

            if (!success) {
                sm->removeFromActiveList();
                sm->status_ = Status::stopped_on_error;
                sm->emitEvent(sm->myEventOnErrorShutdown);
                turag_infof("%s cancelled on error", sm->name);
            }
        } else if (state != sm->pcurrent_state) {
            if (!sm->change_state(state, &lock)) {
                sm->removeFromActiveList();
                sm->status_ = Status::stopped_on_error;
                sm->emitEvent(sm->myEventOnErrorShutdown);
                turag_infof("%s cancelled on error", sm->name);
            }

        }

        sm = sm->next_active;
    }
}



bool Statemachine::change_state(State* next_state, ScopedLock<Mutex> *lock) {
    if (!next_state) {
        return false;
    } else {
        // before calling the state function, we forward some
        // stuff into the state which enables unlocked access to this data
        next_state->setEventQueue(eventqueue_);
        next_state->setArgument(argument_);
        next_state->setStarttime(SystemTime::now());
        next_state->setStatemachineStarttime(startTime);

		// We also forward the signal status from the current to the new state
		if (pcurrent_state) {
			next_state->setSignal(pcurrent_state);
		}

        // now we can safely unlock the mutex. This is good because
        // we don't know anything about this function and it could
        // take a little while to return
        lock->unlock();
        bool success = next_state->state_function();
        lock->lock();

        if (success) {
            if (!supressStatechangeDebugMessages) {
                if (pcurrent_state) {
                    turag_infof("%s: %s --> %s", name, pcurrent_state->name, next_state->name);
                } else {
                    turag_infof("%s: entered initial state: %s", name, next_state->name);
                }
            }

            if (next_state == initializedState && status_ == Status::running) {
                emitEvent(myEventOnSuccessfulInitialization);
                status_ = Status::running_and_initialized;
            }

            pcurrent_state = next_state;
            return true;
        } else {
            turag_criticalf("%s: statechange failed", name);
            return false;
        }
    }
}


void Statemachine::removeFromActiveList(void) {
    if (Statemachine::first_active_statemachine == this) {
        Statemachine::first_active_statemachine = next_active;
        if (next_active) {
            next_active->last_active = nullptr;
        }
    } else {
        last_active->next_active = next_active;
        if (next_active) {
            next_active->last_active = last_active;
        }
    }
    pcurrent_state = nullptr;
}

bool Statemachine::isActiveInternal(void) {
    if (status_ == Status::running ||
            status_ == Status::running_and_initialized ||
            status_ == Status::waiting_for_activation ||
            status_ == Status::running_and_waiting_for_deactivation) {
        return true;
    } else {
        return false;
    }
}

bool Statemachine::isRunningInternal(void) {
    if (status_ == Status::running ||
            status_ == Status::running_and_initialized ||
            status_ == Status::running_and_waiting_for_deactivation) {
        return true;
    } else {
        return false;
    }
}


} //namespace IM
} // ns TURAG

