// ONLY MEMENTO OPERATIONS IMPLEMENTED

#include "WorkItem.h"

WorkItemSnapshot* WorkItem::createSnapshot() {
    return new WorkItemSnapshot(requests, state);
}

void WorkItem::setSnapshot(WorkItemSnapshot* snapshot) {
    requests = snapshot->requests;
    setState(snapshot->state);
}