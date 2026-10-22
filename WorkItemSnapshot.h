#ifndef WORK_ITEM_SNAPSHOT
#define WORK_ITEM_SNAPSHOT

#include <string>
#include <vector>

using namespace std;

class WorkItemState;

class WorkItemSnapshot {
    private:
        vector<string> requests;
        WorkItemState* state;

    public:
        WorkItemSnapshot(vector<string> requests, WorkItemState* state);
        vector<string> getRequests();
        WorkItemState* getState();
};

#endif