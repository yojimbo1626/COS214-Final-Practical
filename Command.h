#ifndef COMMAND_H
#define COMMAND_H

class WorkItem;  

class Command {
protected:
    WorkItem* workItem;  

public:
    explicit Command(WorkItem* workItem);
    virtual ~Command();

    virtual bool execute() = 0;
};

#endif
