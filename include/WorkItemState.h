#ifndef WORKITEMSTATE_H
#define WORKITEMSTATE_H

#include <string>

class WorkItem;
class Participant;

/**
 * @class WorkItemState
 * @brief Abstract State in the State pattern.
 *
 * Each request receives the Context (WorkItem) so the state can decide
 * what to do and, if needed, move the WorkItem to a new state.
 */
class WorkItemState {
public:
    virtual ~WorkItemState();

    virtual std::string process() = 0;
    virtual bool start(WorkItem* c) = 0;
    virtual bool assign(WorkItem* c, Participant* p) = 0;
    virtual bool complete(WorkItem* c) = 0;
    virtual bool cancel(WorkItem* c) = 0;
    virtual bool blocked(WorkItem* b) = 0;
};

#endif
