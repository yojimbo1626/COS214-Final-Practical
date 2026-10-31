#ifndef BLOCKED_H
#define BLOCKED_H

#include "WorkItemState.h"

/**
 * @class Blocked
 * @brief ConcreteState: the WorkItem is waiting for its prerequisite WorkItems to finish.
 *
 * Allowed: blocked() -> Available (release), cancel() -> Rejected.
 * Everything else is refused.
 *
 */

class Blocked : public WorkItemState {
public:
    std::string process();
    bool start(WorkItem* s);
    bool assign(WorkItem* a, Participant* p);
    bool cancel(WorkItem* c);
    bool complete(WorkItem* c);
    bool blocked(WorkItem* b);
};

#endif
