/**
 * @file WorkflowSession.cpp
 * @brief Implements WorkflowSession: turns user actions into engine calls.
 */
#include "WorkflowSession.h"

#include <iostream>
#include <sstream>

#include "Client.h"
#include "Worker.h"
#include "WorkItem.h"
#include "OutputComponent.h"
#include "OutputFeature.h"
#include "WorkOutputFeature.h"
#include "RoleExecution.h"
#include "RuleExecution.h"
#include "WorkloadExecution.h"

namespace {

/**
 * @brief Captures everything written to std::cout while it exists.
 *
 * The engine's observers (Client, Worker) print their updates. The GUI shows those lines
 * as notifications, so we capture them instead of losing them in the server's terminal.
 */
class CoutCapture {
public:
    CoutCapture() : old(std::cout.rdbuf(buffer.rdbuf())) {}
    ~CoutCapture() { std::cout.rdbuf(old); }
    std::string text() const { return buffer.str(); }
private:
    std::ostringstream buffer;
    std::streambuf* old;
};

std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

}  // namespace

WorkflowSession::WorkflowSession(const Industry& industry, const std::string& name)
    : industryData(industry), workflowName(name), strategyLabel("Role"),
      logger("History"), worker(nullptr), client(nullptr), workItem(nullptr),
      browser(nullptr), started(false) {
    processor.setOutputRegistry(&registry);
}

WorkflowSession::~WorkflowSession() {
    delete browser;
    delete client;     // also deletes the client's proxy
    delete worker;
    delete workItem;
    // processor, registry and logger are members and are destroyed after this.
}

// ------------------------------------------------------------------ helpers

ActionResult WorkflowSession::refuse(const std::string& action, const std::string& hint) const {
    std::string msg = "Not allowed: cannot " + action;
    if (workItem != nullptr) msg += " while '" + workItem->getName() + "' is " + workItem->process();
    msg += ".";
    if (!hint.empty()) msg += " " + hint;
    return ActionResult(false, msg);
}

