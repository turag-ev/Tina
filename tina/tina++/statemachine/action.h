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
#include <type_traits>
#include "types.h"

namespace TURAG {

class Action {
public:
    enum {
        event_start = EventNamespace('A', 'c', 't'),
        event_cancel,
        event_return = EventNamespace('A', 'c', 't') + 100,
    };


    enum {
        event_success,
        event_failure
    };

    Action(const char* name);

    const char* getName() const {
        return name_;
    }

    bool func(EventId id, EventArg arg);

    _always_inline
    bool isActive() const {
        return active_;
    }

    void cancel(EventArg arg = 0);
    void kill();

    void startChildAction(Action* child, EventArg arg = 0) {
        child->start(this, arg);
    }

    _always_inline
    const Action* getChildAction() const {
        return child_;
    }

    _always_inline
    bool hasChildAction() const {
        return child_ != nullptr;
    }

    _always_inline
    const Action* getParentAction() const {
        return parent_;
    }
    void start(EventArg data = 0) {
        start(nullptr, data);
    }

protected:
    _always_inline Action* getChildAction() {
        return child_;
    }

    virtual bool handleEvent(EventId id, EventArg arg) = 0;
    virtual void onStart(EventArg arg) = 0;
    void exit(EventId eid);
private:
    void start(Action* parent, EventArg data = 0);

    const char* const name_;
    bool active_;
    Action* parent_;
    Action* child_;
    Action* getInnermostChild();
};

template<class A>
class StateAction: public Action {
public:
    typedef bool (A::*State)(EventId id, EventArg data);

    constexpr
    StateAction(State startstate, const char* name):
        Action(name),
        currentstate_(nullptr),
        startstate_(startstate)
    { }

    void onStart(EventArg arg) {
        currentstate_ = startstate_;
        state(Action::event_start, arg);
    }

    void nextState(State next, EventArg arg = 0) {
        currentstate_ = next;
        state(Action::event_start, arg);
    }

    bool handleEvent(EventId id, EventArg arg) override {
        if(!currentstate_) //TODO error
            return false;
        state(id, arg);
    }

    static A* getPointer() __attribute_deprecated__ {
        return &instance;
    }
    /*static A& get() __attribute_deprecated__ {
        return instance;
    }*/
private:
    _always_inline bool state(EventId id, EventArg arg) {
        static_assert(std::is_base_of<StateAction<A>, A>::value, "");
        return (reinterpret_cast<A*>(this)->*currentstate_)(id, arg);
    }

