/**
 * @file Rejected.cpp
 * @brief Implementation of the Rejected concrete state.
 *
 * The WorkItem was cancelled before work started.
 * This is a final state: every request is refused and the state never changes.
 */

#include "Rejected.h"
#include "WorkItem.h"

/**
 * @brief Returns the name of this state.
 * @return The string "Rejected".
 */
std::string Rejected::process() {
    return "Rejected";
}

/**
 * @brief Attempts to start the WorkItem.
 *
 * Not allowed: the WorkItem has been rejected.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Rejected::start(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Attempts to assign the WorkItem.
 *
 * Not allowed: the WorkItem has been rejected.
 *
 * @param c The WorkItem (unused).
 * @param p The Participant (unused).
 * @return Always false. The state does not change.
 */
bool Rejected::assign(WorkItem* c, Participant* p) {
    (void)c;
    (void)p;
    return false;
}

/**
 * @brief Attempts to complete the WorkItem.
 *
 * Not allowed: the WorkItem has been rejected.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Rejected::complete(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Attempts to cancel the WorkItem.
 *
 * Not allowed: the WorkItem has been rejected.
 *
 * @param c The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Rejected::cancel(WorkItem* c) {
    (void)c;
    return false;
}

/**
 * @brief Attempts to block the WorkItem.
 *
 * Not allowed: the WorkItem has been rejected.
 *
 * @param b The WorkItem (unused).
 * @return Always false. The state does not change.
 */
bool Rejected::blocked(WorkItem* b) {
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
 * @return A new Rejected object. The caller takes ownership of it.
 */
WorkItemState* Rejected::clone() {
    return new Rejected();
}