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
/// ## Aktion definieren
///
/// Aufbau von Aktion erläutern, Singleton, ...
///
/// ## Zustände definieren
///
/// in ACTION_STATES
///
/// ## Aktion starten
///
/// start() erklären
///
/// ## Zustand wechseln
///
/// nextState() erklären
///
/// ## Aktion beenden
///
/// exit() vs. cancel() erklären
///
/// event_cancel erklären
///
/// ## Kindaktionen
///
/// setChildAction
///
/// Aktionshierachie aus SC erklären
///
/// killAllChildren()
///
/// ## Ereignisse
///
/// ### Ereignisse zuführen
/// Hauptaktion zuführen
///
/// Mehrere Hauptaktionen: Ereignisse filter (TODO: Link) und an andere Hauptaktion weiterleiten
///
/// ### Ereignissverarbeitung
///
/// Ereignisse werden erst in Kindaktion bearbeitet
///
/// Bedeutung von return true/false
///
/// Bedeutung für allgemeine Verarbeitung (nur verarbeiten was man kann/muss/will)
///
/// ### Spezielle Ereignisse
///
/// Action::event_*
///
/// event_cancel in Hierachie
///
/// ## Richtlinien/Verwendung
///
/// execute-Funktionen
///
/// Beispiele,Beispiele, Beispiele
///
/// minimal Beispiel
///
///
/// \msc
/// Sender,Receiver;
/// Sender->Receiver [label="Command()", URL="\ref Receiver::Command()"];
/// Sender<-Receiver [label="Ack()", URL="\ref Ack()", ID="1"];
/// \endmsc
///
/// \todo verzögertes Beenden noch benötigt? sonst entfernen, weil noch sehr buggy.
/// @{


////////////////////////////////////////////////////////////////////////////////
//     Action State Model

class Action;

/// \brief Typ von Zustand(-sfunktion)
/// \param id ID von Ereignis, was verarbeitet werden soll.
/// \param data zusätzliche Information zu Ereignis
/// \retval true Wenn Ereignis von dieser Funktion verarbeitet wurde.
/// \retval false Wenn Ereignis nicht verarbeitet wurde.
typedef bool (*State)(EventId id, EventArg data);

/// Aktionszustandsmaschine
class Action {
public:
  /// \brief Ereignis-IDs von Ereignissen von Aktion.
  ///
  /// Diese speziellen Ereignisse bieten die Möglichkeiten auf Ereignisse
  /// innerhalb der Aktion und dessen Zustände zu reagieren.
  enum {
    /// \brief Aktion hat neuen Zustand
    ///
    /// Der zustätzliche Datenparameter hat den Wert, des bei \ref start oder
    /// \ref setChildAction übergebenen zweiten Paramters.
    event_start = EventNamespace('A', 'c', 't'),

    /// \brief Aktion wird von außerhalb abgebrochen.
    ///
    /// Dieses Ereigniss tritt auf, wenn Aktion über die Funktion \ref cancel
    /// abgebrochen wird.
    ///
    /// Gibt der aktuelle Zustand dieser Aktion als Reaktion in der Zustandsfunktion
    /// \c true zurück, dann wird die Aktion noch nicht beendet, sondern bekommt die
    /// besondere Wird-Beendet-Eigenschaft. Mit dieser Eigenschaft dürfen
    /// folgende Funktionen nicht aufgerufen werden: \ref cancel, \ref nextState,
    /// \ref start und \ref setChildAction. Darauf sollte so schnell wie möglich
    /// die Aktion entgültig über \ref cancel oder \ref exit beendet werden.
    ///
    /// Wird vom aktuellen Zustand \c false zurückgegeben, wird die Aktion sofort beendet.
    event_cancel,

    /// \brief Kindaktion hat sich beendet.
    ///
    /// Dieses Ereigniss tritt auf, wenn eine durch die Funktion \ref setChildAction
    /// hinzugefügte Kindaktion durch die Funktion \ref exit beendet wurde.
    /// Dieses Ereignis tritt nicht auf, wenn diese Aktion durch die \ref cancel
    /// beendet wird, aber auch nicht wenn die Aktion nach dem \ref event_cancel Ereignis
    /// durch \ref exit beendet wird.
    ///
    /// Der zweite Argument, welcher der Zustandsfunktion bei diesem Event übergeben wird, ist
    /// der Parameter der der \ref exit Funktion übergeben wird, z.B. \ref event_success
    /// oder \ref event_failure.
    event_return = EventNamespace('A', 'c', 't') + 100,
  };

