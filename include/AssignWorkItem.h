#ifndef ASSIGNWORKITEM_H
#define ASSIGNWORKITEM_H

#include "Command.h"

/**
 * @class AssignWorkItem
 * @brief Concrete command that assigns a work item.
 */
class AssignWorkItem : public Command {
public:
    /**
     * @brief Creates an AssignWorkItem command.
     * @param workItem The work item to assign.
     */
    explicit AssignWorkItem(WorkItem* workItem);

    /**
     * @brief Assigns the work item.
     * @return true if the work item was assigned, false otherwise.
     */
    bool execute() override;
};

#endif
