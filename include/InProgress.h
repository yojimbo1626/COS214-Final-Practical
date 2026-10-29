#ifndef INPROGRESS_H
#define INPROGRESS_H

#include "WorkItemState.h"

/**
 * @class InProgress
 * @brief ConcreteState: InProgress.
 */
class InProgress : public WorkItemState {
public:
    std::string process();
    bool start(WorkItem* c);
    bool assign(WorkItem* c, Participant* p);
    bool complete(WorkItem* c);
    bool cancel(WorkItem* c);
    bool blocked(WorkItem* b);
};

#endif
