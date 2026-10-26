#include "RoleExecution.h"
#include "WorkItem.h"

std::string RoleExecution::execute(WorkItem* w) {
    // TODO
    if (w == nullptr) 
    return "RoleExecution: no work item supplied";

    (void)w;
    return "RoleExecution: dependent on the role assigned";
}