  /// \brief spezielle Rückgabewerte von Aktionen.
  ///
  /// Diese Rückgabewerte können beim Aufruf der \ref exit Funktion verwendet werden.
  enum {
    /// \brief Rückgabewert, wenn Aktion erfolgreich ausgeführt wurde.
    event_success,

    /// \brief Rückgabewert, wenn Aktion nicht erfolgreich ausgeführt wurde.
    event_failure
  };

  /// \brief Aktion erstellen.
  ///
  /// \warning
  /// \ref ACTION_CLASS oder \ref STATIC_ACTION_CLASS sollten für das Erstellen
  /// von Aktionen verwendet werden.
  ///
  /// \param startstate Initalzustand von Aktion.
  /// \param name       Name der Aktion (wird nur für Debuggingzwecke verwendet).
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

  /// \brief Hauptaktion starten
  ///
  /// Die Hauptaktion wird mit dieser Funktion gestartet. Jede weitere Unteraktion
  /// kann mit \ref setChildAction hinzugefügt werden.
  ///
  /// Wird die Aktion gestartet wird der aktuelle Zustand der Aktion auf den
  /// Initalisierungszustand gesetzt (zweiter Parameter in)
  ///
  /// \pre Aktion ist nicht schon aktiv oder wird gerade beendet.
  /// \param data zusätzliche beliebige Informationen, die bei \ref event_start
  ///             als zweiter Parameter beim Starten des ersten Zustands übergeben werden.
  void start(EventArg data = 0) {
    start(nullptr, data);
  }

  /// \brief Ereignis von Aktion verarbeiten
  ///
  /// Wird in der Regel nur benötigt, um Ereignisse der Ereignisschlange (\ref TURAG::EventQueue)
  /// der Hauptaktion zuzuführen.
  ///
  /// \param id Ereignis-ID von Ereignis.
  /// \param data Datenparameter zu Ereignis.
  /// \retval true Ereignis wurde von Aktion verarbeitet.
  /// \retval false Ereignis wurde nicht von Aktion verarbeitet.
  bool func(EventId id, EventArg data);

  /// \brief Aktion ist aktiv.
  constexpr _always_inline
  bool isActive() const {
    return currentstate_ != nullptr;
  }

  /// \brief aktuellen Zustand von Aktion zurückgeben
  /// \return aktueller Zustand von Aktion oder \c nullptr, wenn Aktion nicht
  ///         aktiv (\ref isActive liefert \c false).
  constexpr _always_inline
  State getState() const {
    return currentstate_;
  }  

  /// \brief Bricht Verarbeitung von Aktion und allen Unteraktionen ab.
  ///
  /// Hat diese Aktion Unteraktionen, so werden erst alle Unteraktionen
  /// abgebrochen, bevor diese Aktion abgeborchen wird.
  ///
  /// Wenn die Aktion abgebrochen wird, wird das Ereignis \ref event_cancel
  /// an die Aktion übergeben. Gibt die aktuelle Zustandsfunktion \c true
  /// zurück, so wird die Aktion von sich selber verzögert beendet, was möglichst
  /// schnell gesehen nachdem das Ereignis eingetroffen ist. Wird \c false zurückgegeben,
  /// so wird die Aktion sofort beendet.
  ///
  /// Kann die Aktion nicht sofort beendet werden, so gibt \ref isActive \c false
  /// zurück.
  ///
  /// \todo Wann Benachrichtigtung, dass Aktion beendet wurde???
  ///
  /// \return \c true, falls Aktion sofort abgebrochen werden konnte, sonst \c false.
  ///
  /// \sa exit
  bool cancel();

  /// \brief Unteraktion hinzufügen.
  ///
  /// Jede Aktion kann bis zu eine Unteraktion aufweisen.
  /// Durch diese Funktion wird der Aktion eine Unteraktion hinzugefügt, wenn
  /// noch keine vorhanden ist. Die Unteraktion wird durch diese Funktion
  /// sofort gestartet, siehe \ref start.
  ///
  /// Wird die Unteraktion beendet, wird das Ereignis \ref event_return dem
  /// aktuellen Zustand dieser Aktion übergeben. Im Datenparameter ist der Rückgabewert der
  /// Aktion enthalten.
  ///
  /// Wird der Zustand dieser Aktion gewechselt bzw. diese Aktion beendet oder
  /// abgebrochen, so wird die Unteraktion abgebrochen.
  ///
  /// \pre Aktion hat keine Unteraktion.
  ///
  /// \param child Unteraktion, die hinzugefügt und gestartet werden soll.
  /// \param data Parameter mit extra Daten, der mit dem \ref event_start Ereignis,
  ///              dem Startzustand als Datenparameter übergeben wird.
  ///
  /// \sa start(EventArg)
  ///
  void setChildAction(Action* child, EventArg data = 0);

