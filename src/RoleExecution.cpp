/**
 * @file RoleExecution.cpp
 * @brief Implementation of the RoleExecution concrete strategy.
 *
 * Executes a WorkItem only if it has been assigned to someone, i.e. the
 * work belongs to a role-holder. It then runs the WorkItem from start to
 * finish in one go.
 */

#include "RoleExecution.h"
#include "WorkItem.h"

/**
 * @brief Starts and completes a WorkItem that has been assigned.
 *
 * Refuses to run the WorkItem unless it is in the Assigned state.
 *
 * @param w The WorkItem to execute.
 * @return A log message saying what happened.
 */
std::string RoleExecution::execute(WorkItem* w) {
    if (w == nullptr) {
        return "RoleExecution: no WorkItem given";
    }
    if (w->process() != "Assigned") {
        return "RoleExecution: '" + w->getName() + "' is not assigned (state: "
               + w->process() + ")";
    }
    if (!w->start()) {
        return "RoleExecution: could not start '" + w->getName() + "'";
    }
    if (!w->complete()) {
        return "RoleExecution: started but could not complete '" + w->getName() + "'";
    }
    return "RoleExecution: '" + w->getName() + "' executed by its assigned role";
}
