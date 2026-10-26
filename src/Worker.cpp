/**
 * @file Worker.cpp
 * @brief Implementation of Worker, the Context in the Strategy pattern.
 *
 * A Worker holds one TaskExecutionStrategy at a time and can swap it at
 * runtime. The Worker owns its strategy (composition).
 */

#include "Worker.h"
#include "TaskExecutionStrategy.h"
#include "RoleExecution.h"

/**
 * @brief Creates a Worker that starts with the RoleExecution strategy.
 * @param name The Worker's name.
 * @todo Pass name to Participant(name) once Worker inherits from Participant.
 */
Worker::Worker(const std::string& name)
    : taskExecutionStrategy(new RoleExecution()) {
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
