/**
 * @file RuleExecution.cpp
 * @brief Implementation of the RuleExecution concrete strategy.
 *
 * Checks a set of business rules before running a WorkItem. If any rule
 * fails, the WorkItem is not touched.
 */

#include "RuleExecution.h"
#include "WorkItem.h"

/**
 * @brief Checks the rules, then starts and completes the WorkItem.
 *
 * Rules:
 * 1. The WorkItem must have a name.
 * 2. The WorkItem must have a description.
 * 3. The WorkItem must be in the Assigned state.
 *
 * @param w The WorkItem to execute.
 * @return A log message naming the broken rule, or confirming success.
 */
std::string RuleExecution::execute(WorkItem* w) {
    if (w == nullptr) {
        return "RuleExecution: no WorkItem given";
    }
    if (w->getName().empty()) {
        return "RuleExecution: rule failed, WorkItem has no name";
    }
    if (w->getDescription().empty()) {
        return "RuleExecution: rule failed, '" + w->getName() + "' has no description";
    }
    if (w->process() != "Assigned") {
        return "RuleExecution: rule failed, '" + w->getName()
               + "' must be Assigned (state: " + w->process() + ")";
    }
    if (!w->start() || !w->complete()) {
        return "RuleExecution: rules passed but '" + w->getName() + "' could not be executed";
    }
    return "RuleExecution: all rules passed, '" + w->getName() + "' executed";
}
