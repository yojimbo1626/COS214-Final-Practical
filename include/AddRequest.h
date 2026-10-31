#ifndef ADDREQUEST_H
#define ADDREQUEST_H

#include <string>
#include "Command.h"

/**
 * @class AddRequest
 * @brief Concrete command that adds a request to a work item.
 */
class AddRequest : public Command {
private:
    std::string request; 

public:
    /**
     * @brief Creates an AddRequest command.
     * @param workItem The work item to add the request to.
     * @param request The request text.
     */
    AddRequest(WorkItem* workItem, const std::string& request);

    /**
     * @brief Adds the stored request to the work item.
     * @return true if the request was added, false otherwise.
     */
    bool execute() override;

    /**
     * @brief Changes the request text this command will add.
     * @param request The new request text.
     */
    void defineRequest(const std::string& request);
};

#endif
