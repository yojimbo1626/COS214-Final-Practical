#ifndef CANCELWORKITEM_H
#define CANCELWORKITEM_H

#include "Command.h"

class CancelWorkItem : public Command {
public:
    explicit CancelWorkItem(WorkItem* workItem);

    bool execute() override;
};

#endif