    State currentstate_;
    const State startstate_;
    static A instance;
};

template<typename A>
A StateAction<A>::instance;



#ifdef __DOXYGEN__

/// \brief Aktionsklasse definieren
///
/// Über dieses Makro wird eine Klasse definiert, die von der Klasse
/// \ref TURAG::StaticAction erbt und ist equivalent zu `class name : public StaticAction<name>`.
/// Um den Konstruktor und die nötigen Zustandsfunktionen zu definieren
/// benötigt man \ref ACTION_STATES :
/// \code
/// ACTION_CLASS(MyAction) {
///   ACTION_STATES(MyAction, start_state, one_more_state, end_state)
/// };
///
/// // Initialzustand
/// bool MyAction::start_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
/// }
///
/// // weiterer Zustand
/// bool MyAction::one_more_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
/// }
///
/// // ein weiterer Zustand
/// bool MyAction::end_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
/// }
/// \endcode
///
/// \param name Klassenname, der zuerstellenden Aktionsklasse
/// \sa Action, StaticAction, ACTION_STATES, SIMPLE_ACTION_CLASS
#define ACTION_CLASS(name) class name : public StaticAction<name>

/// \brief Zustände für Aktion definieren
///
/// \code
/// ACTION_CLASS(MyAction) {
///   ACTION_STATES(MyAction, start_state, one_more_state, end_state)
/// };
///
/// // Initialzustand
/// bool MyAction::start_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
///   return false;
/// }
///
/// // weiterer Zustand
/// bool MyAction::one_more_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
/// }
///
/// // ein weiterer Zustand
/// bool MyAction::end_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
/// }
/// \endcode
///
/// \param name Klassenname, der zuerstellenden Aktionsklasse
/// \param s1 Name von zu erstellender Initialzustandsfunktion
/// \param ... optional weitere Namen von zu erstellenden Zustandsfunktionen
/// \sa Action, StaticAction, ACTION_CLASS, SIMPLE_ACTION_CLASS
#define ACTION_STATES(name, s1, ...)

/// \brief einfache Aktionsklasse mit einem Makro
///
/// Äquivalent zu dem Beispiel in \ref ACTION_CLASS und \ref ACTION_STATES
/// \code
/// SIMPLE_ACTION_CLASS(MyAction, start_state, one_more_state, end_state);
///
/// // Initialzustand
/// bool MyAction::start_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
/// }
///
/// // weiterer Zustand
/// bool MyAction::one_more_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
/// }
///
/// // ein weiterer Zustand
/// bool MyAction::end_state(EventId id, EventArg data) {
///   // Ereignisse verarbeiten ...
/// }
/// \endcode
///
/// \param name Klassenname, der zuerstellenden Aktionsklasse
/// \param s1 Name von zu erstellender Initialzustandsfunktion
/// \param ... optional weitere Namen von zu erstellenden Zustandsfunktionen
/// \sa Action, StaticAction, ACTION_CLASS, ACTION_STATES
#define SIMPLE_ACTION_CLASS(name, s1, ...)

#else // __DOXYGEN__

#define ACTION_CLASS(name) \
    class name : public StateAction<name>

#define ACTION_STATES(name, s1, ...) \
    private: \
    ACTION_I_DECL(__NARG__(s1, ## __VA_ARGS__)) (s1, ## __VA_ARGS__); \
    public: \
    name() : \
    StateAction<name>(&name::s1, #name) \
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
    bool s1 (EventId, EventArg)
#define ACTION_I_DECL2(s1, s2) \
    ACTION_I_DECL1(s1); \
    bool s2 (EventId, EventArg)
#define ACTION_I_DECL3(s1, s2, s3) \
    ACTION_I_DECL2(s1, s2); \
    bool s3(EventId, EventArg)
#define ACTION_I_DECL4(s1, s2, s3, s4) \
    ACTION_I_DECL3(s1, s2, s3); \
    bool s4(EventId, EventArg)
#define ACTION_I_DECL5(s1, s2, s3, s4, s5) \
    ACTION_I_DECL4(s1, s2, s3, s4); \
    bool s5 (EventId, EventArg)
#define ACTION_I_DECL6(s1, s2, s3, s4, s5, s6) \
    ACTION_I_DECL5(s1, s2, s3, s4, s5); \
    bool s6 (EventId, EventArg)
#define ACTION_I_DECL7(s1, s2, s3, s4, s5, s6, s7) \
    ACTION_I_DECL6(s1, s2, s3, s4, s5, s6); \
    bool s7 (EventId, EventArg)
#define ACTION_I_DECL8(s1, s2, s3, s4, s5, s6, s7, s8) \
    ACTION_I_DECL7(s1, s2, s3, s4, s5, s6, s7); \
    bool s8 (EventId, EventArg)
#define ACTION_I_DECL9(s1, s2, s3, s4, s5, s6, s7, s8, s9) \
    ACTION_I_DECL8(s1, s2, s3, s4, s5, s6, s7, s8); \
    bool s9 (EventId, EventArg)
#define ACTION_I_DECL10(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10) \
    ACTION_I_DECL9(s1, s2, s3, s4, s5, s6, s7, s8, s9); \
    bool s10 (EventId, EventArg)
#define ACTION_I_DECL11(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11) \
    ACTION_I_DECL10(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10); \
    bool s11 (EventId, EventArg)
#define ACTION_I_DECL12(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12) \
    ACTION_I_DECL11(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11); \
    bool s12 (EventId, EventArg)
#define ACTION_I_DECL13(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13) \
    ACTION_I_DECL12(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12); \
    bool s13 (EventId, EventArg)
#define ACTION_I_DECL14(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14) \
    ACTION_I_DECL13(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13); \
    bool s14 (EventId, EventArg)
#define ACTION_I_DECL15(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15) \
    ACTION_I_DECL14(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14); \
    bool s15 (EventId, EventArg)
#define ACTION_I_DECL16(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16) \
    ACTION_I_DECL15(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15); \
    bool s16 (EventId, EventArg)
#define ACTION_I_DECL17(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17) \
    ACTION_I_DECL16(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16); \
    bool s17 (EventId, EventArg)
#define ACTION_I_DECL18(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18) \
    ACTION_I_DECL17(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17); \
    bool s18 (EventId, EventArg)
#define ACTION_I_DECL19(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19) \
    ACTION_I_DECL18(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18); \
    bool s19 (EventId, EventArg)
#define ACTION_I_DECL20(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20) \
    ACTION_I_DECL19(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19); \
    bool s20 (EventId, EventArg)
#define ACTION_I_DECL21(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21) \
    ACTION_I_DECL20(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20); \
    bool s21 (EventId, EventArg)
#define ACTION_I_DECL22(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22) \
    ACTION_I_DECL21(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21); \
    bool s22 (EventId, EventArg)
#define ACTION_I_DECL23(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23) \
    ACTION_I_DECL22(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22); \
    bool s23 (EventId, EventArg)
#define ACTION_I_DECL24(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24) \
    ACTION_I_DECL23(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23); \
    bool s24 (EventId, EventArg)
#define ACTION_I_DECL25(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25) \
    ACTION_I_DECL24(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24); \
    bool s25 (EventId, EventArg)
#define ACTION_I_DECL26(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26) \
    ACTION_I_DECL25(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25); \
    bool s26 (EventId, EventArg)
#define ACTION_I_DECL27(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27) \
    ACTION_I_DECL26(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26); \
    bool s27 (EventId, EventArg)
#define ACTION_I_DECL28(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28) \
    ACTION_I_DECL27(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27); \
    bool s28 (EventId, EventArg)
#define ACTION_I_DECL29(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29) \
    ACTION_I_DECL28(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28); \
    bool s29 (EventId, EventArg)
#define ACTION_I_DECL30(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30) \
    ACTION_I_DECL29(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29); \
    bool s30 (EventId, EventArg)
#define ACTION_I_DECL31(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31) \
    ACTION_I_DECL30(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30); \
    bool s31 (EventId, EventArg)
#define ACTION_I_DECL32(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32) \
    ACTION_I_DECL31(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31); \
    bool s32 (EventId, EventArg)
#define ACTION_I_DECL33(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33) \
    ACTION_I_DECL32(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32); \
    bool s33 (EventId, EventArg)
#define ACTION_I_DECL34(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34) \
    ACTION_I_DECL33(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33); \
    bool s34 (EventId, EventArg)
#define ACTION_I_DECL35(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35) \
    ACTION_I_DECL34(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34); \
    bool s35 (EventId, EventArg)
#define ACTION_I_DECL36(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36) \
    ACTION_I_DECL35(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35); \
    bool s36 (EventId, EventArg)
#define ACTION_I_DECL37(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37) \
    ACTION_I_DECL36(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36); \
    bool s37 (EventId, EventArg)
#define ACTION_I_DECL38(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38) \
    ACTION_I_DECL37(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37); \
    bool s38 (EventId, EventArg)
#define ACTION_I_DECL39(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39) \
    ACTION_I_DECL38(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38); \
    bool s39 (EventId, EventArg)
#define ACTION_I_DECL40(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40) \
    ACTION_I_DECL39(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39); \
    bool s40 (EventId, EventArg)
#define ACTION_I_DECL41(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41) \
    ACTION_I_DECL40(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40); \
    bool s41 (EventId, EventArg)
#define ACTION_I_DECL42(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42) \
    ACTION_I_DECL41(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41); \
    bool s42 (EventId, EventArg)
#define ACTION_I_DECL43(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43) \
    ACTION_I_DECL42(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42); \
    bool s43 (EventId, EventArg)
#define ACTION_I_DECL44(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44) \
    ACTION_I_DECL43(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43); \
    bool s44 (EventId, EventArg)
#define ACTION_I_DECL45(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45) \
    ACTION_I_DECL44(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44); \
    bool s45 (EventId, EventArg)
#define ACTION_I_DECL46(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46) \
    ACTION_I_DECL45(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45); \
    bool s46 (EventId, EventArg)
#define ACTION_I_DECL47(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47) \
    ACTION_I_DECL46(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46); \
    bool s47 (EventId, EventArg)
#define ACTION_I_DECL48(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48) \
    ACTION_I_DECL47(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47); \
    bool s48 (EventId, EventArg)
#define ACTION_I_DECL49(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49) \
    ACTION_I_DECL48(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48); \
    bool s49 (EventId, EventArg)
#define ACTION_I_DECL50(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50) \
    ACTION_I_DECL49(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49); \
    bool s50 (EventId, EventArg)
#define ACTION_I_DECL51(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51) \
    ACTION_I_DECL50(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50); \
    bool s51 (EventId, EventArg)
#define ACTION_I_DECL52(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52) \
    ACTION_I_DECL51(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51); \
    bool s52 (EventId, EventArg)
#define ACTION_I_DECL53(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53) \
    ACTION_I_DECL52(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52); \
    bool s53 (EventId, EventArg)
#define ACTION_I_DECL54(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53, s54) \
    ACTION_I_DECL53(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53); \
    bool s54 (EventId, EventArg)
#define ACTION_I_DECL55(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53, s54, s55) \
    ACTION_I_DECL54(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33, s34, s35, s36, s37, s38, s39, s40, s41, s42, s43, s44, s45, s46, s47, s48, s49, s50, s51, s52, s53, s54); \
    bool s55 (EventId, EventArg)

#endif // __DOXYGEN__

/// \}
/// \}

} // namespace TURAG

#endif // TINAPP_STATEMACHINE_STATE_H
