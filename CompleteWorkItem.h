#ifndef COMPLETEWORKITEM_H
#define COMPLETEWORKITEM_H

#include "Command.h"

class CompleteWorkItem : public Command {
public:
    explicit CompleteWorkItem(WorkItem* workItem);

    bool execute() override;
};

#endif