void WorkflowSession::collectNotifications(const std::string& printed) {
    std::vector<std::string> lines = splitLines(printed);
    for (size_t i = 0; i < lines.size(); ++i) {
        // Observer updates look like "[Client #2 (Thandi)] received update: ...".
        if (lines[i].find("received update:") != std::string::npos) {
            notificationLog.push_back(lines[i]);
        }
    }
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

// ------------------------------------------------------------------ actions

ActionResult WorkflowSession::createWorkItem(const std::string& name, const std::string& description,
                                             const std::string& strategy, const std::string& clientName,
                                             const std::string& workerName) {
    if (name.empty()) return ActionResult(false, "Please give the work item a name.");
    if (clientName.empty() || workerName.empty())
        return ActionResult(false, "Please enter both a client and a worker.");
    if (started) return ActionResult(false, "This workflow has already started. Create a new workflow for a new work item.");

    // Replacing a work item that was never started is allowed (the user went back and changed it).
    resetBrowsing();
    delete client; client = nullptr;
    delete worker; worker = nullptr;
    delete workItem; workItem = nullptr;
    notificationLog.clear();
    producedOutputs.clear();

    worker = new Worker(workerName + " (" + industryData.workerRole + ")");
    client = new Client(clientName);
    worker->setRegistry(&registry);
    worker->setWorkItemName(name);
    worker->setWorkItemDescription(description);

    if (strategy == "rule") { worker->setTaskExecutionStrategy(new RuleExecution()); strategyLabel = "Rule"; }
    else if (strategy == "workload") { worker->setTaskExecutionStrategy(new WorkloadExecution()); strategyLabel = "Workload"; }
    else { worker->setTaskExecutionStrategy(new RoleExecution()); strategyLabel = "Role"; }

    // Factory Method + Prototype: the worker produces each product once; the registry keeps it as a prototype.
    // (Only the first time: if the user went back and re-created the item, the registry is already filled.)
    if (processor.getAvailableOutputs().empty()) {
        CoutCapture quiet;
        std::vector<std::string> noFeatures;
        for (size_t i = 0; i < industryData.products.size(); ++i) {
            worker->addToRegistry(industryData.products[i], noFeatures, false);
        }
        // Decorator prototypes: the extras a client may add to a product.
        for (size_t i = 0; i < industryData.features.size(); ++i) {
            registry.registerOutput(industryData.features[i], new WorkOutputFeature(industryData.features[i]));
        }
    }

    workItem = new WorkItem(name, description);
    workItem->addObserver(&logger);   // Observer: audit history
    workItem->addObserver(client);    // Observer: the client hears about every change

    return ActionResult(true, "Created '" + name + "' for " + clientName + ". Press START to begin the workflow.");
}

ActionResult WorkflowSession::startWorkflow() {
    if (workItem == nullptr) return ActionResult(false, "Create a work item first.");
    if (started) return refuse("start the workflow again", "It has already started.");
    if (workItem->process() == "Blocked") return refuse("start", "Unblock it first.");

    CoutCapture capture;
    bool ok = processor.startWorkflow(workItem);   // Facade -> Command -> State
    collectNotifications(capture.text());
    if (!ok) return refuse("start", "");
    started = true;
    return ActionResult(true, "Workflow started: '" + workItem->getName() + "' is now In-progress.");
}

ActionResult WorkflowSession::assign() {
    if (workItem == nullptr) return ActionResult(false, "Create a work item first.");
    if (!started) return refuse("assign a worker", "Start the workflow first.");

    CoutCapture capture;
    bool ok = processor.assignWorkItem(worker);    // Facade -> Command -> State; worker joins as observer
    if (ok) client->requestProcessor(worker);      // Proxy: the client gets a ClientProcessor
    collectNotifications(capture.text());
    if (!ok) return refuse("assign a worker", "A worker can only be assigned while the item is In-progress.");
    return ActionResult(true, "Assigned to " + worker->getName() + ". The client can now add requests.");
}

ActionResult WorkflowSession::addRequest(const std::string& output, const std::string& feature) {
    if (workItem == nullptr) return ActionResult(false, "Create a work item first.");
    if (workItem->process() != "Assigned")
        return refuse("add a request", "The client sends requests through its proxy once a worker is assigned.");
    if (workItem->getRequests().size() >= MAX_REQUESTS)
        return ActionResult(false, "Not allowed: a work item can have at most 5 requests.");
    if (output.empty()) return ActionResult(false, "Choose an output first (press FIRST / NEXT).");

    CoutCapture capture;
    bool ok;
    if (feature.empty()) {
        ok = client->addRequest(output);                              // Proxy -> Facade -> Command
    } else {
        std::vector<std::string> features(1, feature);
        ok = client->addSpecialRequest(worker, output, features);     // Decorator + Prototype + Proxy
        resetBrowsing();                                               // registry gained a decorated product
    }
    collectNotifications(capture.text());
    if (!ok) return ActionResult(false, "Not allowed: '" + output + "' is not an output in the registry.");
    return ActionResult(true, "Request added: " + workItem->getRequests().back());
}

ActionResult WorkflowSession::complete() {
    if (workItem == nullptr) return ActionResult(false, "Create a work item first.");
    if (workItem->process() != "Assigned") return refuse("complete", "Only an Assigned item can be completed.");
    if (workItem->getRequests().empty()) return ActionResult(false, "Add at least one request before completing.");

    CoutCapture capture;
    bool ok = worker->completeWorkItem();   // Strategy, then Factory Method/Prototype outputs, then Facade
    std::string printed = capture.text();
    collectNotifications(printed);

    if (!ok) {
        return ActionResult(false, "The " + strategyLabel + " strategy did not complete '" + workItem->getName()
                                   + "' (it is still " + workItem->process() + "). Check the rule, or choose another strategy.");
    }
    std::vector<std::string> lines = splitLines(printed);
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find("  - ") == 0) producedOutputs.push_back(lines[i].substr(4));
    }
    return ActionResult(true, "Completed by the " + strategyLabel + " strategy. "
                              + std::to_string(producedOutputs.size()) + " output(s) delivered to the client.");
}

