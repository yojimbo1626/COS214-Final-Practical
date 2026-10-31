#include "../include/CancelWorkItem.h"
#include "../include/WorkItem.h"

CancelWorkItem::CancelWorkItem(WorkItem* workItem) : Command(workItem) {}

bool CancelWorkItem::execute() {
    if (workItem == nullptr) return false;
    return workItem->cancel();
}
