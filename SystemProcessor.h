#ifndef SYSTEMPROCESSOR_H
#define SYSTEMPROCESSOR_H

#include <iostream>
#include "WorkflowProcessor.h"
#include "WorkItemHistory.h"

class WorkItem;
class WorkflowController;
class OutputRegistry;
class Worker;
class WorkItemHistory

/**
 * @class SystemProcessor
 * @brief Facade: one simple entry point to the workflow subsystem.
 *
 * Callers talk to the SystemProcessor and it forwards the work to the
 * WorkflowController (which runs the Command objects) behind the scenes.
 */
class SystemProcessor : public WorkflowProcessor {
private:
    WorkItem* workItem;                    
    WorkflowController* workflowController; 
    OutputRegistry* outputRegistry;       
    WorkItemHistory* workItemHistory; 

public:
    /** @brief Creates a processor with no work item or controller yet. */
    SystemProcessor();

    /** @brief Destroys the processor and the controller it created. */
    ~SystemProcessor();

    /**
     * @brief Adds a request to the current work item.
     * @param request The request text.
     * @return true on success, false if there is no active workflow or it failed.
     */
    bool addRequest(std::string request) override;

    /**
     * @brief Cancels the current work item. Does nothing if no workflow is active.
     */
    void cancelWork() override;

    /**
     * @brief Starts the workflow for the given work item.
     * @param workItem The work item to run the workflow on.
     * @return true if the workflow started, false otherwise.
     */
    bool startWorkflow(WorkItem* workItem);

    /**
     * @brief Assigns the current work item to a worker.
     * @param worker The worker taking the work item.
     * @return true on success, false if there is no active workflow or it failed.
     */
    bool assignWorkItem(Worker* worker);

    /**
     * @brief Marks the current work item as completed.
     * @return true on success, false if there is no active workflow or it failed.
     */
    bool completeWork();

    /**
     * @brief Creates outputs for each request associated with the current work item.
     * @return A vector of pointers to the created output components.
     */
    vector<OutputComponent*> produceOutput();

    /**
     * @brief Prints the current work item's details and status.
     *
     * Displays the work item's name, description, requests, client, and current
     * state. If no work item is available, prints a message and returns.
     */
    void printWorkItem();

    /**
     * @brief Sets the output registry used to create outputs.
     * @param outputRegistry Pointer to the output registry.
     */
    void setOutputRegistry(OutputRegistry* outputRegistry);
};

#endif
