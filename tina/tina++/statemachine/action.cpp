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

#include "action.h"
#include "../debug.h"
#include "eventqueue.h"

namespace TURAG {

Action::Action(const char* name):
    name_(name), active_(false),
    parent_(nullptr), child_(nullptr)
{ }

void Action::exit(EventId eid)
{
    if (!isActive()) {
        turag_criticalf("Cannot exit inactive action %s.",
                        name_);
		return;
	}

	turag_infof("##### EXIT %s", name_);

    active_ = false;

    // cancel child action
    if (child_) {
		child_->cancel();
		child_ = nullptr;
	}

	if (parent_) {
        // cut connection to parent
        parent_->child_ = nullptr;

		Action* parent = parent_;
		parent_ = nullptr;

        // inform parent
        if (!parent->handleEvent(Action::event_return, eid) ) {
            turag_warningf("Return of action %s was not handled.",
                           name_);
		}
	}
}

void Action::cancel(EventArg arg)
{
	if (!isActive()) {
        turag_criticalf("Cannot cancel inactive action %s.",
                        name_);
	}

	turag_infof("##### CANCEL %s", name_);

    handleEvent(event_cancel, arg);

    // cancel child
    if (child_) {
        child_->cancel(arg);
		child_ = nullptr;
	}
    active_ = false;

	if (parent_) {
        // remove connection between this and parent
        parent_->child_ = nullptr;
		parent_ = nullptr;
	}
}

void Action::kill()
{
    if(!isActive()) {
        turag_criticalf("Cannot kill inactive action %s.", name_);
        return;
    }
    // remove connection to parent
    if (parent_)
		parent_->child_ = nullptr;

    // kill all children
	Action* next = this;
	do {
		Action* current = next;
		next = next->child_;

        current->active_ = false;
        current->child_ = nullptr;
        current->parent_ = nullptr;
	} while (next);
}

void Action::start(Action *parent, EventArg data)
{
	turag_infof("##### START %s", name_);

	if (isActive()) {
        turag_criticalf("Tried to start already active action %s.", name_);
        return;
	}

	if (parent) {
		if (!parent->isActive()) {
            turag_criticalf("Inactive parent action %s cannot accept new child %s.",
							parent->name_, name_);
			return;
		}

		if (parent->getChildAction() != nullptr) {
            turag_criticalf("Parent action %s already has child %s and cannot accept new child %s.",
                            parent->name_, parent->child_->name_, name_);
			return;
		}

		parent_ = parent;
		parent_->child_ = this;
	}

	child_ = nullptr;
    active_ = true;
    onStart(data);
}

bool Action::func(EventId id, EventArg data)
{
    if(!isActive()) {
        turag_criticalf("Inactive action %s cannot process events.",
						name_);
		return false;
	}

	Action* current = getInnermostChild();
	do {
        if(current->handleEvent(id, data) )
			return true;

        current = current->parent_;
    } while(current);

	return false;
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
