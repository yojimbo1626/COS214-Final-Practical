#ifndef RULEEXECUTION_H
#define RULEEXECUTION_H

#include "TaskExecutionStrategy.h"

/**
 * @brief ConcreteStrategy: RuleExecution.
 */
class RuleExecution : public TaskExecutionStrategy {
public:
    std::string execute(WorkItem* w);
};

#endif
