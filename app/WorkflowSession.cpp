/**
 * @file WorkflowSession.cpp
 * @brief Implements WorkflowSession: turns user actions into engine calls.
 */
#include "WorkflowSession.h"

#include <sstream>

#include "Client.h"
#include "OutputComponent.h"
#include "OutputFeature.h"
#include "RoleExecution.h"
#include "RuleExecution.h"
#include "SystemProcessor.h"
#include "WorkItem.h"
#include "WorkOutputFeature.h"
#include "Worker.h"
#include "WorkloadExecution.h"

WorkflowSession::WorkflowSession(const Industry& industry, const std::string& name)
    : industryData(industry), workflowName(name), logger("History"), feed("GUI notifications"),
      registryFilled(false), current(-1), browser(nullptr) {}

WorkflowSession::~WorkflowSession() {
    delete browser;
    for (size_t i = 0; i < items.size(); ++i) {
        delete items[i].client;      // also deletes the client's proxy
        delete items[i].worker;
        delete items[i].processor;   // also deletes its controller and snapshots
        delete items[i].item;
    }
    // registry, logger and feed are members and are destroyed after this.
}

// ------------------------------------------------------------------ helpers

WorkflowSession::Slot* WorkflowSession::slot() {
    return current < 0 ? nullptr : &items[static_cast<size_t>(current)];
}

const WorkflowSession::Slot* WorkflowSession::slot() const {
    return current < 0 ? nullptr : &items[static_cast<size_t>(current)];
}

ActionResult WorkflowSession::refuse(const std::string& action, const std::string& hint) {
    std::string msg = "Not allowed: cannot " + action;
    if (slot() != nullptr) msg += " while '" + slot()->item->getName() + "' is " + slot()->item->process();
    msg += ".";
    if (!hint.empty()) msg += " " + hint;
    return ActionResult(false, msg);
}

void WorkflowSession::resetBrowsing() {
    delete browser;   // the registry changed, so build a fresh iterator next time
    browser = nullptr;
}

bool WorkflowSession::isFeature(OutputComponent* component) const {
    // Lone feature prototypes are offered as extras, not as products to browse.
    return dynamic_cast<OutputFeature*>(component) != nullptr
        && component->printOutput().find(" + ") == 0;
}

void WorkflowSession::fillRegistry(Worker* worker) {
    if (registryFilled) return;
    // Factory Method + Prototype: the worker produces each product once; the registry keeps it as a prototype.
    std::vector<std::string> noFeatures;
    for (size_t i = 0; i < industryData.products.size(); ++i) {
        worker->addToRegistry(industryData.products[i], noFeatures, false);
    }
    // Decorator prototypes: the extras a client may add to a product.
    for (size_t i = 0; i < industryData.features.size(); ++i) {
        registry.registerOutput(industryData.features[i], new WorkOutputFeature(industryData.features[i]));
    }
    registryFilled = true;
}

// ------------------------------------------------------------------ work items

ActionResult WorkflowSession::createWorkItem(const std::string& name, const std::string& description,
                                             const std::string& strategy, const std::string& clientName,
                                             const std::string& workerName) {
    if (name.empty()) return ActionResult(false, "Please give the work item a name.");
    if (clientName.empty() || workerName.empty())
        return ActionResult(false, "Please enter both a client and a worker.");
    if (items.size() >= MAX_ITEMS)
        return ActionResult(false, "Not allowed: a workflow can have at most " + std::to_string(MAX_ITEMS) + " work items.");

    Slot s;
    s.started = false;
    s.processor = new SystemProcessor();             // Facade for this work item
    s.processor->setOutputRegistry(&registry);
    s.worker = new Worker(workerName + " (" + industryData.workerRole + ")");
    s.client = new Client(clientName);
    s.worker->setRegistry(&registry);
    s.worker->setWorkItemName(name);
    s.worker->setWorkItemDescription(description);
    s.worker->assignProcessor(s.processor);          // the worker can undo through this facade

    if (strategy == "rule") { s.worker->setTaskExecutionStrategy(new RuleExecution()); s.strategy = "Rule"; }
    else if (strategy == "workload") { s.worker->setTaskExecutionStrategy(new WorkloadExecution()); s.strategy = "Workload"; }
    else { s.worker->setTaskExecutionStrategy(new RoleExecution()); s.strategy = "Role"; }

    fillRegistry(s.worker);

    s.item = new WorkItem(name, description);
    s.item->addObserver(&logger);   // Observer: readable history
    s.item->addObserver(&feed);     // Observer: the GUI's notifications
    s.item->addObserver(s.client);  // Observer: the client hears about every change

    items.push_back(s);
    current = static_cast<int>(items.size()) - 1;
    return ActionResult(true, "Created '" + name + "' for " + clientName + " (work item "
                              + std::to_string(items.size()) + "). Press START to begin.");
}

