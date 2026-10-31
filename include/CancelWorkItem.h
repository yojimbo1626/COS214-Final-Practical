#ifndef CANCELWORKITEM_H
#define CANCELWORKITEM_H

#include "Command.h"

/**
 * @class CancelWorkItem
 * @brief Concrete command that cancels a work item.
 */
class CancelWorkItem : public Command {
public:
    /**
     * @brief Creates a CancelWorkItem command.
     * @param workItem The work item to cancel.
     */
    explicit CancelWorkItem(WorkItem* workItem);

    /**
     * @brief Cancels the work item.
     * @return true if the work item was cancelled, false otherwise.
     */
    bool execute() override;
};

#endif
