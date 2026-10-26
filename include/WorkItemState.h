#ifndef WORKITEMSTATE_H
#define WORKITEMSTATE_H

#include <string>

class WorkItem;
class Participant;

/**
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

protected:
    /**
     * @brief Lets concrete states change the WorkItem's state.
     *
     * WorkItem::setState is private and WorkItem only befriends this base
     * class. Friendship is not inherited in C++, so subclasses go through
     * this helper.
     *
     * WARNING: the WorkItem deletes its old state (usually `this`), so
     * return straight after calling this. Don't touch members afterwards.
     */
    static void changeState(WorkItem* c, WorkItemState* next);
};

#endif