  /// \brief Gibt eine evtl. vorhandene Unteraktion zurück.
  ///
  /// \return vorhandene Unteraktion oder \c nullptr, wenn keine vorhanden.
  ///
  /// \sa setChildAction
  _always_inline
  const Action* getConstChildAction() const {
    return child_;
  }

  /// \brief Name der Aktion zurückgeben
  ///
  /// Der Name ist für Debuggingzwecke gedacht. Es ist möglich, dass zwei
  /// Aktionen über die gleichen Namen verfügen.
  ///
  /// \return Zeichenkette mit Name der Aktion
  const char* getName() const {
    return name_;
  }

  /// \brief Aktion kann zur Blacklist hinzugefügt werden
  ///
  /// Sagt aus, ob es erlaubt ist die Aktion in die Blacklist des SystemControl
  /// Troubleshooter's hinzu zu fügen. Dies ist nützlich für Aktionen mit
  /// dynamischen Einstiegspunkten.
  constexpr _always_inline
  bool canBeAddedToBlacklist() {
    return can_be_added_to_blacklist_;
  }

protected:

  /// \brief Hauptaktion starten
  ///
  /// Die Hauptaktion wird mit dieser Funktion gestartet. Jede weitere Unteraktion
  /// kann mit \ref setChildAction hinzugefügt werden.
  ///
  /// Wird die Aktion gestartet wird der aktuelle Zustand der Aktion auf den
  /// Initalisierungszustand gesetzt (zweiter Parameter in)
  ///
  /// \pre Aktion ist nicht schon aktiv oder wird gerade beendet.
  /// \param parent Elternaktion oder \c nullptr
  /// \param data zusätzliche beliebige Informationen, die bei \ref event_start
  ///             als zweiter Parameter beim Starten des ersten Zustands übergeben werden.
  void start(Action* parent, EventArg data = 0);

  /// \brief Übergibt übergeordneter Aktion Ereignis
  /// \param id Ereignis-ID zu Ereignis
  /// \param data Datenparamter zu Ereignis
  _always_inline
  bool callParent(EventId id, EventArg data) const {
    return (parent_) ? (parent_->func(id , data)) : (false);
  }

  /// \brief Setzt Zustand von Aktion.
  ///
  /// Durch das Setzen des neuen Zustand werden alle Ereignisse, die dieser
  /// Aktion übergeben werden an den neuen Zustand weitergeleitet. Während
  /// diese Funktion aufgerufen wird, wird das Ereignis \ref event_start
  /// mit dem Parameter \a data an den neuen Zustand übergeben.
  ///
  /// Der Parameter \a next muss eine Methode dieser Aktion sein.
  ///
  /// Evtl. vorhandene Unteraktionen werden abgebrochen.
  ///
  /// \param next zu setzten der Zustand dieser Aktion
  /// \param data optionaler Datenparameter, der mit \ref event_start Ereignis
  ///             bei neuem Zustand übergeben wird.
  ///
  /// \bug Ist eine Unteraktion vorhanden, die verzögert beendet werden muss,
  ///      wird diese trotzdem sofort abgebrochen.
  void nextState(State next, EventArg data = 0);

  /// \brief Beendet Aktion.
  ///
  /// Durch diese Funktion wird die Aktion beendet und evtl. vorhandene Unteraktionen
  /// abgebrochen.
  ///
  /// \param eid Rückgabewert dieser Aktion, z.B. \ref event_success oder \ref event_failure
  /// \return Es wird \c false zurückgegeben, falls die
  bool exit(EventId eid);

  /// \brief Prüft, ob ein Zustand ausgeführt wird
  ///
  /// \return Es wird \c true zurückgegeben, falls \a current die aktuell
  /// von dieser Altion ausgeführte Zustand ist.
  constexpr _always_inline
  bool checkState(State current) const {
    return currentstate_ == current;
  }

  /// \brief Gibt eine evtl. vorhandene Unteraktion zurück.
  ///
  /// \return vorhandene Unteraktion oder \c nullptr, wenn keine vorhanden.
  ///
  /// \sa setChildAction getConstChildAction
  constexpr _always_inline
  Action* getChildAction() {
    return child_;
  }

  /// \brief Unteraktion abwürgen.
  ///
  /// Die Unteraktion und evtl. weitere in der Hierachie darunter folgende Unteraktionen
  /// werden ohne, dass die einzelnen Unteraktionen mit dem Ereignis \ref event_cancel
  /// benachrichtigt werden, abgebrochen.
  ///
  /// Diese Funktion mit Vorsicht benutzen, da die Unteraktionen nicht ordnungsgemäß
  /// beendet werden und evtl. gestartete Aufgaben nicht durch die Unteraktionen
  /// abgebrochen werden können.
  ///
  /// Für das ordnungsgemäße Beenden von der eigenen Aktion \ref exit benutzen und
  /// andere Aktionen mit \ref cancel beenden.
  ///
  /// \sa cancel, exit
  void killChildActions();

