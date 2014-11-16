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

void Action::exit(EventId eid) {
	if (isActive()) {
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

			// Elternaktion über Beenden dieser Aktion informieren
			bool handled = parent_->func(Action::event_return, eid);
			if (!handled) {
				turag_warningf("Beenden von Aktion %s wurde nicht behandelt.",
							   name_);
			}

			// Verbindung zu Elternelement entfernen
			parent_ = nullptr;
		}

	} else {
		turag_criticalf("Inaktive Aktion %s kann nicht beendet werden.", name_);
	}
}

void Action::cancel() {
	if (isActive()) {
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

	} else {
		turag_criticalf("Inaktive Aktion %s kann nicht abgebrochen werden.",
						name_);
	}
}

void Action::nextState(State next, EventArg data) {
	if (isActive()) {
		// Kindaktion abbrechen, wenn vorhanden
		if (child_) {
			child_->cancel();
			child_ = nullptr;
		}

		// nächsten Zustand einnehmen
		currentstate_ = next;

		// Zustand starten
		currentstate_(Action::event_start, data);

	} else {
		turag_criticalf("Zustand der inaktiven Aktion %s kann nicht verändert werden.",
						name_);
	}
}

void Action::start(Action *parent, EventArg data) {
	if (!isActive()) {
		turag_infof("##### START %s", name_);

		// übergeordnete Aktion setzen
		parent_ = parent;

		// Ausgangszustand setzen
		child_ = nullptr;

		// Startzustand setzen
		currentstate_ = startstate_;

		// Zustand starten
		currentstate_(Action::event_start, data);

	} else {
		turag_criticalf("Aktive Aktion %s kann nicht gestartet werden.", name_);
	}
}

_hot
bool Action::func(EventId id, EventArg data) {
	if (isActive()) {

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

	} else {
		turag_criticalf("Inaktive Aktion %s kann keine Ereignisse verarbeiten.",
						name_);
		return false;
	}
}

void Action::setChild(Action* child, EventArg data) {
	if (isActive()) {
		if (child_ == nullptr) {
			// untergeordnete Aktion hinzufügen
			child_ = child;

			// untergeordnete Aktion starten
			child->start(this, data);

		} else {
			turag_criticalf("Action %s kann keine weitere Kindaktion %s hinzugefügt werden.",
							name_, child_->name_);
		}
	} else {
		turag_criticalf("Inaktiver Action %s kann keine Kindaktion %s hinzugefügt werden.",
						name_, child->name_);
	}
}

} // namespace TURAG
