#ifndef ASSIGNWORKITEM_H
#define ASSIGNWORKITEM_H

#include "Command.h"

class Participant;

/**
 * @class AssignWorkItem
 * @brief Concrete Command that assigns a work item to a participant.
 *
 * Like AddRequest, it has to be told what to act on before it runs: call
 * defineWorker() to say who is being assigned, then execute().
 */
class AssignWorkItem : public Command {
private:
    Participant* worker; ///< Who the work item is assigned to. set by defineWorker(), nullptr until assigned.

public:
    /**
     * @brief Creates an AssignWorkItem command.
     * @param workItem The work item to assign.
     */
    explicit AssignWorkItem(WorkItem* workItem);

    /**
     * @brief Assigns the work item to the participant set by defineWorker().
     * @return true if the work item was assigned, false if there is no work
     *         item, no worker has been defined, or the work item refused.
     */
    bool execute() override;

    /**
     * @brief Sets who the work item will be assigned to.
     * @param worker The participant taking the work item.
     */
    void defineWorker(Participant* worker);
};

#endif
