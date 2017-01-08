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

/*!
 * @defgroup im Internal Mechanics
 *
 * @section IM-Statemachine
 * This module offers the possibility to define flexible finite statemachines
 * that are able reflect the usual requirements for mechanical state flows.
 *
 * The central idea about the internal mechanics statemachines is to have special
 * IM-thread running that works on the continuous execution of the statemachines.
 * Some other thread that makes use of the IM-module starts and stops them, receives events
 * concerning their stateflow and to some extent controls the way they are executed.
 * For that reason the end-user interface is strictly thread-safe and the module was
 * designed to be used in a multi-threaded environment, but that is not a requirement.
 *
 * Statemachines consist of states. States are defined by a state-function that
 * evaluates the state's precondition and a transition-function which evaluates
 * the conditions required to enter the appropriate follow-state. Both functions are virtual
 * functions and part of the interface of an abstract base State-class which have
 * to be implemented in inherited State-classes.
 *
 * On details about what state- and transition-functions are and what to keep in mind when implementing
 * them, see: \link State::state_function \endlink and \link State::transition_function \endlink .
 *
 * \subsection features Features
 *  - Statemachines can be started and stopped.
 *  - Multiple statemachines can run concurrently.
 *  - Statemachines can emit specified events to a given eventqueue while being active.
 *  - The execution of a statemachine
 * can be part of the stateflow of another one.
 *  - When started, a statemachine can get an argument, which is valid as long
 * as the execution lasts and is available in all associated states.
 *  - A state can wait for a specific external signal. Signals are only valid
 * in the transition-function of the currently active state.
 *
 * \subsection defstatemachines Creating statemachines
 * Let's look at how to create a simple statemachine that works with some piece
 * of hardware and consists of four states:
 *
 * \dot
 * digraph example {
 * rankdir=LR;
 * node [shape=record, fontname=Helvetica, fontsize=10];
 * entry [ label = "entry", shape="diamond" ];
 * prepare [ label="Prepare hardware"];
 * start [ label="working: step 1"];
 * work [ label="working: step 2"];
 * end [ label="Tidy up & finish"];
 * finished [label="stopped", shape="diamond"];
 * entry -> prepare -> start -> work -> end -> finished;
 * work -> start;
 * prepare -> prepare [ arrowhead="open", style="dashed" ];
 * start -> start [ arrowhead="open", style="dashed" ];
 * work -> work [ arrowhead="open", style="dashed" ];
 * end -> end [ arrowhead="open", style="dashed" ];
 * }
 * \enddot
 *
 * It's mainly linear. In the third state there is the possibility to either restart the
 * working cycle or let the statemachine finish its work.
 *
 * The behaviour of a statemachine is defined by its states and how they are connected.
 * So you should create a new source file, including at least the statemachine-header:
 * \snippet im_statemachine_states_example.cpp Includes
 *
 * Because states are usually defined for one particular purpose while sharing the need
 * to work on some hardware or device, it is useful to first declare a base state
 * the actual states will inherit from. In our example each state will be able to access
 * some piece of hardware with a void-Pointer. In real code this should of course be a pointer
 * to a class.
 * \snippet im_statemachine_states_example.cpp Basestate
 *
 * Now we can define the states we need to build our statemachine. Each state has to
 * implement the state- and the transition-function. You should also note that every
 * state owns one or more pointers to its possible follow states with the exception
 * of the last one. The last state doesn't need such a pointer because it will return
 * a special value, leading to the termination of the statemachine.
 * \snippet im_statemachine_states_example.cpp States
 *
 * Now we can implement the behaviour of our statemachine. Each state owns a state-fumction
 * that is executed once when the state becomes active. Return false in this function
 * to indicate an error and have the whole statemachine cancelled. he second function
 * is the transition-function which is called as long as you return the this-pointer. There
 * are some special return values (see State::transition_function). Keep in mind, that neither
 * the state- nor the transition are allowed to contain code or functions that take an
 * unconsiderable amount of time to finish. Both functions should return quickly.
 * \snippet im_statemachine_states_example.cpp Functions
 *
 * Now, as we have complete state classes, we need to define some instances of them and
 * connect them according to our state flow. Because we want to use our states for two
 * statemachines that behave the same but work on different hardware, we do this in a
 * separate namespace. Take note of the extern declarations before the actual instantiations.
 * They are necessary to define the states in the correct logical order. Otherwise we would
 * get compiler errors.
 * \snippet im_statemachine_states_example.cpp Instances
 *
 * Now all tht's left to do is putting our states together into a statemachine. Not all
 * arguments are required, most are optional. For details see Statemachine::Statemachine.
 * \snippet im_statemachine_states_example.cpp Statemachine
 *
 * At the very end don't forget to close your namespaces and everything will work well:
 * \snippet im_statemachine_states_example.cpp Rest
 *
 * Now you can put your statemachine in some header file and use it wherever needed.
 *
 * \subsection communicationwithevents Communication with events
 * Because the statemachine module was designed to work in a multi-threaded environment
 * it can make use of an Eventqueue-object to inform a different thread about the status of
 * running statemachines.
 *
 * The eventqueue to communicate with can be set globally, or each time when a statemachine
 * is started. It is also possible to start a statemachine in silent mode, then it will not
 * emit any event.
 *
 * Every statemachine is defined with a set of three standard events, which are emitted
 * automatically:
 * - eventOnSuccessfulInitialization
 * - eventOnGracefulShutdown
 * - eventOnErrorShutdown
 *
 * Additionally you can use State::emitEvent to generate events specific to your statemachine.
 *
 * \subsection usingstatemachinesinotherstatemachines Using a statemachine in other statemachines
 * It's possible to start and stop statemachines within the state- or transition-function of states
 * and additionally from other threads because the statemachine-interface is fully thread-safe. Yet
 * as there is no way to receive events from the started child statemachine in the parent statemachine,
 * you are limited to use the the status-functions of the statemachine. A possible
 * use-case could look like this:
 * - in the state-function of one of the parent statemachine's states, start the child statemachine
 * - in the corresponding transition function use Statemachine::isActive() to wait for the child
 * statemachine to finish and decide which state to activate next depending on how the child
 * ended. You can find out whether it caught an error with Statemachine::getStatus().
 *
 * Still, there are some issues to keep in mind if you want to make use of the concept
 * of statemachines-in-statemachines:
 * - Always start the statemachine in silent-mode (Statemachine::startSilent), even if the statemachine was defined without
 * standard events, because the states could still contain emitEvent-calls. This is of course
 * UNLESS you want the statemachine to emit events to the eventqueue, but this practice is
 * highly discouraged.
 * - Statemachines (at least those who emit events) should be exclusive to either one spearate thread
 * or the statemachine thread. Consider the following example: some thread starts statemachine A.
 * %Statemachine A starts statemachine B in the recommended silent mode and runs for a good while.
 * Now the thread that started A suddenly needs to start B which normally emits an initialized-event.
 * But B is already running - in silent mode. So the thread will never know the story about B and
 * probably wait forever. You might say: then A shouldn't start B in silent mode, but that's even worse
 * in the opposite case: now B suddenly emits events which the thread did never expect and it might get
 * totally confused.
 *
 * To wrap it all up: statemachines-in-statemachines is fine and good, but if you use it, start them silent
 * and keep all statemachines separate (unless you can really guarantee that it doesn't emit events and still then
 * there remain some issues).
 *
 *
 * \subsection signalsandarguments Signals and arguments
 * When you start a statemachine, it's possible to pass an argument along. This is an integer value
 * whose meaning depends on the statemachine. You might define some enum and use the argument
 * to customize the behaviour of the statemachine with a few distinct modes or you could use it
 * as a continuous value which might be the angle of the axis of some robot arm. If you need more than
 * what fits into 4 byte, you could supply a pointer but then you have to make sure that the corresponding
 * data doesn't get touched while the statemachine is running.
 *
 * The argument of a statemachine is available in both the state- and the transition-functions of all states
 * that are part of the statemachine.
 *
 * Signals offer a similar functionality with the difference that they only live for the time of the current
 * state still being active. You can only push a signal to a statemachine, once it is running. The signal will then
 * be available only in the transition-function of the currently active state. You can pass an argument along
 * with the signal, but that is optional.
 *
 * A likely use-case of the signals (a state that delays the stateflow until it receives any signal) is
 * readily available for use with the class WaitForSignalState.
 *
 */


