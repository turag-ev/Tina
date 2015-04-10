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
/// \code
/// #include <tina/statemachine/state.h>
/// \endcode
///
/// # Aktionen
///
/// Eine Aktion (\ref Action) ist modular aufgebaut. Damit hat eine Aktion einen bestimmten
/// Zweck. In der SystemControl wird zwischen Hauptaktion (MainAction),
/// Spielstrategieaktion (GameAction, mit wechselner Strategie) und den
/// Spielaktionen unterschieden. Aktionen können andere Aktionen aufrufen und
/// damit eine Hierachie bilden.
///
/// Jede Aktion besteht besteht, wie jede Zustandsmaschine, aus mindestens einem Zustand.
/// Jeder Zustand wird durch eine Zustandsfunktion (\ref Action::State) dargestellt.
/// Diese Zustände verarbeiten Ereignisse (\ref Event), verändern den
/// Zustand der Aktion, beenden die Aktion und rufen eine weitere Aktion auf.
///
/// ## Aktion definieren
///
/// Definiert wird eine Aktion über das Makro \ref ACTION_CLASS, welches als
/// Parameter den Namen der Aktionsklasse verlangt.
/// Mit dem Makro \ref ACTION_STATES werden die Zustände und der Konstruktor definiert.
/// Die Klasse kann wie jede andere C++-Klasse verwendet werden.
/// Jede Aktion hat genau eine Instanz (Singleton-Entwurfsmuster) auf die über \ref StaticAction::get und
/// \ref StaticAction::getPointer zugegriffen werden kann - die Instanz wird durch die Klasse
/// \ref StaticAction bereit gestellt:
/// \code
/// // Klasse definieren
/// ACTION_CLASS(ExampleAction) {
///   // Konstruktor plus Zustände definieren
///   ACTION_STATES(ExampleAction, zustand1, zustand2, zustand3)
///
/// public:
///   // öffentliche Variable
///   int var;
///
///   // öffentliche Methode
///   void incVar() { var++; }
/// };
///
/// ExampleAction::get().var = 42;
/// ExampleAction::getPointer()->incVar(); // var = 43
/// \endcode
///
/// ## Zustände definieren
///
/// In einer Aktion ist immer nur ein Zustand aktiv. Startet die Aktion ist der
/// Initialzustand als Zustand gesetzt. Dieser ist der erste Zustand, der nach
/// dem Namen der Aktionsklasse, als Argument zum Makro \ref ACTION_STATES
/// angegeben wird. In dem Beispiel ist es \c zustand1 .
///
/// Die Signatur einer Zustandsfunktion ist folgende:
/// \code
/// bool AKTIONSKLASSE::ZUSTAND (EventId id, EventArg data);
/// \endcode
///
/// Ereignisse werden in der Zustandsfunktion über ihre Ereignis-Id identifiziert,
/// welche über die ersten Parameter übergeben wird.
/// Jedes Ereignis hat eine einzigartige Id, die über die Funktion \ref EventNamespace
/// erzeugt wird. Der zweite Parameter \a data kann abhängig vom Ereignis noch
/// weitere Informationen enthalten. Der Rückgabewert gibt an, ob das Ereignis
/// von der Zustandsfunktion verarbeitet wurde.
///
/// Eine Zustandsfunktion sieht dann folgendermaßen aus:
/// \code
/// bool ExampleAction::zustand1(EventId id, EventArg data) {
///   switch (id) {
///   case event_start:
///     get().var = 0;
///     return true;
///
///   case EventGenerator::event_42:
///     get().var = data;
///     return true;
///   }
///   return false;
/// }
/// \endcode
///
/// Wird ein Zustand gestartet, wird das Ereignis \ref Action::event_start
/// der neuen Zustandsfunktion übergeben.
/// Der Rückgabewert der Funktion wird dabei ignoriert. Dieses Ereignis ist
/// speziell, da es nicht wie andere Ereignisse, wenn es nicht verarbeitet wird,
/// an die übergeordnete Aktion weitergeleitet wird. Gleiches gilt für die anderen
/// Ereignisse von Action: \ref Action::event_cancel, \ref Action::event_return.
///
/// In der Zustandsfunktion kann wieder über `get()` oder `getPointer()` auf
/// die Instanz der Aktion zugegriffen. Aber es kann auch auf die nicht-statischen
/// Methoden von \ref Action zugegriffen werden - Interna: wieder hilft \ref StaticAction,
/// dort sind die statischen Methoden definiert. Beispiel folgt sofort.
///
/// Der Zustand in der Aktion kann über die Funktion \ref Action::nextState
/// (bzw. \ref StaticAction::nextState) verändert werden. Dabei wird der neue
/// Zustand über die entsprechende Zustandsfunktion angegeben. Ein zusätzlicher
/// Datenparameter kann genutzt werden, um den nächsten Zustand Information
/// zu übergeben.
///
/// Eine Aktion kann über die Funktion \ref Action::exit (bzw. \ref StaticAction::exit)
/// beendet werden. In einem Parameter kann angegeben, ob die Aktion
/// erfolgreich ausgeführt wurde (\ref Action::event_success) oder nicht (\ref Action::event_failure).
///
/// Im Folgenden wird ein Beispiel aus der SystemControl benutzt, wobei
/// der Roboter zum Spielfeldmittelpunkt (0/1000) fahren soll.
/// \code
/// bool ExampleAction::zustand1(EventId id, EventArg data) {
///   switch (id) {
///   // Zustand startet
///   case event_start:
///      // Fahrbefehl: Fahre zu Spielfeldmittelpunkt (0/1000)
/// 	drive.driveTo(
///       Pose(0*Units::mm, 1000*Units::mm, Drive::ANGLE_DONT_CARE),
///       SPEED_INDEX_MEDIUM);
/// 	return true;
///
///   // Roboter ist angekommen
///   case Drive::event_arrived:
/// 	// zu nächsten Zustand gehen:
/// 	nextState(zustand2);
/// 	return true;
///
///   // Roboter hat Collision
///   case Drive::event_collision:
/// 	// Aktion beenden
/// 	exit(event_failure);
/// 	return true;
///   }
///   return false;
/// }
/// \endcode
///
/// Wird die Aktion von außerhalb der Aktion beendet (\ref Action::cancel),
/// dann wird der Zustand mit dem Ereignis \ref Action::event_cancel aufgerufen.
/// Damit können abschließende Aktionen durchgeführt werden. Der Rückgabewert
/// der Zustandsfunktion, wird dabei ignoriert:
/// \code
/// bool ExampleAction::zustand1(EventId id, EventArg data) {
///   switch (id) {
///
///   // ...
///
///   // Aktion wurde abgebrochen
///   case event_cancel:
///     drive.stop();
///     return true;
///   }
///   return false;
/// }
/// \endcode
/// In der SystemControl wird \ref Action::event_cancel meistens nicht
/// verarbeitet, da nachdem eine Aktion abgebrochen wird, der Routinecheck
/// des Troubleshooters alles selbst beendet.
///
/// ## Unteraktionen (Kindaktionen)
///
/// Ein Zustand kann eine weitere Aktion ausführen. Die Kindaktion
/// wird über die Funktion \ref Action::setChildAction hinzugefügt.
/// Mit dem Ereignis \ref Action::event_return wird die Aktion benachrichtigt,
/// wenn die Unteraktion sich beendet hat.
/// Im Parameter \a data steht der Rückgabewert der Aktion
/// (z.B. Action::event_success, Action::event_failure).
/// Der Rückgabewert beim Action::event_return-Event wird ignoriert.
/// \code
/// bool ExampleAction::zustand2(EventId id, EventArg data) {
///    switch (id) {
///    case event_start:
/// 	 // Aktion ExampleAction2 starten:
/// 	 setChildAction(ExampleAction2::getPointer());
/// 	 return true;
///
///   case event_return:
/// 	// Aktion ist fertig -> Rückgabewert auswerten
/// 	if (data == event_success) {
/// 	  // Aktion hat geklappt
/// 	  nextState(zustand3);
///       return true;
///     } else {
/// 	  // Aktion fehlgeschlagen
/// 	  exit(event_failure);
/// 	  return true;
/// 	}
///   }
///   return false;
/// }
/// \endcode
///
/// Ereignisse werden immer erstmal an die untersten Kindaktion ausgeführt.
/// Wenn die Unteraktion das Ereignis nicht verarbeitet (Zustandsfunktion gibt \c false zurück),
/// wird das Ereignis in der nächst höheren Aktion ausgeführt.
/// So müssen nicht alle Ereignisse in einem Zustand verarbeitet werden,
/// wenn die übergeordnete Aktion das Ereignisse verarbeitet.
///
/// In unserem Beispiel würde z.B. ExampleAction2 `Drive::event_collision` nicht
/// verarbeiten müssen, wenn in der übergeordneten Aktion verarbeitet wird:
/// \code
/// bool ExampleAction::zustand2(EventId id, EventArg data) {
///    switch (id) {
///    case event_start:
/// 	 // Aktion ExampleAction2 starten:
/// 	 setChildAction(ExampleAction2::getPointer());
/// 	 return true;
///
///   case event_return:
/// 	// ...
///
///   case Drive::event_collision:
/// 	// Aktion und damit Unteraktion beenden
/// 	exit(event_failure);
/// 	return true;
///   }
///   return false;
/// }
/// \endcode
///
/// Wird Action::nextState, Action::exit oder Action::cancel aufgerufen, wird
/// automatisch die zugehörige Kindaktion abgebrochen.
///
/// \todo kill() erklären
///
/// ## Klassenhierachie in der SystemControl
///
/// So gibt es eine Hauptaktion, die der Ereignisschlange übergeben wird. Diese Hauptaktion steht in der Hierarchie ganz oben.
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
/// ## Richtlinien/Verwendung
///
/// execute-Funktionen
///
/// Beispiele,Beispiele, Beispiele
///
/// minimal Beispiel
///
///
/// @{
// \msc
// Sender,Receiver;
// Sender->Receiver [label="Command()", URL="\ref Receiver::Command()"];
// Sender<-Receiver [label="Ack()", URL="\ref Ack()", ID="1"];
// \endmsc