ActionResult WorkflowSession::selectItem(int index) {
    if (index < 0 || index >= static_cast<int>(items.size())) return ActionResult(false, "No such work item.");
    current = index;
    return ActionResult(true, "Now showing '" + items[static_cast<size_t>(index)].item->getName() + "'.");
}

// ------------------------------------------------------------------ actions

ActionResult WorkflowSession::startWorkflow() {
    Slot* s = slot();
    if (s == nullptr) return ActionResult(false, "Create a work item first.");
    if (s->started) return refuse("start the workflow again", "It has already started.");
    if (s->item->process() == "Blocked") return refuse("start", "Unblock it first.");

    if (!s->processor->startWorkflow(s->item)) return refuse("start", "");   // Facade -> Command -> State
    s->started = true;
    return ActionResult(true, "Workflow started: '" + s->item->getName() + "' is now In-progress.");
}

ActionResult WorkflowSession::assign() {
    Slot* s = slot();
    if (s == nullptr) return ActionResult(false, "Create a work item first.");
    if (!s->started) return refuse("assign a worker", "Start the workflow first.");

    if (!s->processor->assignWorkItem(s->worker))                 // Facade -> Command -> State; worker observes
        return refuse("assign a worker", "A worker can only be assigned while the item is In-progress.");
    s->client->requestProcessor(s->worker);                       // Proxy: the client gets a ClientProcessor
    return ActionResult(true, "Assigned to " + s->worker->getName() + ". The client can now add requests.");
}

ActionResult WorkflowSession::addRequest(const std::string& output, const std::string& feature) {
    Slot* s = slot();
    if (s == nullptr) return ActionResult(false, "Create a work item first.");
    if (s->item->process() != "Assigned")
        return refuse("add a request", "The client sends requests through its proxy once a worker is assigned.");
    if (s->item->getRequests().size() >= MAX_REQUESTS)
        return ActionResult(false, "Not allowed: a work item can have at most " + std::to_string(MAX_REQUESTS) + " requests.");
    if (output.empty()) return ActionResult(false, "Choose an output first (press FIRST / NEXT).");

    std::string broken = industryRuleViolation(industryData, output, feature, s->item->getRequests());
    if (!broken.empty()) return ActionResult(false, "Not allowed by the " + industryData.title + " rule: " + broken);

    bool ok;
    if (feature.empty()) {
        ok = s->client->addRequest(output);                              // Proxy -> Facade -> Command
    } else {
        std::vector<std::string> features(1, feature);
        ok = s->client->addSpecialRequest(s->worker, output, features);  // Decorator + Prototype + Proxy
        resetBrowsing();                                                  // registry gained a decorated product
    }
    if (!ok) return ActionResult(false, "Not allowed: '" + output + "' is not an output in the registry.");
    return ActionResult(true, "Request added: " + s->item->getRequests().back());
}

ActionResult WorkflowSession::complete() {
    Slot* s = slot();
    if (s == nullptr) return ActionResult(false, "Create a work item first.");
    if (s->item->process() != "Assigned") return refuse("complete", "Only an Assigned item can be completed.");
    if (s->item->getRequests().empty()) return ActionResult(false, "Add at least one request before completing.");

    std::vector<std::string> requested = s->item->getRequests();
    if (!s->worker->completeWorkItem()) {   // Strategy, then Factory Method/Prototype outputs, then Facade
        return ActionResult(false, "The " + s->strategy + " strategy did not complete '" + s->item->getName()
                                   + "' (it is still " + s->item->process() + "). Check the rule, or choose another strategy.");
    }
    s->delivered = requested;   // one output was cloned from the registry for each request
    return ActionResult(true, "Completed by the " + s->strategy + " strategy. "
                              + std::to_string(s->delivered.size()) + " output(s) delivered to the client.");
}

ActionResult WorkflowSession::cancel() {
    Slot* s = slot();
    if (s == nullptr) return ActionResult(false, "Create a work item first.");
    std::string st = s->item->process();
    if (st == "Completed" || st == "Rejected") return refuse("cancel", "Completed and cancelled items are final.");

    if (st == "Assigned") s->client->cancelWork();        // Proxy -> Facade -> Command
    else if (s->started) s->processor->cancelWork();      // Facade -> Command
    else s->item->cancel();                               // State: the facade only takes an item once it starts

    bool ok = s->item->process() == "Rejected";
    return ActionResult(ok, ok ? "Cancelled: '" + s->item->getName() + "' was rejected." : "The cancel was refused.");
}

ActionResult WorkflowSession::toggleBlock() {
    Slot* s = slot();
    if (s == nullptr) return ActionResult(false, "Create a work item first.");
    std::string before = s->item->process();
    if (!s->item->blocked())                              // State: Available <-> Blocked
        return refuse("block", "Only an Available item can be blocked (for example: waiting for stock or a free chair).");
    if (before == "Blocked")
        return ActionResult(true, "Unblocked: '" + s->item->getName() + "' is Available again.");
    return ActionResult(true, "Blocked: '" + s->item->getName() + "' waits until it is unblocked.");
}

