#ifndef STARTWORKFLOW_H
#define STARTWORKFLOW_H

#include "Command.h"

/**
 * @class StartWorkflow
 * @brief Concrete command that starts a work item.
 */
class StartWorkflow : public Command {
public:
    /**
     * @brief Creates a StartWorkflow command.
     * @param workItem The work item to start.
     */
    explicit StartWorkflow(WorkItem* workItem);

    /**
     * @brief Starts the work item.
     * @return true if the work item started, false otherwise.
     */
    bool execute() override;
};

#endif
