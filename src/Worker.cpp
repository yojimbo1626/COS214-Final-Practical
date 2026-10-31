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
#include "TaskExecutionStrategy.h"
#include "RoleExecution.h"
#include <iostream>

/**
 * @brief Creates a Worker that starts with the RoleExecution strategy.
 * @param name The Worker's name.
 * @todo Pass name to Participant(name) once Worker inherits from Participant.
 */
Worker::Worker(const std::string& name)
    : taskExecutionStrategy(new RoleExecution()), Participant(name) {
    (void)name;
}

/**
 * @brief Destroys the Worker and the strategy it owns.
 */
Worker::~Worker() {
    delete taskExecutionStrategy;
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