////////////////////////////////////////////////////////////////////////////////
//     Action State Model

/// \brief Aktionszustandsmaschine
/// \copydetails Actions
class Action {
public:
	/// \brief Typ von Zustand(-sfunktion)
	/// \param id ID von Ereignis, was verarbeitet werden soll.
	/// \param data zusätzliche Information zu Ereignis
	/// \retval true Wenn Ereignis von dieser Funktion verarbeitet wurde.
	/// \retval false Wenn Ereignis nicht verarbeitet wurde.
	typedef bool (*State)(EventId id, EventArg data);

  /// \brief Ereignis-IDs von Ereignissen von Aktion.
  ///
  /// Diese speziellen Ereignisse bieten die Möglichkeiten auf Ereignisse
  /// innerhalb der Aktion und dessen Zustände zu reagieren.
  enum {
    /// \brief Aktion hat neuen Zustand
    ///
    /// Der zustätzliche Datenparameter hat den Wert, des bei \ref start oder
    /// \ref setChildAction übergebenen zweiten Paramters.
	///
	/// Der Rückgabewert der Zustandsfunktion wird ignoriert.
	/// \sa start
    event_start = EventNamespace('A', 'c', 't'),

    /// \brief Aktion wird von außerhalb abgebrochen.
    ///
    /// Dieses Ereigniss tritt auf, wenn Aktion über die Funktion \ref cancel
	/// abgebrochen wird. Das gibt die Möglichkeit abschließende Arbeiten
	/// durchzuführen.
    ///
	/// Der Rückgabewert der Zustandsfunktion wird ignoriert.
	/// \sa cancel
    event_cancel,

