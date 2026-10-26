#ifndef WORKLOADEXECUTION_H
#define WORKLOADEXECUTION_H

#include "TaskExecutionStrategy.h"

/**
 * @brief ConcreteStrategy: WorkloadExecution.
 */
class WorkloadExecution : public TaskExecutionStrategy {
public:
    std::string execute(WorkItem* w) override;
};

#endif
