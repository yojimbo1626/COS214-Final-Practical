/**
 * @file WorkItem.cpp
 * @brief Implementation of WorkItem, the Context in the State pattern.
 *
 * Every request (start, assign, complete, cancel, process) is forwarded to
 * the current WorkItemState, which decides what happens and whether the
 * WorkItem moves to a new state.
 */

#include "WorkItem.h"
#include "WorkItemState.h"
#include "Available.h"

/**
 * @brief Creates a WorkItem in the Available state.
 * @param name        Short name of the WorkItem.
 * @param description Longer description of the work to be done.
 */
WorkItem::WorkItem(const std::string& name, const std::string& description)
    : name(name), description(description), state(new Available()) {}

/**
 * @brief Copy constructor.
 *
 * Copies the name and description. The copy always starts in the
 * Available state, because the states have no clone() method to copy
 * the original's current state.
 *
 * @param workItem The WorkItem to copy.
 */
WorkItem::WorkItem(const WorkItem& workItem)
    : name(workItem.name), description(workItem.description), state(new Available()) {}

/**
 * @brief Destroys the WorkItem and the state it owns.
 */
WorkItem::~WorkItem() {
    delete state;
}

/**
 * @brief Returns the WorkItem's name.
 * @return The name.
 */
std::string WorkItem::getName() const {
    return name;
}

/**
 * @brief Returns the WorkItem's description.
 * @return The description.
 */
std::string WorkItem::getDescription() const {
    return description;
}

/**
 * @brief Requests that work on this WorkItem starts.
 * @return true if the current state allowed it (Assigned to InProgress).
 */
bool WorkItem::start() {
    return state->start(this);
}

/**
 * @brief Requests that this WorkItem is assigned to a Participant.
 * @param p The Participant receiving the work.
 * @return true if the current state allowed it (Available to Assigned).
 */
bool WorkItem::assign(Participant* p) {
    return state->assign(this, p);
}

/**
 * @brief Requests that this WorkItem is completed.
 * @return true if the current state allowed it (InProgress to Completed).
 */
bool WorkItem::complete() {
    return state->complete(this);
}

/**
 * @brief Requests that this WorkItem is cancelled.
 * @return true if the current state allowed it (Available or Assigned to Rejected).
 */
bool WorkItem::cancel() {
    return state->cancel(this);
}

/**
 * @brief Asks the current state to describe itself.
 * @return The name of the current state, e.g. "InProgress".
 */
std::string WorkItem::process() {
    return state->process();
}

/**
 * @brief Returns the current state.
 * @return Pointer to the current state. The WorkItem still owns it,
 *         so the caller must not delete it.
 */
WorkItemState* WorkItem::getState() const {
    return state;
}

/**
 * @brief Replaces the current state and deletes the old one.
 *
 * Private: only WorkItemState (through WorkItemState::changeState) may call it.
 * Does nothing if newState is null or already the current state.
 *
 * @param newState The new state. The WorkItem takes ownership of it.
 */
void WorkItem::setState(WorkItemState* newState) {
    if (newState == nullptr || newState == state) {
        return;
    }
    WorkItemState* old = state;
    state = newState;
    delete old;
}