  /// \brief Aktion nicht erlauben auf die Blacklist des SystemControl Troubleshooter's
  ///        gesetzt zu werden.
  /// \param val Bei \c true, kann Aktion Auf Blacklist gesetzt werden, sonst nicht.
  ///
  /// \sa canBeAddedToBlacklist
  _always_inline
  void setCanBeAddedToBlacklist(bool val) {
      can_be_added_to_blacklist_ = val;
  }

private:
  /// übergeordnete Aktion oder \c nullptr, wenn Aktion nicht aktiv oder keine
  /// übergeordnete Aktion ist vorhanden.
  Action* parent_;

  /// aktueller Zustand von \c nullptr, wenn Aktion nicht aktiv
  State currentstate_;

  /// Startzustand von Aktion
  const State startstate_;

  /// Name von Aktion
  const char* const name_;

  /// Unteraktion oder \c nullptr, wenn keine vorhanden oder Aktion nicht aktiv
  Action* child_;

  /// Aktion ist noch aktiv wird aber bald beendet
  bool about_to_close_;

  /// Aktion kann auf Blacklist gesetzt werden
  bool can_be_added_to_blacklist_;
};

// /////////////////////////////////////////////////////////////////////////////
//     StaticAction

/// \brief Singleton für Aktion
///
/// Durch diese Klasse wird aus Aktion ein Singleton und die nicht-statischen Methoden der
/// Aktionsklasse stehen auch in statischen Methoden (z.B. Zustandsfunktionen) zur
/// Verfügung.
///
/// Diese Klasse sollte nicht direkt verwendet werden. Diese Klasse kann
/// mit \ref ACTION_CLASS erstellt und die Zustandsfunktionen mit \ref ACTION_STATES
/// definiert werden.
///
/// \tparam A Von Aktion abgeleitete Klasse
///
/// \sa ACTION_CLASS, ACTION_STATES, SIMPLE_ACTION_CLASS
template<typename A>
class StaticAction: public Action {
public:
  /// \brief Aktions-Singleton erstellen
  ///
  /// Wird in \ref ACTION_STATES aufgerufen.
  constexpr
  StaticAction(State start_state, const char* name) :
    Action(start_state, name)
  { }

  /// \brief Referenz zu Instanz von Aktion bekommen
  static _always_inline A& get() {
    return *getPointer();
  }

  /// \brief Zeiger zu Instanz von Aktion bekommen
  static _always_inline A* getPointer() {
    return &instance;
  }

  /// \copydoc Action::isActive
  static _always_inline bool isActive() {
    return instance.Action::isActive();
  }

  /// \copydoc Action::cancel
  static _always_inline bool cancel() {
    return instance.Action::cancel();
  }

  /// \copydoc Action::func
  static _always_inline bool func(EventId id, EventArg data) {
    return instance.Action::func(id, data);
  }

  /// \copydoc Action::start(EventArg)
  static _always_inline void start(EventArg data = 0) {
    instance.Action::start(nullptr, data);
  }

  /// \copydoc Action::canBeAddedToBlacklist
  constexpr _always_inline bool canBeAddedToBlacklist(void) {
    return instance.Action::canBeAddedToBlacklist();
  }

protected:
  /// \copydoc Action::exit
  static _always_inline bool exit(EventId eid) {
    return instance.Action::exit(eid);
  }

  /// \copydoc Action::nextState
  static _always_inline void nextState(State next, EventArg data = 0) {
    instance.Action::nextState(next, data);
  }

  /// \copydoc Action::callParent
  static _always_inline bool callParent(EventId id, EventArg data) {
    return instance.Action::callParent(id, data);
  }

  /// \copydoc Action::checkState
  constexpr
  static _always_inline bool checkState(State current) {
    return instance.Action::checkState(current);
  }

  /// \copydoc Action::setChildAction
  static _always_inline void setChildAction(Action* child, EventArg data = 0) {
    instance.Action::setChildAction(child, data);
  }

  /// \copydoc Action::getChildAction
  static _always_inline Action* getChildAction() {
    return instance.Action::getChildAction();
  }

  /// \copydoc Action::setCanBeAddedToBlacklist
  static _always_inline void setCanBeAddedToBlacklist(bool val) {
    instance.Action::setCanBeAddedToBlacklist(val);
  }

private:
  /// Singleton Instanz von Aktion
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
