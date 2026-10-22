#include "WorkItemSnapshot.h"

WorkItemSnapshot::WorkItemSnapshot(vector<string> requests, WorkItemState* state) : requests(requests), state(state) {}

vector<string> WorkItemSnapshot::getRequests() {
    return requests;
}

WorkItemState* WorkItemSnapshot::getState() {
    return state;
}