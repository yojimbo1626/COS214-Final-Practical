#include "WorkloadExecution.h"
#include "WorkItem.h"

std::string WorkloadExecution::execute(WorkItem* w) {
    // TODO
    if(w == nullptr) 
        return "WorkloadExecution: no work item supplied";
    (void)w;
    return "WorkloadExecution: dependent on the workload assigned";
}
