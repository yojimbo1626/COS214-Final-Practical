#ifndef REJECTED_H
#define REJECTED_H

#include "WorkItemState.h"
/**
 * @class Rejected
 * @brief ConcreteState: Rejected
 * Final state
 */
class Rejected : public WorkItemState {
public:
    std::string process();
    bool start(WorkItem* c);
    bool assign(WorkItem* c, Participant* p);
    bool complete(WorkItem* c);
    bool cancel(WorkItem* c);
    bool blocked(WorkItem* b);
};

#endif
