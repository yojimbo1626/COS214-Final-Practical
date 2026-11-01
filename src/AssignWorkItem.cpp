#include "../include/AssignWorkItem.h"
#include "../include/WorkItem.h"

AssignWorkItem::AssignWorkItem(WorkItem* workItem) : Command(workItem), worker(nullptr)  {}

bool AssignWorkItem::execute() 
{
    if (workItem == nullptr || worker == nullptr) 
    {
        return false;
    }

    return workItem->assign(worker);
}

void AssignWorkItem::defineWorker(Participant* worker) 
{
    this->worker = worker;
}
