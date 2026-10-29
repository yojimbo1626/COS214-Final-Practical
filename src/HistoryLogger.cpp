/**
 * @file HistoryLogger.cpp
 * @brief Implementation of HistoryLogger
 *  ConcreteObserver in the Observer pattern.
 *
 * A HistoryLogger subscribes to WorkItems (Subjects) and records every update
 * they push to it, in the order received.
 *
 * Doesn't represent a person like Worker and Client
 *
 * @note This is separate from WorkItemHistory (Memento): WorkItemHistory stores
 *       restorable snapshots, while HistoryLogger stores a readable record of
 *       what happened.
 */

#include "HistoryLogger.h"


// To use it: subscribe the logger to each work item in an instance with item.addObserver(&logger). 
//Then call getLog() from the “View history” menu option.

/**
 * @brief Creates a HistoryLogger with an empty log.
 * @param name Display name, e.g. "Audit Log".
 */
HistoryLogger::HistoryLogger(std::string name)
    : Participant(name) {}

/**
 * @brief Records an update pushed by a WorkItem.
 *
 * Push model: the WorkItem sends the full message, so the logger stores it
 * as-is without querying the WorkItem.
 *
 * @param update The message pushed by the WorkItem, e.g.
 *               "WorkItem 'Report' is now InProgress".
 */
void HistoryLogger::receiveUpdate(std::string update) {
    log.push_back(update);
}

/**
 * @brief Returns every update recorded so far, oldest first.
 * @return A copy of the log, so callers cannot change the recorded history.
 */
std::vector<std::string> HistoryLogger::getLog() const {
    return log;
}

/**
 * @brief Describes this HistoryLogger.
 * @return A string in the form "HistoryLogger #<id> (<name>): <n> entries".
 */
std::string HistoryLogger::toString() {
    return "HistoryLogger #" + std::to_string(getId()) + " (" + getName() + "): "
           + std::to_string(log.size()) + " entries";
}
