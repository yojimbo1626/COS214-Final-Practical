#ifndef WORK_ITEM_SNAPSHOT
#define WORK_ITEM_SNAPSHOT

#include <string>
#include <vector>

class WorkItemState;
class WorkItem;

/**
 * @class WorkItemSnapshot
 * @brief Memento: stores the information needed to restore a work item to a previous state.
 *
 * A snapshot stores a copy of the work item's requests and its own clone of the work item's state.
 * The snapshot owns that clone and deletes it, unless WorkItem::setSnapshot() has taken it over.
 * The WorkItem class is a friend so that it can access the snapshot's private data.
 */
class WorkItemSnapshot {
    private:
        /** The requests associated with the work item at the time of the snapshot. */
        std::vector<std::string> requests;

        /** Clone of the work item's state at the time of the snapshot (owned by the snapshot). */
        WorkItemState* state;

        friend WorkItem;

    public:
        /**
         * @brief Constructs a snapshot of a work item's requests and state.
         * @param requests The requests to store in the snapshot.
         * @param state A clone of the work item's current state. The snapshot takes ownership of it.
         */
        WorkItemSnapshot(std::vector<std::string> requests, WorkItemState* state);

        /** @brief Deletes the stored state clone (nullptr once a WorkItem has taken it over). */
        ~WorkItemSnapshot();

        WorkItemSnapshot(const WorkItemSnapshot&) = delete;            ///< Not copyable: it owns its state.
        WorkItemSnapshot& operator=(const WorkItemSnapshot&) = delete; ///< Not assignable: it owns its state.
};

#endif