#ifndef IM_STATEMACHINE_H
#define IM_STATEMACHINE_H

#include <tina++/time.h>
#include <tina++/statemachine.h>
#include <tina++/helper/scoped_lock.h>
#include <tina++/helper/packed.h>

namespace TURAG {

/*!
 * @brief Internal Mechanics.
 */
namespace IM {

/*!
 * @addtogroup im
 * @{
 */


// -------------------------------------------------------------------
// -------------------------------------------------------------------
// ---- State classes
// -------------------------------------------------------------------
// -------------------------------------------------------------------
/*!
 * \brief Abstract base class for states.
 *
 *
 */
class State {
    friend class Statemachine;

public:
    /*!
     * \brief State-Constructor
     * \param name_ Name of the state.
     */
    State(const char* const name_) :
        name(name_), hasSignal_(false), signal_(0), eventOnGracefulShutdown(nullptr),
        eventOnErrorShutdown(nullptr), eventqueue_(nullptr) {}

protected:
    /*!
     * \brief Name of the state.
     */
    const char* const name;

    /*!
     * \brief Check for the availablility of an external signal.
     * \details
     *
     * \return
     */
	bool hasSignal(void) const { return hasSignal_; }

    /*!
     * \brief Returns argument value of received signal.
     * \details Before calling this function you need to check for an
     * available signal by calling hasSignal(). If there is no signal available,
     * the return value of this function is meanignless.
     * \return Argument of received signal.
     */
	uintptr_t getSignal(void) const { return signal_; }
    
