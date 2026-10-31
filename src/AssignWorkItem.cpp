#include "../include/AssignWorkItem.h"
#include "../include/WorkItem.h"

AssignWorkItem::AssignWorkItem(WorkItem* workItem) : Command(workItem) {}

bool AssignWorkItem::execute() {
    if (workItem == nullptr) return false;
    return workItem->assign();
}
