#ifndef ASSIGNED_H
#define ASSIGNED_H

#include "WorkItemState.h"

/**
 * @class Assigned
 * @brief ConcreteState: the WorkItem has a Participant but work hasn't started.
 *
 * Allowed: start() -> InProgress, cancel() -> Rejected. Everything else is refused.
 *
 */
class Assigned : public WorkItemState {
public:
    std::string process();
    bool start(WorkItem* c);
    bool assign(WorkItem* c, Participant* p);
    bool complete(WorkItem* c);
    bool cancel(WorkItem* c);
};

#endif
