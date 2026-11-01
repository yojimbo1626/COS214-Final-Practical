/**
 * @file Assigned.cpp
 * @brief Implementation of the Assigned concrete state.
 *
 * A Participant has been given the WorkItem, but work has not started.
 */

#include "Assigned.h"
#include "Completed.h"
#include "Rejected.h"
#include "WorkItem.h"
/**
 * @brief Returns the name of this state.
 * @return The string "Assigned" (state).
 */
std::string Assigned::process() {
    return "Assigned";
}

/**
 * @brief Attempts to start the WorkItem.
 *
 * Not allowed: the WorkItem has already been started.
 *
 * @param s The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Assigned::start(WorkItem* s) {
    (void)s;
    return false;
}



/**
 * @brief Attempts to assign the WorkItem again.
 *
 * Not allowed: the WorkItem is already assigned.
 *
 * @param c The WorkItem (unused).
 * @param p The Participant (unused).
 * @return Always false - state doesn't change because its already assigned
 */
bool Assigned::assign(WorkItem* c, Participant* p) {
    (void)c;
    (void)p;
    return false;
}

/**
 * @brief Completes the WorkItem.
 *
 * Moves the WorkItem from Assigned to Completed.
 *
 * @param c The WorkItem being completed.
 * @return true if the WorkItem moved to Completed, false if c is null.
 */
bool Assigned::complete(WorkItem* c) {
    if (c == nullptr) {
        return false;
    }
    c->setState(new Completed());
    return true; // `this` has been deleted, so return immediately
}

/**
 * @brief Cancels the WorkItem before work has started.
 *
 * Moves the WorkItem from Assigned to Rejected.
 *
 * @param c The WorkItem being cancelled.
 * @return true if the WorkItem moved to Rejected, false if c is null.
 */
bool Assigned::cancel(WorkItem* c) {
    if (c == nullptr) {
        return false;
    }
    c->setState(new Rejected());
    return true; // this has been deleted, so return immediately
}

/**
 * @brief Attempts to block the WorkItem.
 *
 * Not allowed: the WorkItem is already assigned. Only an Assigned WorkItem
 * can be blocked.
 *
 * @param b The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Assigned::blocked(WorkItem* b) {
    (void)b;
    return false;
}

/**
 * @brief Creates an independent copy of this state.
 *
 * Used by WorkItem::createSnapshot() (Memento). A snapshot must own its own
 * state object, because the WorkItem deletes its current state on every
 * transition: storing the WorkItem's own pointer would leave the snapshot
 * pointing at deleted memory.
 *
 * @return A new Assigned object. The caller takes ownership of it.
 */
WorkItemState* Assigned::clone() {
    return new Assigned();
}