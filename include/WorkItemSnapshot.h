#ifndef WORK_ITEM_SNAPSHOT
#define WORK_ITEM_SNAPSHOT

#include <string>
#include <vector>

using namespace std;

class WorkItemState;
class WorkItem;

/**
 * @class WorkItemSnapshot
 * @brief Stores the information needed to restore a work item to a previous state.
 *
 * A snapshot stores a copy of the work item's requests and a pointer to its state.
 * The WorkItem class is a friend so that it can access the snapshot's private data.
 */
class WorkItemSnapshot {
    private:
        /** The requests associated with the work item at the time of the snapshot. */
        vector<string> requests;

        /** The state of the work item at the time of the snapshot. */
        WorkItemState* state;

        friend WorkItem;

    public:
        /**
         * @brief Constructs a snapshot of a work item's requests and state.
         * @param requests The requests to store in the snapshot.
         * @param state A pointer to the work item's current state.
         */
        WorkItemSnapshot(vector<string> requests, WorkItemState* state);
};

#endif