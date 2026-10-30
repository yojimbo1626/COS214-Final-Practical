/**
 * @file Completed.cpp
 * @brief Implementation of the Completed concrete state.
 *
 * The WorkItem has been finished successfully.
 * This is a final state: every request is refused and the state never changes.
 */

#include "Completed.h"
#include "WorkItem.h"

/**
 * @brief Returns the name of this state.
 * @return The string "Completed".
 */
std::string Completed::process() {
    return "Completed";
}

/**
 * @brief Attempts to start the WorkItem.
 *
 * Not allowed: the WorkItem is already completed.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Completed::start(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Attempts to assign the WorkItem.
 *
 * Not allowed: the WorkItem is already completed.
 *
 * @param c The WorkItem (unused).
 * @param p The Participant (unused).
 * @return Always false. The state does not change.
 */
bool Completed::assign(WorkItem* c, Participant* p) {
    (void)c;
    (void)p;
    return false;
}

/**
 * @brief Attempts to complete the WorkItem.
 *
 * Not allowed: the WorkItem is already completed.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Completed::complete(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Attempts to cancel the WorkItem.
 *
 * Not allowed: the WorkItem is already completed.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Completed::cancel(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Attempts to block the WorkItem.
 *
 * Not allowed: the WorkItem is already completed.
 *
 * @param b The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Completed::blocked(WorkItem* b) {
    (void)b;
    return false;
}
