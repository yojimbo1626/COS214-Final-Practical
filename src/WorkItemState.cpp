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