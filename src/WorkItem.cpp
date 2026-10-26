/**
 * @file WorkItem.cpp
 * @brief Implementation of WorkItem.
 *
 * WorkItem plays two roles:
 * - Context in the State pattern: every request (start, assign, complete,
 *   cancel, process) is forwarded to the current WorkItemState, which decides
 *   what happens and whether the WorkItem moves to a new state.
 * - Subject and ConcreteSubject in the Observer pattern: it keeps a list of
 *   Participants and pushes an update message to all of them whenever its
 *   state changes (push model).
 */

#include "WorkItem.h"
#include "WorkItemState.h"
#include "Available.h"
#include "Participant.h"
#include <algorithm>

/**
 * @brief Creates a WorkItem in the Available state.
 * @param name        Short name of the WorkItem.
 * @param description Longer description of the work to be done.
 */
WorkItem::WorkItem(std::string name, std::string description)
    : name(name), description(description), state(new Available()) {}

/**
 * @brief Copy constructor.
 *
 * Copies the name and description. The copy always starts in the
 * Available state, because the states have no clone() method to copy
 * the original's current state. The copy starts with no observers:
 * Participants subscribed to the original are not subscribed to the copy.
 *
 * @param workItem The WorkItem to copy.
 */
WorkItem::WorkItem(const WorkItem& workItem)
    : name(workItem.name), description(workItem.description), state(new Available()) {}

/**
 * @brief Destroys the WorkItem and the state it owns.
 *
 * The observers are not deleted: the WorkItem does not own them.
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
 * Called by the concrete states when a transition happens. Does nothing if newState is null or already the current state.
 * After the change, every observer is notified of the new state.
 *
 * @param newState The new state. The WorkItem takes ownership of it.
 *
 * @warning The old state is deleted. When a concrete state calls this on
 *          itself, it must return immediately afterwards.
 */
void WorkItem::setState(WorkItemState* newState) {
    if (newState == nullptr || newState == state) {
        return;
    }
    WorkItemState* old = state;
    state = newState;
    delete old;
    notify();
}

/**
 * @brief Subscribes a Participant to this WorkItem's updates.
 *
 * Ignores null pointers and Participants that are already subscribed,
 * so nobody receives the same update twice.
 *
 * @param participant The Participant to subscribe. The WorkItem does not
 *                    take ownership of it.
 */
void WorkItem::addObserver(Participant* participant) {
    if (participant == nullptr) {
        return;
    }
    if (std::find(observers.begin(), observers.end(), participant) != observers.end()) {
        return;
    }
    observers.push_back(participant);
}

/**
 * @brief Unsubscribes a Participant from this WorkItem's updates.
 *
 * Does nothing if the Participant is not subscribed.
 *
 * @param participant The Participant to unsubscribe. It is not deleted.
 */
void WorkItem::removeObserver(Participant* participant) {
    std::vector<Participant*>::iterator it =
        std::find(observers.begin(), observers.end(), participant);
    if (it != observers.end()) {
        observers.erase(it);
    }
}

/**
 * @brief Pushes an update message to every subscribed Participant.
 *
 * Push model: the message contains the WorkItem's name and current state,
 * so observers get everything they need without querying the WorkItem.
 *
 * Loops over a copy of the observer list, so an observer can safely
 * unsubscribe itself from inside receiveUpdate().
 */
void WorkItem::notify() {
    std::string update = "WorkItem '" + name + "' is now " + state->process();
    std::vector<Participant*> toNotify = observers;
    for (std::vector<Participant*>::size_type i = 0; i < toNotify.size(); ++i) {
        toNotify[i]->receiveUpdate(update);
    }
}