ActionResult WorkflowSession::undo() {
    Slot* s = slot();
    if (s == nullptr) return ActionResult(false, "Create a work item first.");
    std::string st = s->item->process();
    if (st == "Completed" || st == "Rejected") return refuse("undo", "Finished items cannot be undone.");

    size_t requestsBefore = s->item->getRequests().size();
    s->worker->undoLastAction();                           // Memento
    std::string now = s->item->process();
    if (now == st && s->item->getRequests().size() == requestsBefore) return ActionResult(false, "Nothing to undo.");
    if (now == "Available") s->started = false;            // undoing the start
    return ActionResult(true, "Undone: '" + s->item->getName() + "' is back to " + now + " with "
                              + std::to_string(s->item->getRequests().size()) + " request(s).");
}

// ------------------------------------------------------------------ iterator

ActionResult WorkflowSession::browseFirst(std::string& out) {
    resetBrowsing();
    browser = registry.createIterator();
    OutputComponent* c = browser->first();
    size_t guard = 0;
    while (c != nullptr && isFeature(c) && guard++ < 1000) c = browser->next();
    if (c == nullptr || isFeature(c)) return ActionResult(false, "The registry is empty. Create a work item first.");
    out = c->printOutput();
    return ActionResult(true, out);
}

ActionResult WorkflowSession::browseNext(std::string& out) {
    if (browser == nullptr) return browseFirst(out);
    OutputComponent* c = browser->next();   // wraps to the first after the last
    size_t guard = 0;
    while (c != nullptr && isFeature(c) && guard++ < 1000) c = browser->next();
    if (c == nullptr || isFeature(c)) return ActionResult(false, "The registry is empty.");
    out = c->printOutput();
    return ActionResult(true, out);
}

// ------------------------------------------------------------------ view

std::string WorkflowSession::itemName(size_t index) const {
    return index < items.size() ? items[index].item->getName() : "";
}
std::string WorkflowSession::itemState(size_t index) const {
    return index < items.size() ? items[index].item->process() : "";
}
std::string WorkflowSession::itemName() const { return slot() ? slot()->item->getName() : ""; }
std::string WorkflowSession::itemDescription() const { return slot() ? slot()->item->getDescription() : ""; }
std::string WorkflowSession::clientName() const { return slot() ? slot()->client->getName() : ""; }
std::string WorkflowSession::workerName() const { return slot() ? slot()->worker->getName() : ""; }
std::string WorkflowSession::strategyName() const { return slot() ? slot()->strategy : ""; }
std::string WorkflowSession::state() const { return slot() ? slot()->item->process() : ""; }

std::string WorkflowSession::blockedFrom() const {
    // In the engine only an Available item can be blocked, and it is released back to Available.
    return state() == "Blocked" ? "Available" : "";
}

std::vector<std::string> WorkflowSession::requests() const {
    return slot() ? slot()->item->getRequests() : std::vector<std::string>();
}

std::vector<std::string> WorkflowSession::outputs() const {
    return slot() ? slot()->delivered : std::vector<std::string>();
}

std::string WorkflowSession::report() {
    std::ostringstream r;
    r << "TIVIDY report\n=============\n";
    r << "Industry:   " << industryData.title << "\n";
    r << "Workflow:   " << workflowName << "\n";
    if (!industryData.ruleText.empty()) r << "Rule:       " << industryData.ruleText << "\n";
    r << "Work items: " << items.size() << "\n";
    if (items.empty()) {
        r << "\nNo work item has been created yet.\n";
        return r.str();
    }
    for (size_t n = 0; n < items.size(); ++n) {
        Slot& s = items[n];
        r << "\n--- Work item " << (n + 1) << ": " << s.item->getName() << " ---\n";
        r << "About:     " << s.item->getDescription() << "\n";
        r << "Client:    " << s.client->toString() << "\n";
        r << "Worker:    " << s.worker->toString() << "\n";
        r << "Strategy:  " << s.strategy << "Execution\n";
        r << "State:     " << s.item->process() << "\n";
        std::vector<std::string> reqs = s.item->getRequests();
        r << "Requests (" << reqs.size() << "/" << MAX_REQUESTS << "):\n";
        if (reqs.empty()) r << "  none\n";
        for (size_t i = 0; i < reqs.size(); ++i) r << "  - " << reqs[i] << "\n";
        r << "Outputs delivered:\n";
        if (s.delivered.empty()) r << "  none yet\n";
        for (size_t i = 0; i < s.delivered.size(); ++i) r << "  - " << s.delivered[i] << "\n";
    }

    r << "\nHistory (" << logger.toString() << "):\n";
    std::vector<std::string> log = logger.getLog();
    if (log.empty()) r << "  nothing yet\n";
    for (size_t i = 0; i < log.size(); ++i) r << "  " << (i + 1) << ". " << log[i] << "\n";
    return r.str();
}
