/**
 * @file Available.cpp
 * @brief Implementation of the Available concrete state.
 *
 * Available is the initial state of every WorkItem. The item exists but
 * nobody has been assigned to it yet.
 */

#include "Available.h"
#include "Assigned.h"
#include "Rejected.h"
#include "WorkItem.h"

/**
 * @brief Returns the name of this state.
 * @return The string "Available".
 */
std::string Available::process() {
    return "Available";
}

/**
 * @brief Attempts to start the WorkItem.
 *
 * Not allowed: a WorkItem must be assigned before work can start.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Available::start(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Assigns the WorkItem to a Participant.
 *
 * Subscribes the Participant to the WorkItem (so it is notified of every
 * change from now on), then moves the WorkItem from Available to Assigned.
 * Because the Participant is subscribed first, it receives the
 * "now Assigned" update too.
 *
 * @param c The WorkItem being assigned.
 * @param p The Participant receiving the work. Must not be null.
 * @return true if the WorkItem moved to Assigned, false if c or p is null.
 */
bool Available::assign(WorkItem* c, Participant* p) {
    if (c == nullptr || p == nullptr) {
        return false;
    }
    c->addObserver(p);
    changeState(c, new Assigned());
    return true; // `this` has been deleted, so return immediately
}

/**
 * @brief Attempts to complete the WorkItem.
 *
 * Not allowed: work that has not started cannot be completed.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Available::complete(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Cancels the WorkItem before anyone has been assigned.
 *
 * Moves the WorkItem from Available to Rejected.
 *
 * @param c The WorkItem being cancelled.
 * @return true if the WorkItem moved to Rejected, false if c is null.
 */
bool Available::cancel(WorkItem* c) {
    if (c == nullptr) {
        return false;
    }
    changeState(c, new Rejected());
    return true; // `this` has been deleted, so return immediately
}