    /// \brief Kindaktion hat sich beendet.
    ///
    /// Dieses Ereigniss tritt auf, wenn eine durch die Funktion \ref setChildAction
    /// hinzugefügte Kindaktion durch die Funktion \ref exit beendet wurde.
	/// Dieses Ereignis tritt nicht auf, wenn diese Aktion durch \ref cancel
	/// beendet wird.
    ///
	/// Das zweite Argument, welches der Zustandsfunktion bei diesem Event übergeben wird, ist
	/// der Parameter, der der \ref exit Funktion übergeben wird, z.B. \ref event_success
    /// oder \ref event_failure.
	///
	/// Ist der Rückgabewert der Zustandsfunktion \c false, wird eine Warnung
	/// ausgegeben.
 /// \sa exit
    event_return = EventNamespace('A', 'c', 't') + 100,
  };

  /// \brief spezielle Rückgabewerte von Aktionen.
  ///
  /// Diese Rückgabewerte können beim Aufruf der \ref exit Funktion verwendet werden.
  /// \sa exit
  enum {
    /// \brief Rückgabewert, wenn Aktion erfolgreich ausgeführt wurde.
	/// \sa exit
    event_success,

    /// \brief Rückgabewert, wenn Aktion nicht erfolgreich ausgeführt wurde.
	/// \sa exit
	event_failure
  };

