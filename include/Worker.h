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
 * @class Worker
 * @brief A participant who carries out work items.
 *
 * Pattern roles:
 * - Strategy: Context (completes work with its TaskExecutionStrategy).
 * - Observer: ConcreteObserver (receives pushed state changes of its work item).
 * - Factory Method: client of Producer (creates WorkOutputs for the registry).
 * - Memento: caretaker side (undoLastAction() restores the last WorkItemSnapshot).
 * - Proxy: creates the ClientProcessor a Client works through.
 *
 * SystemProcessor is a friend so the worker and the facade can share the active work item.
 */
class Worker : public Participant {
friend SystemProcessor;
public:
    /** @brief Creates a worker that uses RoleExecution until another strategy is set. @param name Display name. */
    explicit Worker(std::string name);
    /** @brief Deletes the worker's producer and strategy. */
    ~Worker();
    Worker(const Worker&) = delete;             ///< Not copyable: the worker owns its producer and strategy.
    Worker& operator=(const Worker&) = delete;  ///< Not assignable, for the same reason.

    /** @brief Gives the worker the facade of the work item it was assigned. @param processor Not owned. */
    void assignProcessor(SystemProcessor* processor);
    /**
     * @brief Proxy: creates restricted access to the worker's processor for a client.
     * @return A new ClientProcessor owned by the caller, or nullptr if the worker has no work item yet.
     */
    ClientProcessor* createClientProcessor();
    /**
     * @brief Hands the current work item to another worker (or re-assigns it to this one).
     * @return true if the facade accepted the assignment.
     */
    bool assignWorkItem(Worker* worker);
    /**
     * @brief Completes the current work item with the worker's strategy, then delivers the outputs to the client.
     * @return true if the strategy completed the item and the outputs were delivered; false otherwise
     *         (no work item, no client, or the strategy refused, e.g. a rule failed).
     */
    bool completeWorkItem();
    /** @brief Memento: restores the work item to the snapshot taken before the last action. */
    void undoLastAction();
    /** @brief Rejects (cancels) the current work item through the facade. */
    void rejectWorkItem();
    /** @return Name used for work items this worker creates. */
    std::string getWorkItemName() const;
    /** @return Description used for work items this worker creates. */
    std::string getWorkItemDescription() const;
    /** @param workItemName Name used for work items this worker creates. */
    void setWorkItemName(std::string workItemName);
    /** @param workItemDescription Description used for work items this worker creates. */
    void setWorkItemDescription(std::string workItemDescription);
    /** @param registry The registry this worker adds products to. Not owned. */
    void setRegistry(OutputRegistry* registry);
    /**
     * @brief Creates a product (optionally decorated with features) and registers it as a prototype.
     * @param output Product name; when client is true, the registry name of an existing product.
     * @param features Names of features to wrap round the product (Decorator). When client is true,
     *        each must already be registered as an OutputFeature prototype.
     * @param client true: a client's special request (also added to the work item);
     *        false: the worker creates a new product with its Factory Method.
     * @return true if the product was registered (and, for a client, requested).
     */
    bool addToRegistry(std::string output, std::vector<std::string> features, bool client = true);

    /** @return The current strategy (owned by the worker). */
    TaskExecutionStrategy* getTaskExecutionStrategy() const;
    /** @brief Replaces the strategy and deletes the old one. Ignores nullptr and the current strategy. */
    void setTaskExecutionStrategy(TaskExecutionStrategy* newStrategy);

    /** @brief Observer: prints the pushed update. @param update Text describing the change. */
    void receiveUpdate(std::string update);
    /** @return "Worker #id (name)". */
    std::string toString();

private:
    Producer* producer;                          ///< Factory Method creator (owned).
    OutputRegistry* registry;                    ///< Prototype registry (not owned).
    SystemProcessor* processor;                  ///< Facade of the assigned work item (not owned), or nullptr.
    TaskExecutionStrategy* taskExecutionStrategy;///< Current strategy (owned).
    std::string workItemName;                    ///< Name for new work items.
    std::string workItemDescription;             ///< Description for new work items.
};

#endif
