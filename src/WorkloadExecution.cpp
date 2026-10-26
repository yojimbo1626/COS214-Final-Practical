/**
 * @file WorkloadExecution.cpp
 * @brief Implementation of the WorkloadExecution concrete strategy.
 *
 * Picks up a WorkItem wherever it currently is and pushes it as far as it
 * can go. Useful for clearing a backlog that contains a mix of items that
 * are assigned and items that are already in progress.
 */

#include "WorkloadExecution.h"
#include "WorkItem.h"

/**
 * @brief Moves a WorkItem forward from its current state towards Completed.
 *
 * - Assigned: starts it, then completes it.
 * - InProgress: completes it.
 * - Any other state: nothing can be done.
 *
 * @param w The WorkItem to execute.
 * @return A log message listing the steps taken.
 */
std::string WorkloadExecution::execute(WorkItem* w) {
    if (w == nullptr) {
        return "WorkloadExecution: no WorkItem given";
    }

    std::string log = "WorkloadExecution: '" + w->getName() + "'";
    bool didSomething = false;

    if (w->process() == "Assigned" && w->start()) {
        log += " started";
        didSomething = true;
    }
    if (w->process() == "InProgress" && w->complete()) {
        log += didSomething ? " and completed" : " completed";
        didSomething = true;
    }
    if (!didSomething) {
        log += " could not be moved forward (state: " + w->process() + ")";
    }
    return log;
}
