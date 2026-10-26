#ifndef ROLEEXECUTION_H
#define ROLEEXECUTION_H

#include "TaskExecutionStrategy.h"

/**
 * @brief ConcreteStrategy: RoleExecution.
 */
class RoleExecution : public TaskExecutionStrategy {
public:
    std::string execute(WorkItem* w) override;
};

#endif
