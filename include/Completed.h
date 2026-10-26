#ifndef COMPLETED_H
#define COMPLETED_H

#include "WorkItemState.h"

/**
 * @class Completed
 * @brief ConcreteState: Completed.
 * Final state
 */
class Completed : public WorkItemState {
public:
    std::string process();
    bool start(WorkItem* c);
    bool assign(WorkItem* c, Participant* p);
    bool complete(WorkItem* c);
    bool cancel(WorkItem* c);
};

#endif
