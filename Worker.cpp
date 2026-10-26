#include "Worker.h"
#include "TaskExecutionStrategy.h"
#include "RoleExecution.h"

Worker::Worker(const std::string& name)
    : taskExecutionStrategy(new RoleExecution()) {
    // TODO: pass name to Participant(name) once Worker inherits from it
    (void)name;
}

Worker::~Worker() {
    delete taskExecutionStrategy;
}

TaskExecutionStrategy* Worker::getTaskExecutionStrategy() const {
    return taskExecutionStrategy;
}

void Worker::setTaskExecutionStrategy(TaskExecutionStrategy* newStrategy) {
    if (newStrategy == taskExecutionStrategy || newStrategy == nullptr) return;
    delete taskExecutionStrategy;
    taskExecutionStrategy = newStrategy;
}
