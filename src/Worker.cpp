/**
 * @file Worker.cpp
 * @brief For Obsever
 * Worker plays two roles:
 * - Context in the Strategy pattern: it holds one TaskExecutionStrategy at a
 *   time, can swap it at runtime, and owns it (composition).
 * - ConcreteObserver in the Observer pattern: it is notified when a WorkItem
 *   it has been assigned to changes.
 */

#include "Worker.h"
#include "Client.h"
#include "OutputProducer.h"
#include "WorkOutput.h"
#include "WorkOutputFeature.h"
#include "OutputRegistry.h"
#include "SystemProcessor.h"
#include "ClientProcessor.h"
#include "WorkItem.h"
#include "WorkItemHistory.h"
#include "TaskExecutionStrategy.h"
#include "RoleExecution.h"
#include <iostream>

/**
 * @brief Creates a Worker that starts with the RoleExecution strategy, one of the concrete strategies.
 * @param name The Worker's name.
 * @todo Pass name to Participant(name) once Worker inherits from Participant.
 */
Worker::Worker(std::string name)
    : Participant(name), producer(new OutputProducer), registry(nullptr), processor(nullptr), 
      taskExecutionStrategy(new RoleExecution()) {}

/**
 * @brief Destroys the Worker and the strategy it owns.
 */
Worker::~Worker() {
    delete producer;
    delete taskExecutionStrategy;
}

void Worker::assignProcessor(SystemProcessor* processor) {
    this->processor = processor;
}

ClientProcessor* Worker::createClientProcessor() {
    if (processor == nullptr) {
        processor = new SystemProcessor;
        processor->startWorkflow(new WorkItem(workItemName, workItemDescription));
        return new ClientProcessor(processor);
    }   
    else 
        return nullptr;
}

bool Worker::assignWorkItem(Worker* worker) {
    if (processor != nullptr && processor->assignWorkItem(worker)) 
        processor = nullptr;
    else  
        return false; 
}

bool Worker::completeWorkItem() {
    if (processor != nullptr) {
        if (processor->workItem != nullptr && processor->workItem->getClient() != nullptr) {
            taskExecutionStrategy->execute(processor->workItem);
            processor->workItem->getClient()->receiveResults(processor->produceOutput());
            return processor->completeWork();
        }
    }
    return false;
}

void Worker::undoLastAction() {
    if (processor != nullptr && processor->workItem != nullptr && processor->workItemHistory != nullptr) {
        processor->workItem->setSnapshot(processor->workItemHistory->undo());
    }
}

void Worker::rejectWorkItem() {
    if (processor != nullptr) processor->cancelWork();
}

std::string Worker::getWorkItemName() const {
    return workItemName;
}

std::string Worker::getWorkItemDescription() const {
    return workItemDescription;
}

void Worker::setWorkItemDescription(std::string workItemDescription) {
    this->workItemDescription = workItemDescription;
}

void Worker::setRegistry(OutputRegistry* registry) {
    this->registry = registry;
}

bool Worker::addToRegistry(std::string output, std::vector<std::string> features, bool client) {
    if (registry == nullptr || (client && processor == nullptr))
        return false;

    OutputComponent* product = nullptr;

    if (client) {
        product = registry->createOutput(output);
    }
    else {
        product = producer->produceOutput(output);
    }
    if (product == nullptr || dynamic_cast<WorkOutput*>(product) == nullptr) {
        delete product;
        return false;
    }

    for (size_t i = 0; i < features.size(); i++) {
        if (client) {
            OutputComponent* registeredFeature = registry->createOutput(features[i]);
            if (registeredFeature == nullptr || dynamic_cast<OutputFeature*>(registeredFeature) == nullptr) {
                delete registeredFeature;
                delete product;
                return false;
            }
            delete registeredFeature;
        }
        WorkOutputFeature* feature = new WorkOutputFeature(features[i]);
        feature->add(product);
        product = feature;
    }
    registry->registerOutput(product->printOutput(), product);
    if (client) {
        return processor->addRequest(product->printOutput());
    }
    return true;
}


/**
 * @brief Returns the current execution strategy.
 * @return Pointer to the current strategy. The Worker still owns it,
 *         so the caller must not delete it.
 */
TaskExecutionStrategy* Worker::getTaskExecutionStrategy() const {
    return taskExecutionStrategy;
}

/**
 * @brief Replaces the current execution strategy and deletes the old one.
 *
 * Does nothing if newStrategy is null (a Worker must always have a strategy)
 * or if it is already the current strategy.
 *
 * @param newStrategy The new strategy. The Worker takes ownership of it.
 */
void Worker::setTaskExecutionStrategy(TaskExecutionStrategy* newStrategy) {
    if (newStrategy == nullptr || newStrategy == taskExecutionStrategy) {
        return;
    }
    delete taskExecutionStrategy;
    taskExecutionStrategy = newStrategy;
}

/**
 * @brief Receives a pushed update from a WorkItem this Worker observes.
 *
 * Push model: the WorkItem sends all the information in the message, so
 * the Worker never has to query the WorkItem.
 *
 * @param update The message pushed by the WorkItem.
 */
void Worker::receiveUpdate(std::string update) {
    std::cout << "[" << toString() << "] received update: " << update << std::endl;
}

/**
 * @brief Describes this Worker.
 * @return A string in the form "Worker #<id> (<name>)".
 */
std::string Worker::toString() {
    return "Worker #" + std::to_string(getId()) + " (" + getName() + ")";
}
