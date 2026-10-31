/**
 * @file Participant.cpp
 * @brief Implementation of Participant, the abstract Observer in the Observer pattern.
 *
 * A Participant can subscribe to a WorkItem (the Subject). When the WorkItem
 * changes, it pushes an update message to every subscribed Participant
 * through receiveUpdate(). Client and Worker are the concrete observers.
 */

#include "Participant.h"

/**
 * @brief Shared counter used to give every Participant a unique id.
 *
 * Starts at 0. The first Participant created gets id 1.
 */
int Participant::idCounter = 0;

/**
 * @brief Creates a Participant with a unique id.
 * @param name The Participant's display name.
 */
Participant::Participant(std::string name)
    : id(++idCounter), name(name) {}

/**
 * @brief Virtual destructor so Clients and Workers are destroyed correctly
 *        through a Participant pointer.
 *
 * @warning A WorkItem does not own its observers. Remove a Participant from
 *          every WorkItem it observes (WorkItem::removeObserver) before
 *          deleting it, otherwise the WorkItem keeps a dangling pointer.
 */
Participant::~Participant() {}

/**
 * @brief Returns the Participant's unique id.
 * @return The id assigned at construction.
 */
int Participant::getId() const {
    return id;
}

/**
 * @brief Returns the Participant's name.
 * @return The name given at construction.
 */
std::string Participant::getName() const {
    return name;
}