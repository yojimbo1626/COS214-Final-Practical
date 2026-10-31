#ifndef WORKER_H
#define WORKER_H

#include "Participant.h"

class TaskExecutionStrategy;

/**
 * @brief Context in the Strategy pattern and a ConcreteObserver in the
 *        Observer pattern.
 */
class Worker : public Participant {
public:
    explicit Worker(std::string name);
    ~Worker();

    TaskExecutionStrategy* getTaskExecutionStrategy() const;
    void setTaskExecutionStrategy(TaskExecutionStrategy* newStrategy); // deletes the old one

    void receiveUpdate(std::string update);
    std::string toString();

private:

    TaskExecutionStrategy* taskExecutionStrategy;
};

#endif
