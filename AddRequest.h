#ifndef ADDREQUEST_H
#define ADDREQUEST_H

#include <string>
#include "Command.h"

class AddRequest : public Command {
private:
    std::string request;

public:
    AddRequest(WorkItem* workItem, const std::string& request);

    bool execute() override;
    void defineRequest(const std::string& request);
};

#endif
