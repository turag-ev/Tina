/**
 * \file state.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief a finit state machine
 *
 */

#ifndef TINAPP_STATEMACHINE_STATE_H
#define TINAPP_STATEMACHINE_STATE_H

#include <tina++/tina.h>
#include "types.h"

IGNORE_WARNING("-Wunused-parameter");

namespace TURAG {

/// @addtogroup StateMaschines
/// @{
///
/// @defgroup Actions Aktionen [C++]
/// \brief Aktionen sind Zustandsmaschinen, die auf Events reagieren.
///
/// \msc
/// Sender,Receiver;
/// Sender->Receiver [label="Command()", URL="\ref Receiver::Command()"];
/// Sender<-Receiver [label="Ack()", URL="\ref Ack()", ID="1"];
/// \endmsc
/// @{


////////////////////////////////////////////////////////////////////////////////
//     Action State Model

class Action;

/// Every state is a static function
/// \param id id of the event to be processed
/// \param data extra information for the event
/// \returns true, if the event was handled by the function
typedef bool (*State)(EventId id, EventArg data);

/// Action class
class Action {
public:
  enum {
    /// Event id when state is started
    event_start = EventNamespace('A', 'c', 't') + 0,

    /// Event id when action is about to be closed from outside of the action
    /// with the \a Action::cancel function.
    /// If state function returns true: Action must be exited as soon as possible
    /// with \a exit function. If state function returns false: action is closed
    /// immediately after state function returns
    event_cancel,

    /// Event id when child action exited. exit code is passed in data parameter
    event_return = EventNamespace('A', 'c', 't') + 100,
    /// Exit code for successfully executed action
    event_success,
    /// Exit code for failure in executed action
    event_failure
  };

  /// Create Action.
  /// ACTION_CLASS or STATIC_ACTION_CLASS should be used for creating actions.
  /// \param startstate initial condition by start of action
  /// \param name       name of action for debugging
  constexpr
  Action(State startstate, const char* name) :
    parent_(nullptr),
    currentstate_(0),
    startstate_(startstate),
    name_(name),
    child_(0),
    about_to_close_(false),
    can_be_added_to_blacklist_(true)
  { }

  /// Start action
  void start(Action* parent, EventArg data = 0);

  /// Process event by active state of action
  bool func(EventId id, EventArg data);

  /// Return whether action is active.
  // Achtung: Kann sein, das Aktion nicht mehr aktiv ist, aber Kinderaktionen sind gerade beim Beenden.
  //          Dann liefert isActive() false, aber event_return wurde noch nicht gesendet.
  constexpr _always_inline
  bool isActive() const {
    return currentstate_ != nullptr;
  }


  constexpr _always_inline
  State getState() const {
    return currentstate_;
  }  

  /// Cancel action and child action (when existing)
  /// \returns false, if action cannot be closed immediately.
  ///          state function is called with Action::event_return of action is closed
  ///          if action itself cannot be closed immediately, then isActive() returns true
  ///          if a child action of the action cannot be closed immediately, then isActive() returns false
  bool cancel();

  /// Set child action.
  /** NOTE: Only one child action per action can be active.
   *  NOTE: Do not this function, if \a event_cancel was called to the state function
   * \param child child action to start
   * \param data  parameter for extra data passed with Action::event_start event
   *              when first state of child action is called.
   */
  void setChildAction(Action* child, EventArg data = 0);

  /// Return pointer to the child action or nullptr when no exists.
  constexpr _always_inline
  const Action* getConstChildAction() const {
    return child_;
  }

  /// get name of Action
  const char* getName() const {
    return name_;
  }

  /** define if Action can be added to the Troubleshooter's blacklist
   * this is useful for actions with dynamic entry points.
   */
  constexpr _always_inline
  bool canBeAddedToBlacklist(void) {
    return can_be_added_to_blacklist_;
  }

protected:

  /// Call active state of parent action
  /**
   * \param id event id passed to state of parent action
   * \param data paramter for extra data passed to state of parent action
   */
  _always_inline
  bool callParent(EventId id, EventArg data) const {
    return parent_->func(id , data);
  }

  /// Start the next state of action
  /** Sets next active state and calls next state with a Action::event_start event.
   *  NOTE: child action should be cancelable with cancel() == true or should not exist
   * \param next next state
   * \param data parameter for extra data passed to next state in Action::event_start
   */
  void nextState(State next, EventArg data = 0);

