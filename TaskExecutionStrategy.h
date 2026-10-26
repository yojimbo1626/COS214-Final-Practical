#ifndef TASKEXECUTIONSTRATEGY_H
#define TASKEXECUTIONSTRATEGY_H

#include <string>

class WorkItem;

/**
 * @brief Abstract Strategy in the Strategy pattern.
 *
 * Each concrete strategy is a different way for a Worker to execute
 * a WorkItem. The Worker can swap strategies at runtime.
 */
class TaskExecutionStrategy {
public:
    virtual ~TaskExecutionStrategy();
    virtual std::string execute(WorkItem* w) = 0;
};

#endif
