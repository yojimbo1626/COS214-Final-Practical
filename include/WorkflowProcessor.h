#ifndef WORKFLOWPROCESSOR_H
#define WORKFLOWPROCESSOR_H

#include <string>
#include <vector>

/**
 * @class WorkflowProcessor
 * @brief Common interface shared by the real facade (SystemProcessor)
 *        and its proxy (ClientProcessor).
 *
 * Clients only know about this interface, so they never need to care
 * whether they are talking to the real thing or to the proxy.
 */
class WorkflowProcessor {
public:
    virtual ~WorkflowProcessor() {}

    /**
     * @brief Retrieves the names of all available output types from the output registry.
     * @return A vector containing the descriptions of the available outputs.
     */
    virtual std::vector<std::string> getAvailableOutputs() = 0;

    /**
     * @brief Adds a request to the current work item.
     * @param request The request text.
     * @return true if the request was added, false otherwise.
     */
    virtual bool addRequest(std::string request) = 0;

    /**
     * @brief Cancels the current work item.
     */
    virtual void cancelWork() = 0;
};

#endif