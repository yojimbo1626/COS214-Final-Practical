#include "WorkItemHistory.h"

WorkItemHistory::~WorkItemHistory() {
    for (WorkItemSnapshot* snapshot : history) 
        delete snapshot;  
}

void WorkItemHistory::save(WorkItemSnapshot* snapshot) {
    history.push_back(snapshot);
}

WorkItemSnapshot* WorkItemHistory::undo() {
    if (history.empty()) return nullptr;

    WorkItemSnapshot* snapshot = history.back();
    history.pop_back();

    return snapshot;
}