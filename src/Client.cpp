/**
 * @file Client.cpp
 * @brief Implementation of Client, a ConcreteObserver in the Observer pattern.
 *
 * A Client is the person who requested the work. It subscribes to a WorkItem
 * so it hears about every change in progress.
 */

#include "Client.h"
#include <iostream>

/**
 * @brief Creates a Client.
 * @param name The Client's display name.
 */
Client::Client(std::string name)
    : Participant(name) {}

/**
 * @brief Destroys the Client.
 */
Client::~Client() {}

/**
 * @brief Receives a pushed update from a WorkItem this Client observes.
 *
 * Push model: the WorkItem sends all the information in the message, so
 * the Client never has to query the WorkItem.
 *
 * @param update The message pushed by the WorkItem.
 */
void Client::receiveUpdate(std::string update) {
    std::cout << "[" << toString() << "] received update: " << update << std::endl;
}

/**
 * @brief Describes this Client.
 * @return A string in the form "Client #<id> (<name>)".
 */
std::string Client::toString() {
    return "Client #" + std::to_string(getId()) + " (" + getName() + ")";
}
