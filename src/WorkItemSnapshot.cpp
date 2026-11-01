#include "WorkItemSnapshot.h"
#include "WorkItemState.h"

WorkItemSnapshot::WorkItemSnapshot(std::vector<std::string> requests, WorkItemState* state)
    : requests(requests), state(state) {}

WorkItemSnapshot::~WorkItemSnapshot() {
    delete state;
}
