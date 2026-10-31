#include "../include/CompleteWorkItem.h"
#include "../include/WorkItem.h"

CompleteWorkItem::CompleteWorkItem(WorkItem* workItem) : Command(workItem) {}

bool CompleteWorkItem::execute() {
    if (workItem == nullptr) return false;
    return workItem->complete();
}
