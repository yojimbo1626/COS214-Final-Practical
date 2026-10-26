#ifndef COMMAND_H
#define COMMAND_H

class WorkItem;

/**
 * @class Command
 * @brief Abstract base class for all workflow commands.
 *
 * Each command wraps one action on a WorkItem (the receiver) so the action
 * can be stored, passed around and executed later by the WorkflowController.
 */
class Command {
protected:
    WorkItem* workItem;

public:
    /**
     * @brief Creates a command bound to a work item.
     * @param workItem The work item the command will act on.
     */
    explicit Command(WorkItem* workItem);

    /** @brief Virtual destructor so derived commands are cleaned up correctly. */
    virtual ~Command();

    /**
     * @brief Carries out the command on its work item.
     * @return true if the action succeeded, false otherwise.
     */
    virtual bool execute() = 0;
};

#endif
