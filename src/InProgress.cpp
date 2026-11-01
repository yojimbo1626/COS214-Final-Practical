/**
 * @file InProgress.cpp
 * @brief Implementation of the InProgress concrete state.
 *
 * Work on the WorkItem has started. The only way forward is to complete it.
 * Cancelling is no longer allowed once work has started.
 */

#include "InProgress.h"
#include "Assigned.h"
#include "Rejected.h"
#include "WorkItem.h"

/**
 * @brief Returns the name of this state.
 * @return The string "InProgress".
 */
std::string InProgress::process() {
    return "InProgress";
}

/**
 * @brief Attempts to start the WorkItem again.
 *
 * Not allowed: work has already started.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool InProgress::start(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Assigns the WorkItem to a Participant.
 *
 * Subscribes the Participant as an observer, then moves the WorkItem from
 * InProgress to Assigned.
 *
 * @param a The WorkItem being assigned.
 * @param p The Participant receiving the work. Must not be null.
 * @return true if the WorkItem moved to Assigned, false if a or p is null.
 */
bool InProgress::assign(WorkItem* a, Participant* p) {
    if (a == nullptr || p == nullptr) {
        return false;
    }
    a->addObserver(p);
    a->setState(new Assigned());
    return true; // `this` has been deleted, so return immediately
}

/**
 * @brief Attempts to complete the WorkItem.
 *
 * Not allowed: the WorkItem must be assigned before it can be completed.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool InProgress::complete(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Cancels the WorkItem.
 *
 * Moves the WorkItem from InProgress to Rejected.
 *
 * @param c The WorkItem being cancelled.
 * @return true if the WorkItem moved to Rejected, false if c is null.
 */
bool InProgress::cancel(WorkItem* c) {
    if (c == nullptr) {
        return false;
    }
    c->setState(new Rejected());
    return true; // `this` has been deleted, so return immediately
}

/**
 * @brief Attempts to block the WorkItem.
 *
 * Not allowed: work has already started.
 *
 * @param b The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool InProgress::blocked(WorkItem* b) {
    (void)b;
    return false;
}
