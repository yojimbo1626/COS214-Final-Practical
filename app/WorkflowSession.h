/**
 * @file WorkflowSession.h
 * @brief One running workflow instance, with any number of work items, as used by the GUI.
 *
 * WorkflowSession is application code, not part of the engine. It owns the engine objects for
 * one workflow instance and turns each user action into calls on the engine:
 * - Facade:   each work item's SystemProcessor runs start / assign / complete / cancel.
 * - Command:  SystemProcessor forwards each action to a Command via the WorkflowController.
 * - State:    the WorkItem's state decides whether each action is allowed.
 * - Proxy:    the client adds requests and cancels through its ClientProcessor.
 * - Observer: the client, the worker, a HistoryLogger and a SessionObserver are told about
 *             every state change.
 * - Strategy: the worker completes the task with the chosen TaskExecutionStrategy.
 * - Memento:  undo restores the last WorkItemSnapshot.
 * - Iterator: the user browses the registry's outputs with first() / next().
 * - Factory Method, Prototype, Decorator: the worker creates the products, the registry
 *   clones them, and extras decorate them.
 *
 * The products registry, the history and the notifications are shared by the work items of one
 * workflow. Each work item has its own client, worker, strategy and facade.
 */
#ifndef WORKFLOW_SESSION_H
#define WORKFLOW_SESSION_H

#include <string>
#include <vector>

#include "IndustryCatalog.h"
#include "OutputRegistry.h"
#include "HistoryLogger.h"
#include "SessionObserver.h"
#include "Iterator.h"

class Worker;
class Client;
class WorkItem;
class SystemProcessor;
class OutputComponent;

/**
 * @struct ActionResult
 * @brief Outcome of one user action.
 */
struct ActionResult {
    bool ok;               ///< true if the engine accepted the action.
    std::string message;   ///< What happened, or why it was refused.

    ActionResult(bool ok, const std::string& message) : ok(ok), message(message) {}
};

/**
 * @class WorkflowSession
 * @brief One independent workflow instance of one industry.
 */
class WorkflowSession {
public:
    /** Maximum number of requests per work item (keeps the GUI tidy). */
    static const size_t MAX_REQUESTS = 5;
    /** Maximum number of work items per workflow. */
    static const size_t MAX_ITEMS = 10;

    /**
     * @brief Creates an empty workflow instance for an industry.
     * @param industry The industry this workflow belongs to.
     * @param name The user's name for this workflow.
     */
    WorkflowSession(const Industry& industry, const std::string& name);

    /** @brief Deletes the engine objects this session owns. */
    ~WorkflowSession();

    WorkflowSession(const WorkflowSession&) = delete;
    WorkflowSession& operator=(const WorkflowSession&) = delete;

    /**
     * @brief Adds a work item, with its own client and worker, and makes it the current one.
     * @param name Work item name. @param description What the work item is.
     * @param strategy "role", "rule" or "workload". @param client Client name. @param worker Worker name.
     */
    ActionResult createWorkItem(const std::string& name, const std::string& description,
                                const std::string& strategy, const std::string& client,
                                const std::string& worker);

    /** @brief Makes another work item the current one. @param index Position in the item list. */
    ActionResult selectItem(int index);

    // ---- actions on the current work item ----
    ActionResult startWorkflow();   ///< Facade + Command: Available -> InProgress.
    ActionResult assign();          ///< Facade + Command: InProgress -> Assigned; gives the client its proxy.
    ActionResult complete();        ///< Strategy + Facade: completes the item and produces the outputs.
    ActionResult cancel();          ///< Proxy / Facade: cancels the item (-> Rejected).
    ActionResult toggleBlock();     ///< State: Available <-> Blocked.
    ActionResult undo();            ///< Memento: restores the last snapshot.

    /**
     * @brief Client request through the proxy, optionally decorated with one extra feature.
     * @param output An output name exactly as the iterator showed it.
     * @param feature An extra from features(), or empty for none.
     */
    ActionResult addRequest(const std::string& output, const std::string& feature);

    /** @brief Iterator: moves to the first product. @param out Receives its name. */
    ActionResult browseFirst(std::string& out);
    /** @brief Iterator: moves to the next product, wrapping round. @param out Receives its name. */
    ActionResult browseNext(std::string& out);

    /** @brief The workflow report shown on the Report page (every work item). */
    std::string report();

    // ---- read-only view, for the GUI ----
    const Industry& industry() const { return industryData; }
    std::string name() const { return workflowName; }
    bool hasWorkItem() const { return current >= 0; }
    int currentItem() const { return current; }
    size_t itemCount() const { return items.size(); }
    std::string itemName(size_t index) const;    ///< Name of item index.
    std::string itemState(size_t index) const;   ///< State of item index.
    std::string itemName() const;                ///< Name of the current item.
    std::string itemDescription() const;
    std::string clientName() const;
    std::string workerName() const;
    std::string strategyName() const;
    std::string state() const;
    std::string blockedFrom() const;   ///< "Available" while Blocked, else "".
    std::vector<std::string> requests() const;
    std::vector<std::string> outputs() const;
    std::vector<std::string> notifications() const { return feed.getNotifications(); }
    std::vector<std::string> history() const { return logger.getLog(); }
    std::vector<std::string> features() const { return industryData.features; }

private:
    /** @brief Everything one work item needs. All pointers are owned. */
    struct Slot {
        WorkItem* item;
        Client* client;
        Worker* worker;
        SystemProcessor* processor;
        std::string strategy;                 ///< "Role", "Rule" or "Workload".
        bool started;                         ///< true once startWorkflow() succeeded.
        std::vector<std::string> delivered;   ///< Outputs delivered when it completed.
    };

    Slot* slot();
    const Slot* slot() const;
    ActionResult refuse(const std::string& action, const std::string& hint);
    void resetBrowsing();
    bool isFeature(OutputComponent* component) const;
    void fillRegistry(Worker* worker);

    Industry industryData;
    std::string workflowName;

    OutputRegistry registry;           ///< Prototype manager + Iterator aggregate, shared by all items.
    HistoryLogger logger;              ///< Observer: readable history of every state change.
    SessionObserver feed;              ///< Observer: the GUI's notifications.
    bool registryFilled;

    std::vector<Slot> items;
    int current;                       ///< Index of the current work item, or -1.
    Iterator<OutputComponent>* browser;///< Current registry iterator (owned), or nullptr.
};

#endif
