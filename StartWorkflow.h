#ifndef STARTWORKFLOW_H
#define STARTWORKFLOW_H

#include "Command.h"

class StartWorkflow : public Command {
public:
    explicit StartWorkflow(WorkItem* workItem);

    bool execute() override;
};

#endif