ActionResult WorkflowSession::cancel() {
    if (workItem == nullptr) return ActionResult(false, "Create a work item first.");
    std::string s = workItem->process();
    if (s == "Completed" || s == "Rejected") return refuse("cancel", "Completed and cancelled items are final.");

    CoutCapture capture;
    if (s == "Assigned") client->cancelWork();          // Proxy -> Facade -> Command
    else if (started) processor.cancelWork();           // Facade -> Command
    else workItem->cancel();                            // State: Available / Blocked -> Rejected
    collectNotifications(capture.text());
    return ActionResult(workItem->process() == "Rejected",
                        workItem->process() == "Rejected" ? "Cancelled: '" + workItem->getName() + "' was rejected."
                                                          : "The cancel was refused.");
}

ActionResult WorkflowSession::toggleBlock() {
    if (workItem == nullptr) return ActionResult(false, "Create a work item first.");
    std::string before = workItem->process();
    CoutCapture capture;
    bool ok = workItem->blocked();                      // State: Available <-> Blocked
    collectNotifications(capture.text());
    if (!ok) return refuse("block", "Only an Available item can be blocked (for example: waiting for stock or a free chair).");
    return ActionResult(true, before == "Blocked" ? "Unblocked: the item is Available again." : "Blocked: the item waits until it is unblocked.");
}

ActionResult WorkflowSession::undo() {
    if (workItem == nullptr) return ActionResult(false, "Create a work item first.");
    std::string s = workItem->process();
    if (s == "Completed" || s == "Rejected") return refuse("undo", "Finished items cannot be undone.");
    if (s != "Assigned") return ActionResult(false, "Nothing to undo yet: the worker can undo once the item is assigned.");

    size_t requestsBefore = workItem->getRequests().size();
    CoutCapture capture;
    worker->undoLastAction();                            // Memento
    collectNotifications(capture.text());
    if (workItem->process() == s && workItem->getRequests().size() == requestsBefore)
        return ActionResult(false, "Nothing to undo.");
    return ActionResult(true, "Undone: the work item is back to " + workItem->process() + " with "
                              + std::to_string(workItem->getRequests().size()) + " request(s).");
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

std::string WorkflowSession::itemName() const { return workItem ? workItem->getName() : ""; }
std::string WorkflowSession::itemDescription() const { return workItem ? workItem->getDescription() : ""; }
std::string WorkflowSession::clientName() const { return client ? client->getName() : ""; }
std::string WorkflowSession::workerName() const { return worker ? worker->getName() : ""; }
std::string WorkflowSession::state() const { return workItem ? workItem->process() : ""; }
std::vector<std::string> WorkflowSession::requests() const {
    return workItem ? workItem->getRequests() : std::vector<std::string>();
}

std::string WorkflowSession::report() {
    std::ostringstream r;
    r << "TIVIDY report\n=============\n";
    r << "Industry:  " << industryData.title << "\n";
    r << "Workflow:  " << workflowName << "\n\n";
    if (workItem == nullptr) {
        r << "No work item has been created yet.\n";
        return r.str();
    }
    r << "Work item: " << workItem->getName() << "\n";
    r << "About:     " << workItem->getDescription() << "\n";
    r << "Client:    " << (client ? client->toString() : "-") << "\n";
    r << "Worker:    " << (worker ? worker->toString() : "-") << "\n";
    r << "Strategy:  " << strategyLabel << "Execution\n";
    r << "State:     " << workItem->process() << "\n\n";

    r << "Requests (" << workItem->getRequests().size() << "/" << MAX_REQUESTS << "):\n";
    std::vector<std::string> reqs = workItem->getRequests();
    if (reqs.empty()) r << "  none\n";
    for (size_t i = 0; i < reqs.size(); ++i) r << "  - " << reqs[i] << "\n";

    r << "\nOutputs delivered:\n";
    if (producedOutputs.empty()) r << "  none yet\n";
    for (size_t i = 0; i < producedOutputs.size(); ++i) r << "  - " << producedOutputs[i] << "\n";

    r << "\nHistory (" << logger.toString() << "):\n";
    std::vector<std::string> log = logger.getLog();
    if (log.empty()) r << "  nothing yet\n";
    for (size_t i = 0; i < log.size(); ++i) r << "  " << (i + 1) << ". " << log[i] << "\n";
    return r.str();
}