  /// \brief Aktion erstellen.
  ///
  /// \warning
  /// \ref ACTION_CLASS oder \ref SIMPLE_ACTION_CLASS sollten für das Erstellen
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
    can_be_added_to_blacklist_(true)
  { }

  /// \brief Hauptaktion starten
  ///
  /// Die Hauptaktion wird mit dieser Funktion gestartet. Eine Unteraktion
  /// kann mit \ref setChildAction hinzugefügt werden.
  ///
  /// Beim Starten der Aktion wird der Startzustand als aktueller Zustand
  /// gesetzt und dieser direkt mit dem \ref event_start Ereigniss aufgerufen.
  /// Der zugehörige Datenparameter entspricht \a data.
  ///
  /// \pre Aktion ist nicht aktiv: `isActive() == false`
  /// \param data zusätzliche beliebige Informationen, die bei \ref event_start
  ///             als zweiter Parameter beim Starten des ersten Zustands übergeben wird.
  /// \sa setChildAction
  void start(EventArg data = 0) {
    start(nullptr, data);
  }

  /// \brief Ereignis von Aktion und Unteraktione verarbeiten lassen.
  ///
  /// Wird in der Regel nur benötigt, um Ereignisse der Ereignisschlange (\ref TURAG::EventQueue)
  /// der Hauptaktion zuzuführen.
  ///
  /// Das Ereignis wird zuerst an das am weitesten verschachelte Kindaktion
  /// übergeben. Gibt die aktuelle Zustandsfunktion dieser Aktion \c true
  /// zurück gilt des Ereignis als behandelt und diese Funktion gibt auch den
  /// Wert \c true zurück. Gibt die aktuelle Zustandsfunktion \c false zurück,
  /// wird das Ereignis an die nächsthöhere Aktion weitergegeben und wieder
  /// geprüft, ob das Ereignis behandelt wurde. Wurde das Ereignis auch auf höhere
  /// Ebene nicht behandelt, gibt diese Funktion \c false zurück.
  ///
  /// \pre Aktion ist aktiv: `isActive() == true`
  /// \param id Ereignis-ID von Ereignis.
  /// \param data Datenparameter zu Ereignis.
  /// \return Es wird \c true zurückgegeben, falls das Ereignis verarbeitet wurde, sonst \a false.
  /// \sa getState
  bool func(EventId id, EventArg data);

