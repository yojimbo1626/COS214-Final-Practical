/**
 * @file Client.cpp
 * @brief Implementation of Client, a ConcreteObserver in the Observer pattern.
 *
 * A Client is the person who requested the work. It subscribes to a WorkItem
 * so it hears about every change in progress.
 */

#include "Client.h"
#include "Worker.h"
#include "ClientProcessor.h"
#include <iostream>

/**
 * @brief Creates a Client.
 * @param name The Client's display name.
 */
Client::Client(std::string name)
    : Participant(name), processor(nullptr) {}

/**
 * @brief Destroys the Client.
 */
Client::~Client() {}

void Client::requestProcessor(Worker* worker) {
    if (worker)
        processor = worker->createClientProcessor();
}

void Client::viewAvailableOutputs() {
    if (processor == nullptr)
        return;

    std::vector<std::string> outputs = processor->getAvailableOutputs();

    std::cout << "\nAvailable Outputs:\n";

    for (size_t i = 0; i < outputs.size(); i++) {
        std::cout << "- " << outputs[i] << std::endl;
    }
}

bool Client::addRequest(std::string request) {
    if (processor)
        return processor->addRequest(request);
}

bool Client::addSpecialRequest(Worker* worker, std::string output, std::vector<std::string> features) {
    if (processor)
        return worker->addToRegistry(output, features);
}

void Client::receiveResults(std::vector<OutputComponent*> outputs) {
    for (OutputComponent* output : outputs)
        delete output;
    processor = nullptr;
}

void Client::cancelWork() {
    processor->cancelWork();
}

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