  /// Exit action and return to parent with a return code
  /**
   * \param eid Return code for parent.
   *            Is passed to parent in \a Action::event_return as data parameter.
   * \returns false, if action cannot be closed immediately.
   *          state function is called with Action::event_return of action is closed
   *          if action itself cannot be closed immediately, then isActive() returns true
   *          if a child action of the action cannot be closed immediately, then isActive() returns false
   *
   * Usage example in state of parent action:
   * \code{.cpp}
   * switch (id) {
   * case Action::event_return:
   *   return_code = unpack<EventId>(data);
   *   break;
   * }
   * \endcode
   */
  bool exit(EventId eid);

  /// Check whether \a current is the active state of the action.
  constexpr _always_inline
  bool checkState(State current) const {
    return currentstate_ == current;
  }

  constexpr _always_inline
  Action* getChildAction() {
    return child_;
  }

  /// exit all child actions (use with caution!). Call getChildAction->cancel to
  /// cancel a child action correct.
  void killChildActions();

  /// set to false to avoid being put into the Troubleshooter blacklist
  _always_inline
  void setCanBeAddedToBlacklist(bool val) {
      can_be_added_to_blacklist_ = val;
  }

private:
  Action* parent_;
  State currentstate_;
  const State startstate_;
  const char* const name_;
  Action* child_;
  bool about_to_close_;
  bool can_be_added_to_blacklist_;
};

////////////////////////////////////////////////////////////////////////////////
//     StaticAction

template<typename A>
class StaticAction: public Action {
public:
  constexpr
  StaticAction(State start_state, const char* name) :
    Action(start_state, name)
  { }

  static _always_inline A& get() {
    return *getPointer();
  }

  static _always_inline A* getPointer() {
    return &instance;
  }

  static _always_inline bool isActive() {
    return instance.Action::isActive();
  }

  static _always_inline bool cancel() {
    return instance.Action::cancel();
  }

  static _always_inline bool func(EventId id, EventArg data) {
    return instance.Action::func(id, data);
  }

  static _always_inline void start(Action* parent, EventArg data) {
    instance.Action::start(parent, data);
  }

  constexpr _always_inline bool canBeAddedToBlacklist(void) {
    return instance.Action::canBeAddedToBlacklist();
  }

protected:
  static _always_inline bool exit(EventId eid) {
    return instance.Action::exit(eid);
  }

  static _always_inline void nextState(State next, EventArg data = 0) {
    instance.Action::nextState(next, data);
  }

  static _always_inline bool callParent(EventId id, EventArg data) {
    return instance.Action::callParent(id, data);
  }

  constexpr
  static _always_inline bool checkState(State current) {
    return instance.Action::checkState(current);
  }

  static _always_inline void setChildAction(Action* child, EventArg data = 0) {
    instance.Action::setChildAction(child, data);
  }

  static _always_inline Action* getChildAction() {
    return instance.Action::getChildAction();
  }