    /*!
     * \brief Returns the value of the argument the associated statemachine
     * was started with.
     * \return Value of the statemachine's argument.
     */
	uintptr_t getArgument(void) const { return argument_; }

    /*!
     * \brief Returns how long this state is already active.
     * \return Runtime of this state.
     */
    SystemTime getRuntime(void) { return SystemTime::now() - stateStarttime_; }

    /*!
     * \brief Returns how long the whole statemachine is already active.
     * \return Runtime of statemachine in whose context this state is being executed.
     */
    SystemTime getStatemachineRuntime(void) { return SystemTime::now() - statemachineStarttime_; }

    /*!
     * \brief Add an event to the eventqueue.
     * \details If an eventqueue was set in the statemachine this state is part of
     * and the statemachine was not started in silent mode, this function adds an event
     * of the given event_class with an optional parameter.
     * \param event_class Event class of the event to add.
     * \param params Optional params.
     */
    void emitEvent(const EventClass* event_class, EventArg params = 0) const {
        if (eventqueue_&& event_class && event_class->id != EventQueue::event_null) eventqueue_->push(event_class, params);
    }
    /*!
     *\brief Add an event to the eventqueue.
     * \details Same as above.
     * \param event_class Event class of the event to add.
     * \param param Parameter object. Needs to be <= 4 bytes.
     */
    template<typename T, REQUIRES(!std::is_integral<T>)> _always_inline
    void emitEvent(const EventClass* event_class, T param) const {
      emitEvent(event_class, pack<EventArg>(param));
    }

    /**
     * @brief Überschreibt das Event, das beim korrekten Beenden der Statemaschine emittiert wird.
     * @param eventOnGracefulShutdown_ Zu emittierendes Event.
     *
     * Mit dieser Funktion kann das Event, welches automatisch beim erfolgreichen
     * Beenden der Statemachine emittiert wird, überschrieben werden. Dies gilt für
     * die aktuelle Ausführung der Statemachine, bei einem erneuten Start der Statemachine
     * gilt wieder das im Konstruktor übergebene.
     *
     * \note Es ist nicht möglich, die Ausgabe des Events zu unterdrücken,
     * indem nullptr an diese Funktion übergeben wird. Wird nullptr übergeben, tut diese
     * Funktion nichts.
     */
    void overrideEventOnGracefulShutdown(const EventClass*  eventOnGracefulShutdown_) {
        eventOnGracefulShutdown = eventOnGracefulShutdown_;
    }

    /**
     * @brief Überschreibt das Event, das beim Fehler-Abbruch der Statemaschine emittiert wird.
     * @param eventOnErrorShutdown_ Zu emittierendes Event.
     *
     * Mit dieser Funktion kann das Event, welches automatisch beim durch einen Fehler
     * ausgelösten Abbruch der Statemachine emittiert wird, überschrieben werden. Dies gilt für
     * die aktuelle Ausführung der Statemachine, bei einem erneuten Start der Statemachine
     * gilt wieder das im Konstruktor übergebene.
     *
     * \note Es ist nicht möglich, die Ausgabe des Events zu unterdrücken,
     * indem nullptr an diese Funktion übergeben wird. Wird nullptr übergeben, tut diese
     * Funktion nichts.
     */
    void overrideEventOnErrorShutdown(const EventClass*  eventOnErrorShutdown_) {
        eventOnErrorShutdown = eventOnErrorShutdown_;
    }

