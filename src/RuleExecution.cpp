#include "RuleExecution.h"
#include "WorkItem.h"

std::string RuleExecution::execute(WorkItem* w) {
    // TODO
    if(w == nullptr) 
        return "RuleExecution: no work item supplied";
    (void)w;
    return "RuleExecution: dependent on the rule assigned";
}
