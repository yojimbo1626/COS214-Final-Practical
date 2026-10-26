#ifndef WORKER_H
#define WORKER_H

#include <string>

class TaskExecutionStrategy;

/**
 * @brief Context in the Strategy pattern.
 *
 * Owns its strategy (composition, multiplicity 1): deletes the old one
 * when a new one is set, and deletes the current one on destruction.
 * Only strategy-related members are stubbed here.
 *
 * TODO: inherit from Participant (": public Participant") once merged.
 */
class Worker {
public:
    explicit Worker(const std::string& name);
    ~Worker();

    TaskExecutionStrategy* getTaskExecutionStrategy() const;
    void setTaskExecutionStrategy(TaskExecutionStrategy* taskExecutionStrategy);

private:
    Worker(const Worker&);            
    Worker& operator=(const Worker&); 

    TaskExecutionStrategy* taskExecutionStrategy;
};

#endif