    /**
     * \brief State-function modeling the pre-condition of the state.
     * \details Implement this abstract function with the functionality required to check
     * whether all preconditions needed to enter the state are fulfilled and all actions that should be
     * performed once during the activity of the state. This could include things like:
     * - checking the availibilty of a device
     * - issuing control commands to a device
     * - starting a child statemachine
     *
     * \return True to proceed normally. False to stop the statemachine due to an
     * error for whatever reason.
     */
    virtual bool state_function(void) = 0;

    /** \brief Transition-function choosing the next state.
     * \details Implement this abstract function with the functionality required
     * to decide which state should be entered next.
     *
     * \note The transition-function is called continuously. Avoid function calls
     * that take a long time to finish as this would be a hindrance to the
     * pseudo-parallel execution of the different active statemachines.
     *
     * \return
     *  - this: indicate that no statechange should take place.
     *  - A pointer to any valid state: indicate a statechange to this state.
     *  - Statemachine::error: indicate that the statemachine handling the
     * current state(s) should be stopped due to an error.
     *  - Statemachine::finished: indicate that statemachine handling the
     * current state(s) should be stopped due to an external request or as a
     * result of the normal state flow.
     *  - Statemachine::restartState: request executing the state function once again.
     */
    virtual State* transition_function(void) = 0;


private:
    void setEventQueue(EventQueue* eventqueue) { eventqueue_ = eventqueue; }

	void setSignal(uintptr_t signal = 0) { signal_ = signal; hasSignal_ = true; }
    void clearSignal(void) { hasSignal_ = false; }

	void setArgument(uintptr_t argument) { argument_ = argument; }

    void setStarttime(SystemTime starttime) { stateStarttime_ = starttime; }
    void setStatemachineStarttime(SystemTime starttime) { statemachineStarttime_ = starttime; }

    const EventClass* getEventOnGracefulShutdownOverride(void) const {
        return eventOnGracefulShutdown;
    }
    const EventClass* getEventOnErrorShutdownOverride(void) const {
        return eventOnErrorShutdown;
    }
    void clearEventOverrides(void) {
        eventOnGracefulShutdown = nullptr;
        eventOnErrorShutdown = nullptr;
    }

    bool hasSignal_;
	uintptr_t signal_;
	uintptr_t argument_;
    SystemTime stateStarttime_;
    SystemTime statemachineStarttime_;

    const EventClass*  eventOnGracefulShutdown;
    const EventClass*  eventOnErrorShutdown;

    EventQueue* eventqueue_;

};

/*!
 * \brief Time-delaying state.
 *
 * This state can be used in the stateflow of a statemachine to delay its
 * execution for a specific amount of time.
 * The length of the delay is specified in the constructor.
 */
class DelayState final : public State {
public:
    /*!
     * \brief DelayState-Constructor
     * \param delay Time between entering this state and activating the next one.
     * \param nextState Pointer to the state that is entered when the delay has passed.
     * \param event Event that is emitted after the delay time has passed.
     */
    DelayState(SystemTime delay, State* const nextState, const EventClass* event = nullptr) :
        State("DelayState"), myDelay(delay), myStartTime(), myNextState(nextState), myEvent(event) {}

private:
    const SystemTime myDelay;
    SystemTime myStartTime;
    State* const myNextState;
    const EventClass* myEvent;

    bool state_function(void) {
        myStartTime = SystemTime::now();
        return true;
    }
    State* transition_function(void) {
        // This was necessary to avoid some trouble in the
        // behaviour of SystemTime on chibi.
        if (myDelay <= 2_ms) {
            if (myEvent) emitEvent(myEvent);
            return myNextState;
        } else if (SystemTime::now() - myStartTime < myDelay) {
            return this;
        } else {
            if (myEvent) emitEvent(myEvent);
            return myNextState;
        }
    }
};


/*!
 * \brief Delays until signal was received.
 *
 * This state can be used in the stateflow of a statemachine to delay its
 * execution until the statemachine received en external signal (the argument of
 * the received signal is ignored).
 */
class WaitForSignalState final : public State {
public:
    /*!
     * \brief WaitForSignalState-Constructor
     * \param nextState Pointer to the state that is entered when the signal was received.
     */
    WaitForSignalState(State* const nextState) :
        State("Wait for signal"), myNextState(nextState) { }

private:
    State* const myNextState;

