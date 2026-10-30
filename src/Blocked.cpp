/**
 * @file Blocked.cpp
 * @brief Implementation of the Blocked concrete state.
 *
 * A Blocked WorkItem is waiting for other WorkItems (its prerequisites) to
 * finish. It cannot be assigned, started or completed until it is released.
 *
 * Transitions:
 * - Available to blocked() = Blocked (item is waiting on prerequisites)
 * - Blocked to blocked() = Available (prerequisites done, item released)
 * - Blocked tocancel() = Rejected (cancelled before work started)
 */

#include "Blocked.h"
#include "Available.h"
#include "Rejected.h"
#include "WorkItem.h"

/**
 * @brief Returns the name of this state.
 * @return The string "Blocked".
 */
std::string Blocked::process() {
    return "Blocked";
}

/**
 * @brief Attempts to start the WorkItem.
 *
 * Not allowed: the WorkItem is waiting for its prerequisites.
 *
 * @param s The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Blocked::start(WorkItem* s) {
    (void)s;
    return false;
}

/**
 * @brief Attempts to assign the WorkItem.
 *
 * Not allowed: the WorkItem is waiting for its prerequisites. It must be
 * released (back to Available) before it can be assigned.
 *
 * @param a The WorkItem (unused).
 * @param p The Participant (unused).
 * @return Always false. The state does not change.
 */
bool Blocked::assign(WorkItem* a, Participant* p) {
    (void)a;
    (void)p;
    return false;
}

/**
 * @brief Cancels the WorkItem while it is still waiting.
 *
 * Moves the WorkItem from Blocked to Rejected. Allowed because work has
 * not started yet.
 *
 * @param c The WorkItem being cancelled.
 * @return true if the WorkItem moved to Rejected, false if c is null.
 */
bool Blocked::cancel(WorkItem* c) {
    if (c == nullptr) {
        return false;
    }
    c->setState(new Rejected());
    return true; // `this` has been deleted, so return immediately
}

/**
 * @brief Attempts to complete the WorkItem.
 *
 * Not allowed: work on a Blocked WorkItem has not started.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Blocked::complete(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Releases the WorkItem once its prerequisites are done.
 *
 * Calling blocked() on a Blocked WorkItem toggles it back: it moves from
 * Blocked to Available, where it can be assigned as normal.
 *
 * @param b The WorkItem being released.
 * @return true if the WorkItem moved to Available, false if b is null.
 */
bool Blocked::blocked(WorkItem* b) {
    if (b == nullptr) {
        return false;
    }
    b->setState(new Available());
    return true; // `this` has been deleted, so return immediately
}
