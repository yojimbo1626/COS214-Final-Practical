#ifndef WORK_ITEM_HISTORY
#define WORK_ITEM_HISTORY

#include "WorkItemSnapshot.h"

using namespace std;

/**
 * @class WorkItemHistory
 * @brief Maintains a history of snapshots to support undoing work item changes.
 *
 * Stores snapshots of a work item over time. Snapshots can be saved and retrieved
 * to restore a previous state.
 */
class WorkItemHistory {
    private:
        /** The collection of snapshots stored in the work item's history. */
        vector<WorkItemSnapshot*> history;
        
    public:
        /**
         * @brief Destroys the history and deletes its stored snapshots.
         */
        ~WorkItemHistory();

        /**
         * @brief Saves a snapshot to the history.
         * @param snapshot Pointer to the snapshot to save.
         */
        void save(WorkItemSnapshot* snapshot);

        /**
         * @brief Removes and returns the most recent snapshot for undoing a change.
         * @return Pointer to the most recent snapshot, or nullptr if the history is empty.
         */
        WorkItemSnapshot* undo();
};

#endif