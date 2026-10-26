#include "StartWorkflow.h"
#include "WorkItem.h"

StartWorkflow::StartWorkflow(WorkItem* workItem) : Command(workItem) {}

bool StartWorkflow::execute() {
    if (workItem == nullptr) return false;
    return workItem->start();
}
