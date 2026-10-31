#ifndef AVAILABLE_H
#define AVAILABLE_H

#include "WorkItemState.h"

/**
 * @class Available
 * @brief ConcreteState: the initial state. The WorkItem exists but has no Participant yet.
 *
 * Allowed: assign(p) -> Assigned, cancel() -> Rejected, blocked() -> Blocked.
 * Everything else is refused.
 */
class Available : public WorkItemState {
public:
    std::string process();
    bool start(WorkItem* c);
    bool assign(WorkItem* c, Participant* p);
    bool complete(WorkItem* c);
    bool cancel(WorkItem* c);
    bool blocked(WorkItem* b);
};

#endif
