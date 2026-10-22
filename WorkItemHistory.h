#ifndef WORK_ITEM_HISTORY
#define WORK_ITEM_HISTORY

#include "WorkItemSnapshot.h"

using namespace std;

class WorkItemHistory {
    private:
        vector<WorkItemSnapshot*> history;
        
    public:
        ~WorkItemHistory();
        void save(WorkItemSnapshot* snapshot);
        WorkItemSnapshot* undo();
};

#endif