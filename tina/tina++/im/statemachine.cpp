
#define TURAG_DEBUG_LOG_SOURCE "J"

#include "statemachine.h"
#include <tina++/thread.h>
#include <tina/debug.h>

namespace TURAG {
namespace IM {

Mutex interface_mutex;


namespace {

class RedoState final : public State {
public:
	RedoState() : State("Error") {}
protected:
	State* transition_function(bool) { return nullptr; }
};

class FinishedState final : public State {
public:
	FinishedState() : State("Finished") {}
protected:
	State* transition_function(bool) { return nullptr; }
};

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


Statemachine::Statemachine(const char* const name_,
        State* const entrystate_,
        const EventClass* const eventOnGracefulShutdown,
        const EventClass* const eventOnErrorShutdown,
        State* const abortstate_):
	next_active(nullptr),
	last_active(nullptr),
	next_to_be_activated(nullptr),
    previous_to_be_activated(nullptr),
	next_to_be_stopped(nullptr),
	name(name_),
    startTime(0),
	current_state(nullptr),
	entrystate(entrystate_),
	abortstate(abortstate_),
    resultCallback_(nullptr),
    myEventOnGracefulShutdown(eventOnGracefulShutdown),
    myEventOnErrorShutdown(eventOnErrorShutdown),
	eventqueue_(nullptr),
	status_(Status::none),
	supressStatechangeDebugMessages(false),
	sendSignal_(false),
	clearSignal_(false),
	signal_(0),
	argument_(0),
	enteredNewState_(false)
		{ }

void Statemachine::start(uintptr_t argument, bool supressStatechangeDebugMessages, EventMethod resultCallback) {
    start(defaultEventqueue_, argument, supressStatechangeDebugMessages, resultCallback);
}

void Statemachine::start(EventQueue* eventqueue, uintptr_t argument, bool supressStatechangeDebugMessages_, EventMethod resultCallback) {
    Mutex::Lock lock(interface_mutex);

    argument_ = argument;
    eventqueue_ = eventqueue;
    resultCallback_ = resultCallback;

    if (getStatusInternal() == Status::waiting_for_activation) {
        turag_infof("%s: not added: already in activation queue", name);
    } else if (getStatusInternal() == Status::running) {
        turag_infof("%s: not added: already running", name);
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
	} else if (getStatusInternal() == Status::running) {
		next_to_be_stopped = Statemachine::first_to_be_stopped_statemachine;
        Statemachine::first_to_be_stopped_statemachine = this;
        status_ = Status::running_and_waiting_for_deactivation;
    } else {
        turag_infof("%s: not stopped: wasn't running", name);
        // send shutdown event nonetheless
		emitEvent(myEventOnGracefulShutdown);
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

    Statemachine* sm;
    // activate statemachines that are in activation queue
    for (sm = Statemachine::first_to_be_activated_statemachine;
         sm != nullptr;
         sm = sm->next_to_be_activated)
    {
        if (sm->status_ == Status::running) {
            turag_criticalf("%s not activated: Still running", sm->name);
            continue;
        }
        if (sm->status_ == Status::running_and_waiting_for_deactivation) {
            turag_infof("%s not activated: Still running and waiting for deactivation", sm->name);
            continue;
        }
		if (!sm->entrystate) {
			turag_errorf("%s: entry state is nullptr", sm->name);
			sm->status_ = Status::stopped_on_error;
			sm->emitEvent(sm->myEventOnErrorShutdown);
            continue;
        }
        if (!sm->supressStatechangeDebugMessages) {
            turag_infof("%s activated", sm->name);
        } else {
            turag_infof("%s activated; output of state change debug messages supressed", sm->name);
        }
        sm->startTime = SystemTime::now();
        sm->status_ = Status::running;
        sm->change_state(sm->entrystate);

        sm->next_active = Statemachine::first_active_statemachine;
        sm->last_active = nullptr;
        if (sm->next_active != nullptr)
            sm->next_active->last_active = sm;
        Statemachine::first_active_statemachine = sm;
        
        if (sm->next_to_be_activated)
            sm->next_to_be_activated->previous_to_be_activated = sm->previous_to_be_activated;
        if (sm->previous_to_be_activated)
            sm->previous_to_be_activated->next_to_be_activated = sm->next_to_be_activated;
        if (Statemachine::first_to_be_activated_statemachine == sm)
            Statemachine::first_to_be_activated_statemachine = sm->next_to_be_activated;
        if (Statemachine::last_to_be_activated_statemachine == sm)
            Statemachine::last_to_be_activated_statemachine = sm->previous_to_be_activated;
    }

    // deactivate statemachines that are in deactivation queue
    sm = Statemachine::first_to_be_stopped_statemachine;
    while (sm != nullptr) {
		if (sm->abortstate) {
            if(sm->abortstate == Statemachine::finished) {
                sm->removeFromActiveList();
                sm->status_ = Status::stopped_gracefully;
                sm->emitEvent(sm->myEventOnGracefulShutdown);
                turag_infof("%s: stopped", sm->name);
            }
			sm->change_state(sm->abortstate);
		} else {
			sm->removeFromActiveList();
			sm->status_ = Status::stopped_on_error;
			sm->emitEvent(sm->myEventOnErrorShutdown);
			turag_infof("%s: abort state is nullptr", sm->name);
		}
		sm = sm->next_to_be_stopped;
    }
    Statemachine::first_to_be_stopped_statemachine = nullptr;


    // execute all active state machines
    sm = Statemachine::first_active_statemachine;
    while (sm != nullptr) {
		// update the signal status of the state
		if (sm->sendSignal_) {
			sm->current_state->sendSignal(sm->signal_);
			sm->sendSignal_ = false;
		} else if (sm->clearSignal_){
			sm->current_state->clearSignal();
			sm->clearSignal_ = false;
        }

		// call the transition function of the currently active state.
		// This will return a pointer to the next state.
		// we unlock the mutex because this function might take
		// a longer while
		lock.unlock();
		State *state = sm->current_state->transition_function(sm->enteredNewState_);
        lock.lock();
		sm->enteredNewState_ = false;

        // execute appropriate actions, depending on new state
        // don't do anything, if state == sm->pcurrent_state
        if (state == Statemachine::error) {
            sm->removeFromActiveList();
            sm->status_ = Status::stopped_on_error;
			sm->emitEvent(sm->myEventOnErrorShutdown);
			turag_infof("%s: cancelled on error", sm->name);
        } else if (state == Statemachine::finished) {
            sm->removeFromActiveList();
            sm->status_ = Status::stopped_gracefully;
			sm->emitEvent(sm->myEventOnGracefulShutdown);
			turag_infof("%s: finished", sm->name);
        } else if (state == Statemachine::restartState) {
			sm->change_state(sm->current_state);
		} else if (state != sm->current_state) {
			if (state) {
				sm->change_state(state);
			} else {
                sm->removeFromActiveList();
                sm->status_ = Status::stopped_on_error;
                sm->emitEvent(sm->myEventOnErrorShutdown);
				turag_infof("%s: next state is nullptr", sm->name);
            }
        }

        sm = sm->next_active;
    }
}


void Statemachine::change_state(State* next_state) {
	enteredNewState_ = true;

	// forward some stuff into the state which enables
	// unlocked access to this data from within it
	next_state->setEventQueue(eventqueue_);
	next_state->setArgument(argument_);
	next_state->setStarttime(SystemTime::now());
	next_state->setStatemachineStarttime(startTime);

	// We also forward the signal status from the current to the new state
	if (current_state) {
		next_state->setSignal(current_state);
	}

	if (!supressStatechangeDebugMessages) {
		if (current_state) {
			turag_infof("%s: %s --> %s", name, current_state->name, next_state->name);
		} else {
			turag_infof("%s: entered initial state: %s", name, next_state->name);
		}
	}
	current_state = next_state;
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
	current_state = nullptr;
}

bool Statemachine::isActiveInternal(void) {
    if (status_ == Status::running ||
            status_ == Status::waiting_for_activation ||
            status_ == Status::running_and_waiting_for_deactivation) {
        return true;
    } else {
        return false;
    }
}

bool Statemachine::isRunningInternal(void) {
    if (status_ == Status::running ||
            status_ == Status::running_and_waiting_for_deactivation) {
        return true;
    } else {
        return false;
	}
}


} //namespace IM
} // ns TURAG

