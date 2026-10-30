/**
 * @file InProgress.cpp
 * @brief Implementation of the InProgress concrete state.
 *
 * Work on the WorkItem has started. The only way forward is to complete it.
 * Cancelling is no longer allowed once work has started.
 */

#include "InProgress.h"
#include "Completed.h"
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
 * @brief Attempts to assign the WorkItem.
 *
 * Not allowed: the WorkItem is already being worked on.
 *
 * @param c The WorkItem (unused).
 * @param p The Participant (unused).
 * @return Always false. The state does not change.
 */
bool InProgress::assign(WorkItem* c, Participant* p) {
    (void)c;
    (void)p;
    return false;
}

/**
 * @brief Completes the WorkItem.
 *
 * Moves the WorkItem from InProgress to Completed.
 *
 * @param c The WorkItem being completed.
 * @return true if the WorkItem moved to Completed, false if c is null.
 */
bool InProgress::complete(WorkItem* c) {
    if (c == nullptr) {
        return false;
    }
    changeState(c, new Completed());
    return true; // this has been deleted, so return immediately
}

/**
 * @brief Attempts to cancel the WorkItem.
 *
 * Not allowed: a WorkItem cannot be cancelled once work has started.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool InProgress::cancel(WorkItem* c) {
    (void)c;
    return false;
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
