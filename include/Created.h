#ifndef CREATED_H
#define CREATED_H

#include "WorkItemState.h"

/**
 * @class Created
 * @brief ConcreteState: Created.
 */
class Created : public WorkItemState {
public:
    std::string process();
    bool start(WorkItem* c);
    bool assign(WorkItem* c, Participant* p);
    bool complete(WorkItem* c);
    bool cancel(WorkItem* c);
    bool blocked(WorkItem* b);
};

#endif
