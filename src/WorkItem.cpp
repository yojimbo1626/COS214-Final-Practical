#include "WorkItem.h"
#include "WorkItemSnapshot.h"

/**
 * @brief Creates a snapshot of the work item's current data and state.
 * @return A pointer to the newly created WorkItemSnapshot.
 */
WorkItemSnapshot* WorkItem::createSnapshot() {
    return new WorkItemSnapshot(requests, state);
}

/**
 * @brief Restores the work item's data and state from a snapshot.
 * @param snapshot Pointer to the snapshot containing the data to restore.
 */
void WorkItem::setSnapshot(WorkItemSnapshot* snapshot) {
    requests = snapshot->requests;
    setState(snapshot->state);
}