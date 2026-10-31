#ifndef COMPLETEWORKITEM_H
#define COMPLETEWORKITEM_H

#include "Command.h"

/**
 * @class CompleteWorkItem
 * @brief Concrete command that marks a work item as completed.
 */
class CompleteWorkItem : public Command {
public:
    /**
     * @brief Creates a CompleteWorkItem command.
     * @param workItem The work item to complete.
     */
    explicit CompleteWorkItem(WorkItem* workItem);

    /**
     * @brief Completes the work item.
     * @return true if the work item was completed, false otherwise.
     */
    bool execute() override;
};

#endif