  /// \brief Aktion ist aktiv.
  /// \sa getState, checkState
  _always_inline
  bool isActive() const {
    return currentstate_ != nullptr;
  }

  /// \brief aktuellen Zustand von Aktion zurückgeben
  /// \return aktueller Zustand von Aktion oder \c nullptr, wenn Aktion nicht
  ///         aktiv (\ref isActive liefert \c false).
  /// \sa checkState, isActive
  _always_inline
  State getState() const {
    return currentstate_;
  }  

  /// \brief Bricht Verarbeitung von Aktion und allen Unteraktionen ab.
  ///
  /// Hat diese Aktion Unteraktionen, so werden erst alle Unteraktionen
  /// abgebrochen, bevor diese Aktion abgebrochen wird.
  /// Wenn die Aktion abgebrochen wird, wird das Ereignis \ref event_cancel
  /// direkt an den aktuellen Zustand der betreffenden Aktion weitergegeben.
  /// Damit wird der Aktion die Möglichkeit gegeben abschließende Arbeiten
  /// zu tätigen.
  ///
  /// Im Gegensatz zu \ref exit, wird nicht das Ereignis \ref event_return
  /// an die nächst höhere Aktion übergeben.
  ///
  /// \pre Aktion ist aktiv: `isActive() == true`
  /// \post Aktion ist nicht mehr aktiv: `isActive() == false`
  /// \post Aktion hat keine Kindaktionen: `getChildAction() == nullptr`
  /// \post Aktion hat keine übergeordnete Aktion: `getParentAction() == nullptr`
  ///
  /// \sa exit, kill
  void cancel();

  /// \brief Würgt Verarbeitung von Aktion ab.
  ///
  /// Hat diese Aktion Unteraktionen, so werden diese auch abgewürgt.
  ///
  /// Im Gegensatz zu \ref exit, wird nicht das Ereignis \ref event_return
  /// an die nächst höhere Aktion übergeben und im Gegensatz zu \ref cancel, wird
  /// kein Ereignis \ref event_cancel in der Aktion ausgeführt.
  ///
  /// \warning Durch das harte Beenden kann die Aktion evtl. Hintergundaktionen
  ///          nicht ordnungsgemäß beenden.
  ///
  /// \pre Aktion ist aktiv: `isActive() == true`
  /// \post Aktion ist nicht mehr aktiv: `isActive() == false`
  /// \post Aktion hat keine Kindaktionen: `getChildAction() == nullptr`
  /// \post Aktion hat keine übergeordnete Aktion: `getParentAction() == nullptr`
  ///
  /// \sa cancel, exit
  void kill();

  /// \brief Unteraktion hinzufügen.
  ///
  /// Jede Aktion kann bis zu eine Unteraktion aufweisen.
  /// Durch diese Funktion wird der Aktion eine Unteraktion hinzugefügt, wenn
  /// noch keine vorhanden ist. Die Unteraktion wird durch diese Funktion
  /// gestartet, siehe \ref start. Dadurch wird der aktuelle Zustand der
  /// neuen Kindaktion mit dem Ereignis \ref event_start und \a data als Datenparameter
  /// aufgerufen.
  ///
  /// \pre Aktion ist aktiv: `isActive() == true`
  /// \pre Aktion hat keine Unteraktion: `getChildAction() == nullptr`
  /// \pre Parameter \a child ist nicht \c nullptr: `child != nullptr`
  ///
  /// \param child Unteraktion, die hinzugefügt und gestartet werden soll.
  /// \param data Parameter mit extra Daten, der mit dem \ref event_start Ereignis,
  ///              dem Startzustand als Datenparameter übergeben wird.
  ///
  /// \sa start(EventArg), getChildAction() const
  ///
  void setChildAction(Action* child, EventArg data = 0) {
	  child->start(this, data);
  }

