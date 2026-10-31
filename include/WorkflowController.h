#ifndef WORKFLOWCONTROLLER_H
#define WORKFLOWCONTROLLER_H

#include <string>

class Command;
class WorkItem;
class Worker;

/**
 * @class WorkflowController
 * @brief Invoker in the Command pattern: holds one command for each workflow
 *        action and runs the right one when asked.
 *
 * The controller creates its five commands once, for a single WorkItem, and
 * deletes them when it is destroyed. The SystemProcessor (facade) only ever
 * talks to the controller, never to the commands directly.
 */
class WorkflowController {
private:
    WorkItem* workItem;           
    Command* addRequestCommand;   
    Command* start;               
    Command* assign;              
    Command* complete;            
    Command* cancel;              

public:
    /**
     * @brief Creates a controller and its five commands for one work item.
     * @param workItem The work item the commands will act on.
     */
    explicit WorkflowController(WorkItem* workItem);

    /** @brief Deletes the five commands. The work item is not deleted. */
    ~WorkflowController();

    /** @brief Controllers own their commands, so they cannot be copied. */
    WorkflowController(const WorkflowController&) = delete;

    /** @brief Controllers own their commands, so they cannot be copied. */
    WorkflowController& operator=(const WorkflowController&) = delete;

    /**
     * @brief Adds a request to the work item.
     * @param request The request text.
     * @return true if the request was added, false otherwise.
     */
    bool addRequest(std::string request);

    /**
     * @brief Starts the work item.
     * @return true if the work item started, false otherwise.
     */
    bool startWorkflow();

    /**
     * @brief Assigns the work item to a worker.
     * @param worker The worker taking the work item.
     * @return true if the work item was assigned, false if there is no worker
     *         or the work item could not be assigned.
     */
    bool assignWorkItem(Worker* worker);

    /**
     * @brief Marks the work item as completed.
     * @return true if the work item was completed, false otherwise.
     */
    bool completeWorkItem();

    /**
     * @brief Cancels the work item.
     * @return true if the work item was cancelled, false otherwise.
     */
    bool cancelWorkItem();
};

#endif
