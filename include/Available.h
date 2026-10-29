#ifndef AVAILABLE_H
#define AVAILABLE_H

#include "WorkItemState.h"

/**
 * @class Available
 * @brief ConcreteState: the WorkItem has a Participant but work hasn't started.
 *
 * Allowed: start() -> InProgress, cancel() -> Rejected. Everything else is refused.
 *
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