  /// \brief Gibt eine evtl. vorhandene Unteraktion zurück.
  ///
  /// \return vorhandene Unteraktion oder \c nullptr, wenn keine vorhanden.
  ///
  /// \sa setChildAction, hasChildAction, getParentAction
  _always_inline
  const Action* getChildAction() const {
    return child_;
  }

  /// \brief Gibt zurück, ob eine Unteraktion vorhandenen ist.
  /// \sa setChildAction, getParentAction
  _always_inline
  bool hasChildAction() const {
	return child_ != nullptr;
  }

  /// \brief Gibt eine evtl. übergeordnete Unteraktion zurück.
  ///
  /// \return übergeordnete Unteraktion oder \c nullptr, wenn keine vorhanden.
  ///
  /// \sa callParentAction, setChildAction
  _always_inline
  const Action* getParentAction() const {
	return parent_;
  }

  /// \brief Name der Aktion zurückgeben
  ///
  /// Der Name ist für Debuggingzwecke gedacht. Es ist möglich, dass zwei
  /// Aktionen über die gleichen Namen verfügen.
  ///
  /// \return Zeichenkette mit Name der Aktion
  /// \sa ACTION_CLASS, SIMPLE_ACTION_CLASS
  const char* getName() const {
    return name_;
  }

  /// \brief Aktion kann zur Blacklist hinzugefügt werden
  ///
  /// Sagt aus, ob es erlaubt ist die Aktion in die Blacklist des SystemControl
  /// Troubleshooter's hinzu zu fügen. Dies ist nützlich für Aktionen mit
  /// dynamischen Einstiegspunkten.
  /// \sa setCanBeAddedToBlacklist
  constexpr _always_inline
  bool canBeAddedToBlacklist() const {
    return can_be_added_to_blacklist_;
  }

  /// \brief Prüft, ob ein Zustand ausgeführt wird
  ///
  /// \return Es wird \c true zurückgegeben, falls \a current der aktuell
  /// von dieser Aktion ausgeführte Zustand ist.
  /// \sa getState
  _always_inline
  bool checkState(State current) const {
	return currentstate_ == current;
  }

protected:

  /// \brief Aktion starten
  ///
  /// Beim Starten der Aktion wird der Startzustand als aktueller Zustand
  /// gesetzt und dieser direkt mit dem \ref event_start Ereigniss aufgerufen.
  /// Der zugehörige Datenparameter entspricht \a data.
  ///
  /// \pre Aktion ist nicht aktiv: `isActive() == false`
  /// \pre Elternaktion ist aktiv und hat keine Kindaktion:
  ///      `parent == nullptr || (parent->isActive() && parent->getChildAction() == nullptr)`
  /// \param parent Elternaktion oder \c nullptr, wenn diese Aktion die Hauptaktion ist.
  /// \param data zusätzliche beliebige Informationen, die bei \ref event_start
  ///             als zweiter Parameter beim Starten des ersten Zustands übergeben werden.
  /// \sa setChildAction, start(EventArg)
  void start(Action* parent, EventArg data = 0);

  /// \brief Übergibt übergeordneter Aktion Ereignis
  ///
  /// \pre Aktion ist aktiv: `isActive() == true`
  /// \pre Aktion hat übergeordnete Aktion: `getParentAction() != nullptr`
  ///
  /// \param id Ereignis-ID zu Ereignis
  /// \param data Datenparamter zu Ereignis
  /// \return Ereignis wurde von übergeordneter Aktion verarbeitet oder \a false,
  ///         falls keine übergeordnete Aktion vorhanden ist.
  /// \sa getParentAction, setChildAction
  _always_inline
  bool callParentAction(EventId id, EventArg data) const {
    return (parent_) ? (parent_->func(id , data)) : (false);
  }

