/**
 * @file WorkItemState.cpp
 * @brief Implementation of the abstract State base class WorkItemState.
 */

#include "WorkItemState.h"
#include "WorkItem.h"

/**
 * @brief Virtual destructor so concrete states are destroyed correctly
 *        through a WorkItemState pointer.
 */
WorkItemState::~WorkItemState() {}

/**
 * @brief Moves a WorkItem into a new state.
 *
 * WorkItem::setState is private and only WorkItemState is a friend.
 * Friendship is not inherited, so concrete states call this protected
 * helper instead of calling setState directly.
 *
 * @param c    The WorkItem (Context) whose state is changing.
 * @param next The new state. The WorkItem takes ownership of it.
 *
 * @warning The WorkItem deletes its previous state, which is usually the
 *          calling object (this). The caller must return immediately
 *          after this call and must not touch any of its own members.
 */
void WorkItemState::changeState(WorkItem* c, WorkItemState* next) {
    c->setState(next);
}