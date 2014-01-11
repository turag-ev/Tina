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

// Mögliche Zustände für Aktionen:
//                     | currentstate_ | parent_  | child_   | about_to_close_
// -----------------------------------------------------------------------------
// aktiv               | !nullptr      | undef.   | undef.   | false
// wartet auf Beenden  | !nullptr      | undef.   | !nullptr | true
// wird beendet        | !nullptr      | undef.   | nullptr  | true

bool Action::exit(EventId eid) {
  if (isActive() && !about_to_close_) {
    infof("#####-##### EXIT %s #####-#####", name_);

    currentstate_ = nullptr;
    if (child_) {
      bool nondelayed_close = child_->cancel();
      if (!nondelayed_close) {
        // Aktion befindet sich in semi-aktiven Zustand
        // Aktion wird beendet, wenn sich Kindaktion beendet
        about_to_close_ = true;
        return false;

      } else {
        child_ = nullptr;
      }
    }
    // alle Kinder beendet

    if (parent_) {
      parent_->child_ = nullptr;
      if (parent_->about_to_close_) {
        parent_->cancel();
      } else {
        bool handled = parent_->func(Action::event_return, eid);
        if (!handled) {
          criticalf("Exit of Action %s is not handled", name_);
        }
      }
    }
    return true;

  } else {
    if (child_) {
      // Aktion beendet, aber eines der Kinderaktionen noch nicht
      criticalf("Tried to exit an nonactive action %s (with unclosed action %s)", name_, child_->name_);
      return false;
    } else {
      criticalf("Tried to exit an nonactive action %s", name_);
      return true;
    }
  }
}

bool Action::cancel() {
  if (isActive() && !about_to_close_) {
    infof("#####-##### CANCEL %s #####-#####", name_);
    if (child_) {
      bool nondelayed_close = child_->cancel();
      if (nondelayed_close) {
        child_ = nullptr;
      } else {
        // Aktion befindet sich in semi-aktiven Zustand
        // Aktion wird beendet, wenn sich Kindaktion beendet
        currentstate_ = nullptr;
        about_to_close_ = true;
        return false;
      }
    }
    // ab jetzt alle Kinderaktionen beendet

    bool delayed_close = currentstate_(Action::event_cancel, 0);
    if (!delayed_close) {
      // Aktion wird sofort geschlossen
      currentstate_ = nullptr;
      if (parent_) {
        if (parent_->about_to_close_) {
          parent_->cancel();
        } else {
          parent_->child_ = nullptr;
          parent_ = nullptr;
        }
      }
    } else {
      about_to_close_ = true;
    }
    return !delayed_close;

  } else {
    if (!child_) {
      criticalf("Tried to cancel an nonactive action %s", name_);
      return true;
    } else {
      // Wenn man auf das Beenden einer Kindaktion wartet
      return false;
    }
  }
}

void Action::nextState(State next, EventArg data) {
  if (isActive() && !about_to_close_) {
    if (child_) {
      if (!child_->cancel()) {
        errorf("called nextState in action %s with not trivial cancelable child action %s", name_, child_->name_);
        // TODO: Zustand zulassen

        // Sollte nicht passieren: siehe Hinweis
        child_->parent_ = nullptr;
        child_ = nullptr;
      }
    }

    currentstate_ = next;
    //printf("Action::nextState -> currentstate_ = %p\n", currentstate_);
    currentstate_(Action::event_start, data);

  } else {
    criticalf("Tried to change state of nonactive action %s", name_);
  }
}

void Action::start(Action *parent, EventArg data) {
  if (!isActive() && !about_to_close_) {
    infof("#####-##### START %s #####-#####", name_);
    parent_ = parent;
    child_ = nullptr;
    currentstate_ = startstate_;
    //printf("Action::start -> currentstate_ = %p\n", currentstate_);
    currentstate_(Action::event_start, data);

  } else {
    criticalf("Tried to start an active action %s", name_);
  }
}

_hot
bool Action::func(EventId id, EventArg data) {
  if (isActive()) {
    if (child_) {
      State state_by_event = currentstate_;
      bool handled = child_->func(id, data);
      if (!handled && state_by_event == currentstate_ && !about_to_close_) {
        return currentstate_(id, data);
      } else {
        return true;
      }

    } else {
      return currentstate_(id, data);
    }

  } else {
    criticalf("Tried to call an nonactive action %s", name_);
    return false;
  }
}

void Action::setChildAction(Action* child, EventArg data) {
  if (isActive() && !about_to_close_) {
    if (child_ == nullptr) {
      child_ = child;
      child->start(this, data);

    } else {
      criticalf("Tried to set child of action %s, that has already a active child action: %s.", name_, child_->name_);
    }
  } else {
    criticalf("Tried to set child of non-active action %s to %s", name_, child->name_);
  }
}

void Action::killChildActions() {
  if (isActive()) {
    Action* next = child_;

    while (next) {
      Action* current = next;
      next = next->child_;

      current->child_ = nullptr;
      current->currentstate_ = nullptr;
      current->parent_ = nullptr;
      current->about_to_close_ = false;
    }
    child_ = nullptr;
  }
}

} // namespace TURAG
