/**
 * @file WorkflowSession.h
 * @brief One running workflow instance, as used by the GUI.
 *
 * WorkflowSession is application code, not part of the engine. It owns the engine objects for
 * one workflow instance (registry, facade, worker, client, history logger and work item) and
 * turns each user action into calls on the engine:
 * - Facade:   SystemProcessor runs start / assign / complete / cancel.
 * - Command:  SystemProcessor forwards each action to a Command via the WorkflowController.
 * - State:    the WorkItem's state decides whether each action is allowed.
 * - Proxy:    the client adds requests and cancels through its ClientProcessor.
 * - Observer: the client, the worker and a HistoryLogger are told about every state change.
 * - Strategy: the worker completes the task with the chosen TaskExecutionStrategy.
 * - Memento:  undo restores the last WorkItemSnapshot.
 * - Iterator: the user browses the registry's outputs with first() / next().
 * - Factory Method, Prototype, Decorator: the worker creates the products, the registry
 *   clones them, and extras decorate them.
 *
 * Sessions are independent: each has its own engine objects, so two workflow instances
 * never affect each other.
 */
#ifndef WORKFLOW_SESSION_H
#define WORKFLOW_SESSION_H

#include <string>
#include <vector>

#include "IndustryCatalog.h"
#include "OutputRegistry.h"
#include "SystemProcessor.h"
#include "HistoryLogger.h"
#include "Iterator.h"

class Worker;
class Client;
class WorkItem;
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
     * @brief Creates the work item, its client and its worker, and fills the registry.
     * @param name Work item name. @param description What the work item is.
     * @param strategy "role", "rule" or "workload". @param client Client name. @param worker Worker name.
     */
    ActionResult createWorkItem(const std::string& name, const std::string& description,
                                const std::string& strategy, const std::string& client,
                                const std::string& worker);

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

    /** @brief The work item report shown on the Report page. */
    std::string report();

    // ---- read-only view of the instance, for the GUI ----
    const Industry& industry() const { return industryData; }
    std::string name() const { return workflowName; }
    bool hasWorkItem() const { return workItem != nullptr; }
    std::string itemName() const;
    std::string itemDescription() const;
    std::string clientName() const;
    std::string workerName() const;
    std::string strategyName() const { return strategyLabel; }
    std::string state() const;
    std::vector<std::string> requests() const;
    std::vector<std::string> notifications() const { return notificationLog; }
    std::vector<std::string> history() const { return logger.getLog(); }
    std::vector<std::string> outputs() const { return producedOutputs; }
    std::vector<std::string> features() const { return industryData.features; }

private:
    ActionResult refuse(const std::string& action, const std::string& hint) const;
    void collectNotifications(const std::string& printed);
    void resetBrowsing();
    bool isFeature(OutputComponent* component) const;

    Industry industryData;
    std::string workflowName;
    std::string strategyLabel;

    OutputRegistry registry;           ///< Prototype manager + Iterator aggregate (owned).
    SystemProcessor processor;         ///< Facade (owned).
    HistoryLogger logger;              ///< Observer that records every state change.

    Worker* worker;                    ///< Owned.
    Client* client;                    ///< Owned.
    WorkItem* workItem;                ///< Owned.
    Iterator<OutputComponent>* browser;///< Current registry iterator (owned), or nullptr.
    bool started;                      ///< true once startWorkflow() succeeded.

    std::vector<std::string> notificationLog;
    std::vector<std::string> producedOutputs;
};

#endif
