/**
 * @file Available.cpp
 * @brief Implementation of the Available concrete state.
 *
 * Available is the initial state of every WorkItem. The item exists but
 * nobody has been assigned to it yet.
 */

#include "Available.h"
#include "InProgress.h"
#include "Rejected.h"
#include "WorkItem.h"
#include "Blocked.h"

/**
 * @brief Returns the name of this state.
 * @return The string "Available".
 */
std::string Available::process() {
    return "Available";
}

/**
 * @brief Starts the WorkItem.
 *
 * Moves the WorkItem from Available to InProgress.
 *
 * @param s The WorkItem being started.
 * @return true if the WorkItem moved to InProgress, false if s is null.
 */
bool Available::start(WorkItem* s) {
    if (s == nullptr) {
        return false;
    }
    s->setState(new InProgress());
    return true; // `this` has been deleted, so return immediately
}

/**
 * @brief Attempts to assign the WorkItem.
 *
 * Not allowed: the WorkItem must be started before it can be assigned.
 *
 * @param a The WorkItem (unused).
 * @param p The Participant (unused).
 * @return Always false. The state does not change.
 */
bool Available::assign(WorkItem* a, Participant* p) {
    (void)a;
    (void)p;
    return false;
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
    c->setState(new Rejected());
    return true; //  this  has been deleted, so return immediately
}

/**
 * @brief Blocks the WorkItem until its prerequisites are done.
 *
 * Moves the WorkItem from Available to Blocked. Calling blocked() again
 * on the Blocked WorkItem releases it back to Available.
 *
 * @param b The WorkItem being blocked.
 * @return true if the WorkItem moved to Blocked, false if b is null.
 */
bool Available::blocked(WorkItem* b) {
    if (b == nullptr) {
        return false;
    }
    b->setState(new Blocked());
    return true; // `this` has been deleted, so return immediately
}
