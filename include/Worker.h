#ifndef WORKER_H
#define WORKER_H

#include "Participant.h"

#include <vector>

class Producer;
class OutputRegistry;
class SystemProcessor;
class ClientProcessor;
class TaskExecutionStrategy;

/**
 * @brief Context in the Strategy pattern and a ConcreteObserver in the
 *        Observer pattern.
 */
class Worker : public Participant {
friend SystemProcessor;
public:
    explicit Worker(std::string name);
    ~Worker();
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    void assignProcessor(SystemProcessor* processor);
    ClientProcessor* createClientProcessor();
    bool assignWorkItem(Worker* worker);
    bool completeWorkItem();
    void undoLastAction();
    void rejectWorkItem();
    std::string getWorkItemName() const;
    std::string getWorkItemDescription() const;
    void setWorkItemDescription(std::string workItemDescription);
    void setRegistry(OutputRegistry* registry);
    bool addToRegistry(std::string output, std::vector<std::string> features, bool client = true);

    TaskExecutionStrategy* getTaskExecutionStrategy() const;
    void setTaskExecutionStrategy(TaskExecutionStrategy* newStrategy); // deletes the old one

    void receiveUpdate(std::string update);
    std::string toString();

private:
    Producer* producer;
    OutputRegistry* registry;
    SystemProcessor* processor;
    TaskExecutionStrategy* taskExecutionStrategy;
    std::string workItemName;
    std::string workItemDescription;
};

#endif
