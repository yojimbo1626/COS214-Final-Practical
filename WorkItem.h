// ONLY MEMENTO ATTRIBUTES AND OPERATIONS DECLARED

#ifndef WORK_ITEM
#define WORK_ITEM

#include "WorkItemSnapshot.h"

#include <string>
#include <vector>

using namespace std;

class WorkItemState;

class WorkItem {
    private:
        vector<string> requests;
        WorkItemState* state;

    public:
        WorkItemSnapshot* createSnapshot();
        void setSnapshot(WorkItemSnapshot* snapshot);

    private:
        void setState(WorkItemState* state);
};

#endif