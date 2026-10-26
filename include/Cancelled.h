#ifndef CANCELLED_H
#define CANCELLED_H

#include "WorkItemState.h"

/**
 * @class Assigned
 * @brief ConcreteState: Cancelled
 *
 */
class Cancelled : public WorkItemState {
public:
    std::string process();
    bool start(WorkItem* c);
    bool assign(WorkItem* c, Participant* p);
    bool complete(WorkItem* c);
    bool cancel(WorkItem* c);
};

#endif