  static _always_inline void setCanBeAddedToBlacklist(bool val) {
    instance.Action::setCanBeAddedToBlacklist(val);
  }

private:
  static A instance;
};

template<typename A>
A StaticAction<A>::instance;



#ifdef __DOXYGEN__

/// \brief Aktionsklasse definieren
#define ACTION_CLASS(name)

/// \brief Zustände für Aktion definieren
#define ACTION_STATES(name, s1, ...)

/// \brief einfache Aktionsklasse mit einem Makro
#define SIMPLE_ACTION_CLASS(name, ...) \
  ACTION_CLASS(name) { \
    ACTION_STATES(name, __VA_ARGS__) \
  }

#else // __DOXYGEN__

#define ACTION_CLASS(name) \
  class name : public StaticAction<name>

#define ACTION_STATES(name, s1, ...) \
  private: \
    ACTION_I_DECL(__NARG__(s1, ## __VA_ARGS__)) (s1, ## __VA_ARGS__); \
  public: \
    name() : \
      StaticAction<name>(s1, #name) \
    { } \
  private:

#define SIMPLE_ACTION_CLASS(name, ...) \
  ACTION_CLASS(name) { \
    ACTION_STATES(name, __VA_ARGS__) \
  }

// intern macros
//#define ACTION_I_DECL(...)
#define ACTION_I_DECL(n) ACTION_I_DECL_I(n)
#define ACTION_I_DECL_I(n) ACTION_I_DECL ## n

#define ACTION_I_DECL1(s1) \
    static bool s1 (EventId, EventArg)
#define ACTION_I_DECL2(s1, s2) \
    ACTION_I_DECL1(s1); \
    static bool s2 (EventId, EventArg)
#define ACTION_I_DECL3(s1, s2, s3) \
    ACTION_I_DECL2(s1, s2); \
    static bool s3(EventId, EventArg)
#define ACTION_I_DECL4(s1, s2, s3, s4) \
    ACTION_I_DECL3(s1, s2, s3); \
    static bool s4(EventId, EventArg)
#define ACTION_I_DECL5(s1, s2, s3, s4, s5) \
    ACTION_I_DECL4(s1, s2, s3, s4); \
    static bool s5 (EventId, EventArg)
#define ACTION_I_DECL6(s1, s2, s3, s4, s5, s6) \
    ACTION_I_DECL5(s1, s2, s3, s4, s5); \
    static bool s6 (EventId, EventArg)
#define ACTION_I_DECL7(s1, s2, s3, s4, s5, s6, s7) \
    ACTION_I_DECL6(s1, s2, s3, s4, s5, s6); \
    static bool s7 (EventId, EventArg)
#define ACTION_I_DECL8(s1, s2, s3, s4, s5, s6, s7, s8) \
    ACTION_I_DECL7(s1, s2, s3, s4, s5, s6, s7); \
    static bool s8 (EventId, EventArg)
#define ACTION_I_DECL9(s1, s2, s3, s4, s5, s6, s7, s8, s9) \
    ACTION_I_DECL8(s1, s2, s3, s4, s5, s6, s7, s8); \
    static bool s9 (EventId, EventArg)
#define ACTION_I_DECL10(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10) \
    ACTION_I_DECL9(s1, s2, s3, s4, s5, s6, s7, s8, s9); \
    static bool s10 (EventId, EventArg)
#define ACTION_I_DECL11(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11) \
    ACTION_I_DECL10(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10); \
    static bool s11 (EventId, EventArg)
#define ACTION_I_DECL12(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12) \
    ACTION_I_DECL11(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11); \
    static bool s12 (EventId, EventArg)
#define ACTION_I_DECL13(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13) \
    ACTION_I_DECL12(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12); \
    static bool s13 (EventId, EventArg)
#define ACTION_I_DECL14(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14) \
    ACTION_I_DECL13(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13); \
    static bool s14 (EventId, EventArg)
#define ACTION_I_DECL15(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15) \
    ACTION_I_DECL14(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14); \
    static bool s15 (EventId, EventArg)
#define ACTION_I_DECL16(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16) \
    ACTION_I_DECL15(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15); \
    static bool s16 (EventId, EventArg)
#define ACTION_I_DECL17(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17) \
    ACTION_I_DECL16(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16); \
    static bool s17 (EventId, EventArg)
#define ACTION_I_DECL18(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18) \
    ACTION_I_DECL17(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17); \
    static bool s18 (EventId, EventArg)
#define ACTION_I_DECL19(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19) \
    ACTION_I_DECL18(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18); \
    static bool s19 (EventId, EventArg)
#define ACTION_I_DECL20(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20) \
    ACTION_I_DECL19(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19); \
    static bool s20 (EventId, EventArg)
#define ACTION_I_DECL21(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21) \
    ACTION_I_DECL20(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20); \
    static bool s21 (EventId, EventArg)
#define ACTION_I_DECL22(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22) \
    ACTION_I_DECL21(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21); \
    static bool s22 (EventId, EventArg)
#define ACTION_I_DECL23(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23) \
    ACTION_I_DECL22(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22); \
    static bool s23 (EventId, EventArg)
#define ACTION_I_DECL24(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24) \
    ACTION_I_DECL23(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23); \
    static bool s24 (EventId, EventArg)
#define ACTION_I_DECL25(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25) \
    ACTION_I_DECL24(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24); \
    static bool s25 (EventId, EventArg)
#define ACTION_I_DECL26(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26) \
    ACTION_I_DECL25(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25); \
    static bool s26 (EventId, EventArg)
#define ACTION_I_DECL27(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27) \
    ACTION_I_DECL26(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26); \
    static bool s27 (EventId, EventArg)
#define ACTION_I_DECL28(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28) \
    ACTION_I_DECL27(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27); \
    static bool s28 (EventId, EventArg)
#define ACTION_I_DECL29(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29) \
    ACTION_I_DECL28(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28); \
    static bool s29 (EventId, EventArg)
#define ACTION_I_DECL30(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30) \
    ACTION_I_DECL29(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29); \
    static bool s30 (EventId, EventArg)
#define ACTION_I_DECL31(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31) \
    ACTION_I_DECL30(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30); \
    static bool s31 (EventId, EventArg)
#define ACTION_I_DECL32(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32) \
    ACTION_I_DECL31(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31); \
    static bool s32 (EventId, EventArg)
#define ACTION_I_DECL33(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33) \
    ACTION_I_DECL32(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32); \
    static bool s33 (EventId, EventArg)
#define ACTION_I_DECL34(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34) \
    ACTION_I_DECL33(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33); \
    static bool s34 (EventId, EventArg)
#define ACTION_I_DECL35(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35) \
    ACTION_I_DECL34(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34); \
    static bool s35 (EventId, EventArg)
#define ACTION_I_DECL36(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36) \
    ACTION_I_DECL35(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35); \
    static bool s36 (EventId, EventArg)
#define ACTION_I_DECL37(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37) \
    ACTION_I_DECL36(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36); \
    static bool s37 (EventId, EventArg)
#define ACTION_I_DECL38(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38) \
    ACTION_I_DECL37(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37); \
    static bool s38 (EventId, EventArg)
#define ACTION_I_DECL39(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39) \
    ACTION_I_DECL38(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38); \
    static bool s39 (EventId, EventArg)
#define ACTION_I_DECL40(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40) \
    ACTION_I_DECL39(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39); \
    static bool s40 (EventId, EventArg)
#define ACTION_I_DECL41(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41) \
    ACTION_I_DECL40(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40); \
    static bool s41 (EventId, EventArg)
#define ACTION_I_DECL42(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42) \
    ACTION_I_DECL41(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41); \
    static bool s42 (EventId, EventArg)
#define ACTION_I_DECL43(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43) \
    ACTION_I_DECL42(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42); \
    static bool s43 (EventId, EventArg)
#define ACTION_I_DECL44(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44) \
    ACTION_I_DECL43(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43); \
    static bool s44 (EventId, EventArg)
#define ACTION_I_DECL45(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45) \
    ACTION_I_DECL44(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44); \
    static bool s45 (EventId, EventArg)
#define ACTION_I_DECL46(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46) \
    ACTION_I_DECL45(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45); \
    static bool s46 (EventId, EventArg)
#define ACTION_I_DECL47(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47) \
    ACTION_I_DECL46(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46); \
    static bool s47 (EventId, EventArg)
#define ACTION_I_DECL48(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48) \
    ACTION_I_DECL47(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47); \
    static bool s48 (EventId, EventArg)
#define ACTION_I_DECL49(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49) \
    ACTION_I_DECL48(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48); \
    static bool s49 (EventId, EventArg)
#define ACTION_I_DECL50(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50) \
    ACTION_I_DECL49(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49); \
    static bool s50 (EventId, EventArg)
#define ACTION_I_DECL51(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51) \
    ACTION_I_DECL50(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50); \
    static bool s51 (EventId, EventArg)
#define ACTION_I_DECL52(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52) \
    ACTION_I_DECL51(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51); \
    static bool s52 (EventId, EventArg)
#define ACTION_I_DECL53(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53) \
    ACTION_I_DECL52(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52); \
    static bool s53 (EventId, EventArg)
#define ACTION_I_DECL54(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53, s54) \
    ACTION_I_DECL53(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53); \
    static bool s54 (EventId, EventArg)
#define ACTION_I_DECL55(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53, s54, s55) \
    ACTION_I_DECL54(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53, s54); \
    static bool s55 (EventId, EventArg)

#endif // __DOXYGEN__

/// \}
/// \}

} // namespace TURAG

#endif // TINAPP_STATEMACHINE_STATE_H
