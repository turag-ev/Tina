/**
 * \file state.cpp
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief a finit state machine
 *
 */

#define TURAG_DEBUG_LOG_SOURCE "A"
#include "../debug.h"

#include "state.h"
#include "../debug.h"
#include "eventqueue.h"

namespace TURAG {

void Action::exit(EventId eid)
{
	if (!isActive()) {
		turag_criticalf("Inaktive Aktion %s kann nicht beendet werden.", name_);
		return;
	}

	turag_infof("##### EXIT %s", name_);

	// Aktion inaktiv setzen
	currentstate_ = nullptr;

	// Kindaktion abbrechen, wenn vorhanden
	if (child_) {
		child_->cancel();
		child_ = nullptr;
	}

	if (parent_) {
		// Verbindung von Elternelement entfernen
		parent_->child_ = nullptr;

		// Verbindung zu Elternelement entfernen
		Action* parent = parent_;
		parent_ = nullptr;

		// Elternaktion über Beenden dieser Aktion informieren
		bool handled = parent->currentstate_(Action::event_return, eid);
		if (!handled) {
			turag_warningf("Beenden von Aktion %s wurde nicht behandelt.",
						   name_);
		}
	}
}

void Action::cancel()
{
	if (!isActive()) {
		turag_criticalf("Inaktive Aktion %s kann nicht abgebrochen werden.",
						name_);
	}

	turag_infof("##### CANCEL %s", name_);

	// Aktion inaktiv setzen
	State last_state = currentstate_;
	currentstate_ = nullptr;

	// Kindaktion abbrechen, wenn vorhanden
	if (child_) {
		child_->cancel();
		child_ = nullptr;
	}

	// Aktion benachrichtigen
	last_state(event_cancel, 0);

	if (parent_) {
		// Verbindung zu und von Elternelement entfernen
		parent_->child_ = nullptr;
		parent_ = nullptr;
	}
}

void Action::kill()
{
	if (!isActive()) return;

	// Verbindung von übergeordneter Aktion lösen
	if (parent_)
		parent_->child_ = nullptr;

	Action* next = this;
	do {
		Action* current = next;
		next = next->child_;

		current->child_ = nullptr;
		current->currentstate_ = nullptr;
		current->parent_ = nullptr;
	} while (next);
}

void Action::nextState(State next, EventArg data)
{
	if (!isActive()) {
		turag_criticalf("Zustand der inaktiven Aktion %s kann nicht verändert werden.",
						name_);
	}

	// Kindaktion abbrechen, wenn vorhanden
	if (child_) {
		child_->cancel();
		child_ = nullptr;
	}

	// nächsten Zustand einnehmen
	currentstate_ = next;

	// Zustand starten
	currentstate_(Action::event_start, data);
}

void Action::start(Action *parent, EventArg data)
{
	turag_infof("##### START %s", name_);

	if (isActive()) {
		cancel();
		turag_criticalf("Aktive Aktion %s wird gestartet.", name_);
		//return;
	}

	if (parent) {
		if (!parent->isActive()) {
			turag_criticalf("Inaktiver Aktion %s kann keine Kindaktion %s hinzugefügt werden.",
							parent->name_, name_);
			return;
		}

		if (parent->getChildAction() != nullptr) {
			turag_criticalf("Aktion %s kann keine weitere Kindaktion %s hinzugefügt werden.",
							parent->name_, name_);
			return;
		}

		// übergeordnete Aktion setzen
		parent_ = parent;
		parent_->child_ = this;
	}

	// Ausgangszustand setzen
	child_ = nullptr;

	// Startzustand setzen
	currentstate_ = startstate_;

	// Zustand starten
	currentstate_(Action::event_start, data);
}

bool Action::func(EventId id, EventArg data)
{
	if (!isActive()) {
		turag_criticalf("Inaktive Aktion %s kann keine Ereignisse verarbeiten.",
						name_);
		return false;
	}

#if 0
	// TODO: Implementation verbessern
	Action* current = getInnermostChild();
	do {
		bool handled = current->currentstate_(id, data);
		if (handled) {
			return true;
		}

		if (current->currentstate_ == nullptr) {
			// FIXME: Aktion hat sich beendet, Event wird nicht weitergereicht
			return true;
		}

		current = current->parent_;
	} while (current);

	return false;
#else
	// Ereignis an untergeordnete Aktion weiterreichen
	if (child_) {
		State state_by_event = currentstate_;
		bool handled = child_->func(id, data); // TODO: Rekursion auflösen
		// Wenn nicht behandelt, dann in dieser Aktion von aktuellen
		// Zustand behandeln.
		// Wenn sich Zustand dieser Aktion verändert hat, zählt
		// dies auch als behandelt.
		if (!handled && state_by_event == currentstate_) {
			return currentstate_(id, data);
		}
		return true;
	}
	// Ereignis in aktuellen Zustand verarbeiten
	return currentstate_(id, data);
#endif
}

Action* Action::getInnermostChild()
{
	Action* next = child_;
	Action* current = this;

	while (next) {
		current = next;
		next = current->getChildAction();
	};

	return current;
}

} // namespace TURAG
