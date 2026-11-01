/**
 * @file TaskExecutionStrategy.cpp
 * @brief Implementation of the abstract Strategy base class TaskExecutionStrategy.
 */

#include "TaskExecutionStrategy.h"

/**
 * @brief Virtual destructor so concrete strategies are destroyed correctly
 *        through a TaskExecutionStrategy pointer.
 */
TaskExecutionStrategy::~TaskExecutionStrategy() {}