  /// \brief Setzt Zustand von Aktion.
  ///
  /// Diese Funktion setzt einen neuen Zustand für diese Aktion, der ab jetzt
  /// alle Ereignisse verarbeitet. Der neue Zustand wird mit dem
  /// Ereignis \ref event_start und dem Parameter \a data aufgerufen.
  ///
  /// Der neue Zustand \a next sollte eine statische Methode dieser Aktion sein.
  ///
  /// Evtl. vorhandene Unteraktionen werden vor dem Setzen des neuen Zustands abgebrochen.
  ///
  /// \pre Aktion ist aktiv: `isActive() == true`
  ///
  /// \param next zu setzten der Zustand dieser Aktion
  /// \param data optionaler Datenparameter, der mit \ref event_start Ereignis
  ///             bei neuem Zustand übergeben wird.
  void nextState(State next, EventArg data = 0);

  /// \brief Beendet Aktion.
  ///
  /// Durch diese Funktion wird die Aktion beendet und evtl. vorhandene Unteraktionen
  /// abgebrochen.
  ///
  /// Wenn eine übergeordnete Aktion vorhanden ist, wird der aktuelle Zustand
  /// diese Aktion mit dem Ereignis \ref event_return und dem Datenparameter
  /// \a eid aufgerufen. Als Datenparameter wird \ref event_success vorgeschlagen,
  /// wenn die Aktion erfolgreich ausgeführt wurde, und \ref event_failure, falls
  /// dies nicht das Fall war.
  ///
  /// \pre Aktion ist aktiv: `isActive() == true`
  /// \post Aktion ist nicht mehr aktiv: `isActive() == false`
  /// \post Aktion hat keine Kindaktionen: `getChildAction() == nullptr`
  /// \post Aktion hat keine übergeordnete Aktion: `getParentAction() == nullptr`
  ///
  /// \param eid Rückgabewert dieser Aktion, z.B. \ref event_success oder \ref event_failure.
  /// \sa cancel
  void exit(EventId eid);

  /// \copydoc getChildAction() const
  _always_inline
  Action* getChildAction() {
    return child_;
  }

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

  /// Aktion kann auf Blacklist gesetzt werden
  bool can_be_added_to_blacklist_;

  /// Das am weitesten untergeordnete Kindaktion zurückgeben
  Action* getInnermostChild();
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
/// \tparam A zuerstellende Aktionsklasse
///
/// \sa Action, ACTION_CLASS, ACTION_STATES, SIMPLE_ACTION_CLASS
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
  static _always_inline void cancel() {
	instance.Action::cancel();
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

  /// \copydoc Action::hasChildAction
  static _always_inline bool hasChildAction() {
	return instance.Action::hasChildAction();
  }

  /// \copydoc Action::getParentAction
  static _always_inline const Action* getParentAction() {
	return instance.Action::getParentAction();
  }

protected:
  /// \copydoc Action::exit
  static _always_inline void exit(EventId eid) {
	instance.Action::exit(eid);
  }

  /// \copydoc Action::nextState
  static _always_inline void nextState(State next, EventArg data = 0) {
    instance.Action::nextState(next, data);
  }

  /// \copydoc Action::callParentAction
  static _always_inline bool callParentAction(EventId id, EventArg data) {
	return instance.Action::callParentAction(id, data);
  }

  /// \copydoc Action::checkState
  static _always_inline bool checkState(State current) {
    return instance.Action::checkState(current);
  }

  /// \copydoc Action::getState
  static _always_inline State getState() {
	return instance.Action::getState();
  }

  /// \copydoc Action::getChildAction
  static _always_inline Action* getChildAction() {
	return instance.Action::getChildAction();
  }

  /// \copydoc Action::setChildAction
  static _always_inline void setChildAction(Action* child, EventArg data = 0) {
	instance.Action::setChildAction(child, data);
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
///
/// Über dieses Makro wird eine Klasse definiert, die von der Klasse
/// \ref StaticAction erbt und ist equivalent zu `class name : public StaticAction<name>`.
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