    bool state_function(void) { return true; }

    State* transition_function(void) {
        if (hasSignal()) {
            return myNextState;
        } else {
            return this;
        }
    }
};


// -------------------------------------------------------------------
// -------------------------------------------------------------------
// ---- Statemachine class
// -------------------------------------------------------------------
// -------------------------------------------------------------------
/*!
 * \brief The %Statemachine class
 *
 * This class represents the end-user interface of a statemachine.
 * It comes with a thread-safe interface to start and stop its execution,
 * send signals to it and to get runtime information.
 *
 * \see im
 */
class Statemachine {
public:
    /*!
     * \brief Status-type of Statemachine-class.
     */
    enum class Status {
        //! Initial state after instantiation.
        none,
        //! %Statemachine is part of activation queue.
        waiting_for_activation,
        //! %Statemachine is active and running.
        running,
        //! %Statemachine is active and running and passed or is in
        //! its init-state.
        running_and_initialized,
        //! %Statemachine is active but will stop soon.
        running_and_waiting_for_deactivation,
        //! %Statemachine is not running. Its last execution resulted in an error.
        stopped_on_error,
        //! %Statemachine is not running. Its last execution was finished error-free.
        stopped_gracefully
    };

    /*!
     * \hideinitializer
     * \brief Special return value for State::transition_function().
     * \details Return this value to request the restart of the currently active state.
     * This has the effect of an anewed execution-attempt of the statemachine. If the call
     * fails, the statemachine is cancelled. The runtime of the state is not affected in anyway.
     */
    static State* const restartState;
    /*!
     * \hideinitializer
     * \brief Special return value for State::transition_function().
     * \details Return this value to request a graceful shutdown of the statemachine.
     * You can also supply this value as the abort-state for a statemachine if there are no
     * actions required prior stopping the statemachine and you want it to stop immediately.
     */
    static State* const finished;
    /*!
     * \hideinitializer
     * \brief Special return value for State::transition_function().
     * \details Return this value to request aborting the statemachine due to an error.
     */
    static State* const error;

    /*!
     * \brief Statemachine-Constructor
     * \details If you supply only nullptr as arguments for the events, the statemachine will not
     * emit any of the three standard events. It is still possible to emit custom events from
     * the statemachine's states.
     * \param pname Name of the statemachine.
     * \param pentrystate Initial state of the statemachine.
     * \param initState Upon the entry of this state the eventOnSuccessfulInitialization-event
     * is emitted (if there is an eventqueue and the statemachine is not started silently).
     * \param eventOnSuccessfulInitialization %Event that is emitted upon entry of the initState.
     * \param pabortstate %State to be executed when the statemachin should be stopped.
     * You can use Statemachine::finished if you don't need to supply custom actions prior the shutdown
     * of the statemachine.
     * \param eventOnGracefulShutdown %Event that is emitted when the statemachine was stopped
     * without an error.
     * \param eventOnErrorShutdown %Event that is emitted when the statemachine was unexpectedly cancelled
     * due to an error.
     */
    Statemachine(
            const char* const pname,
            State* const pentrystate,
            State* const initState = nullptr,
            const EventClass* const eventOnSuccessfulInitialization = nullptr,
            State* const pabortstate = Statemachine::finished,
            const EventClass* const eventOnGracefulShutdown = nullptr,
            const EventClass* const eventOnErrorShutdown = nullptr);


    // thread-safe access functions
    /*!
     * \brief Start the statemachine.
     * \details The started statemachine will use the default eventqueue to emit events.
     * \param argument Optional argument to execute the statemachine with.
     * \param supressStatechangeDebugMessages Supress the automatic output of debug
     * messages informing about state changes.
     * \remark This function is thread-safe.
     */
	void start(uintptr_t argument = 0, bool supressStatechangeDebugMessages = false);

    /*!
     * \brief Start the statemachine.
     * \param eventqueue Eventqueue the statemachine should use for emitting events.
     * \param argument Optional argument to execute the statemachine with.
     * \param supressStatechangeDebugMessages Supress the automatic output of debug
     * messages informing about state changes.
     * \remark This function is thread-safe.
     */
	void start(EventQueue* eventqueue, uintptr_t argument = 0, bool supressStatechangeDebugMessages = false);

