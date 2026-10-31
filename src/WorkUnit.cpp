/**
 * @file WorkUnit.cpp
 * @brief Implementation of WorkUnit, the abstract Subject in the Observer pattern.
 *
 * WorkUnit manages the list of observers (Participants) and pushes update
 * messages to them. It does not know what the messages mean: concrete
 * subjects such as WorkItem decide when to notify and what to send.
 */

#include "WorkUnit.h"
#include "Participant.h"
#include <algorithm>

/**
 * @brief Subscribes a Participant to this WorkUnit's updates.
 *
 * Ignores null pointers and Participants that are already subscribed,
 * so nobody receives the same update twice.
 *
 * @param participant The Participant to subscribe. The WorkUnit does not
 *                    take ownership of it.
 */
void WorkUnit::addObserver(Participant* participant) {
    if (participant == nullptr) {
        return;
    }
    if (std::find(observers.begin(), observers.end(), participant) != observers.end()) {
        return;
    }
    observers.push_back(participant);
}

/**
 * @brief Unsubscribes a Participant from this WorkUnit's updates.
 *
 * Does nothing if the Participant is not subscribed.
 *
 * @param participant The Participant to unsubscribe. It is not deleted.
 */
void WorkUnit::removeObserver(Participant* participant) {
    std::vector<Participant*>::iterator it =
        std::find(observers.begin(), observers.end(), participant);
    if (it != observers.end()) {
        observers.erase(it);
    }
}

/**
 * @brief Pushes an update message to every subscribed Participant.
 *
 * Push model: the concrete subject builds the message and passes it in,
 * so observers get everything they need without querying the subject.
 *
 * Loops over a copy of the observer list, so an observer can safely
 * unsubscribe itself from inside receiveUpdate().
 *
 * @param update The message to send to every observer.
 */
void WorkUnit::notify(std::string update) {
    std::vector<Participant*> toNotify = observers;
    for (std::vector<Participant*>::size_type i = 0; i < toNotify.size(); ++i) {
        toNotify[i]->receiveUpdate(update);
    }
}

/**
 * @brief Pure virtual destructor: makes WorkUnit abstract.
 *
 * It still needs a body, because every derived class destructor calls it.
 * The observers are not deleted: the WorkUnit does not own them.
 */
WorkUnit::~WorkUnit() {}