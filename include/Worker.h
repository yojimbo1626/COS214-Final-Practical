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
    explicit Worker(const std::string& name);
    ~Worker();

    TaskExecutionStrategy* getTaskExecutionStrategy() const;
    void setTaskExecutionStrategy(TaskExecutionStrategy* taskExecutionStrategy); // deletes the old one

    void receiveUpdate(const std::string& update);
    std::string toString();

private:

    TaskExecutionStrategy* taskExecutionStrategy;
};

#endif