    /*!
     * \brief Start the statemachine in silent mode (no event will ever be emitted).
     * \param argument Optional argument to execute the statemachine with.
     * \param supressStatechangeDebugMessages Supress the automatic output of debug
     * messages informing about state changes.
     * \remark This function is thread-safe.
     */
	void startSilent(uintptr_t argument = 0, bool supressStatechangeDebugMessages = false) {
        start(nullptr, argument, supressStatechangeDebugMessages);
    }

    /*!
     * \brief Stop the statemachine.
     * \details Enqueue the statemachine for stopping. Depending on the statemachine's
     * stateflow the stopping process itself can consist of several states and therefore
     * take a considerable amount of time.
     * \remark This function is thread-safe.
     */
    void stop(void);

    /*!
     * \brief Sends signal to the currently active state of the running statemachine.
     * \param signal Optional argument to the signal.
     * \return false if the statemachine is not running, true otherwise.
     * \remark This function is thread-safe.
     */
	bool sendSignal(uintptr_t signal = 0);

    /*!
     * \brief Returns whether the statemachine is active.
     * \details A statemachine is considered active if its status equals one of the follwoing values:
     * - Status::waiting_for_activation
     * - Status::running
     * - Status::running_and_initialized
     * - Status::running_and_waiting_for_deactivation
     *
     * \return true if active, false otherwise.
     * \remark This function is thread-safe.
     */
    bool isActive(void);

    /*!
     * \brief Returns whether the statemachine is running.
     * \details A statemachine is considered running if its status equals one of the follwoing values:
     * - Status::running
     * - Status::running_and_initialized
     * - Status::running_and_waiting_for_deactivation
     *
     * \return true if running, false otherwise.
     * \remark This function is thread-safe.
     */
    bool isRunning(void);

    /*!
     * \brief Returns the status of the statemachine.
     * \return The statemachine's status.
     * \remark This function is thread-safe.
     */
    Status getStatus(void);

    /*!
     * \brief Returns the runtime of the statemachine.
     * \return 0 if the statemachine is not running, otherwise the time since it started
     * running until now.
     * \remark This function is thread-safe.
     */
    SystemTime getRunningTime(void);

    /*!
     * \brief Sets the default eventqueue for statemachines.
     * \param eventqueue Eventqueue to start a statemachine with if no
     * one is given to the start-function.
     * \remark This function is thread-safe.
     */
    static void setDefaultEventqueue(EventQueue* eventqueue);

    // call this from IM-thread
    /*!
     * \brief Process the statemachines and the activation-/deactivation queues.
     * \details In the context of this function all the heavy lifting is done.
     * It processes the queues for activation and deactivation and executes the
     * statemachines that are active.
     * \note This function needs to be called continuously, preferrably in its own thread.
     */
    static void doStatemachineProcessing(void);

private:
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


    // doubly linked list to manage active statemachines
    static Statemachine* first_active_statemachine;
    Statemachine* next_active;
    Statemachine* last_active;

    // doubly linked list for statemachines that should be activated
    // we need a doubly linked list to enable the activation of
    // statemachines from within statemachines
    static Statemachine* first_to_be_activated_statemachine;
    static Statemachine* last_to_be_activated_statemachine;
    Statemachine* next_to_be_activated;
    Statemachine* previous_to_be_activated;

    // singly linked list for statemachines, that should be stopped
    static Statemachine* first_to_be_stopped_statemachine;
    Statemachine* next_to_be_stopped;


    const char* const name;
    SystemTime startTime;
    Status status_;
	uintptr_t argument_;
    bool hasSignal_;
	uintptr_t signal_;
    State* pcurrent_state;
    State * const entrystate;
    State * const initializedState;
    State * const abortstate;

    const EventClass* const myEventOnSuccessfulInitialization;
    const EventClass* const myEventOnGracefulShutdown;
    const EventClass* const myEventOnErrorShutdown;

    const EventClass* myEventOnGracefulShutdownOverride;
    const EventClass* myEventOnErrorShutdownOverride;

    static EventQueue* defaultEventqueue_;
    EventQueue* eventqueue_;

    bool supressStatechangeDebugMessages;
};




#ifndef __DOXYGEN__
class RedoState final : public State {
public:
    RedoState() : State("Error") {}
protected:
    bool state_function(void) { return false; }
    State* transition_function(void) { return nullptr; }
};

class FinishedState final : public State {
public:
    FinishedState() : State("Finished") {}
protected:
    bool state_function(void) { return true; }
    State* transition_function(void) { return Statemachine::finished; }
};

#endif


//!@}

} // namespace IM
} // namespace TURAG

#endif //IM_STATEMACHINE_H
