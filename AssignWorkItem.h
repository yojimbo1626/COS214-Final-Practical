#ifndef ASSIGNWORKITEM_H
#define ASSIGNWORKITEM_H

#include "Command.h"

class AssignWorkItem : public Command {
public:
    explicit AssignWorkItem(WorkItem* workItem);

    bool execute() override;
};

#endif
