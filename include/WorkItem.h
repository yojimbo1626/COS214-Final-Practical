#ifndef WORKITEM_H
#define WORKITEM_H

#include <string>
#include <vector>

class WorkItemState;
class WorkItemSnapshot;
class Participant;

/**
 * @brief Context in the State pattern, and Subject / ConcreteSubject in the
 *        Observer pattern.
 */
class WorkItem {
    public:
        WorkItemSnapshot* createSnapshot();
        void setSnapshot(WorkItemSnapshot* snapshot);

    private:
        WorkItemState* state;
        std::vector<std::string> requests;
};

#endif