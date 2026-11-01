/**
 * @file WorkItemState.cpp
 * @brief Implementation of the abstract State base class WorkItemState.
 *
 * Concrete states move a WorkItem to a new state by calling
 * WorkItem::setState(). The WorkItem deletes its previous state (usually
 * the calling object), so a concrete state must return immediately after
 * calling setState() and must not touch any of its own members.
 */

#include "WorkItemState.h"

/**
 * @brief Virtual destructor so concrete states are destroyed correctly
 *        through a WorkItemState pointer.
 */
WorkItemState::~WorkItemState() {}