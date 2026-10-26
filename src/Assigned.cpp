/**
 * @file Assigned.cpp
 * @brief Implementation of the Assigned concrete state.
 *
 * A Participant has been given the WorkItem, but work has not started.
 */

#include "Assigned.h"
#include "InProgress.h"
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
 * @brief Starts work on the WorkItem.
 *
 * Moves the WorkItem from Assigned to InProgress.
 *
 * @param c The WorkItem being started.
 * @return true if the WorkItem moved to InProgress, false if c is null.
 */
bool Assigned::start(WorkItem* c) {
    if (c == nullptr) {
        return false;
    }
    changeState(c, new InProgress());
    return true; // this has been deleted so return
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
 * @brief Attempts to complete the WorkItem.
 *
 * Not allowed: work must be started before it can be completed.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Assigned::complete(WorkItem* c) {
    (void)c;
    return false;
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
    changeState(c, new Rejected());
    return true; // this has been deleted, so return immediately
}