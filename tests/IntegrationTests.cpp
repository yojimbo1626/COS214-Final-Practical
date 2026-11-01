/**
 * @file IntegrationTests.cpp
 * @brief End-to-end tests: several patterns working together through the real classes.
 *
 * These follow the same path as the GUI: the worker fills the registry (Factory Method,
 * Decorator, Prototype), the facade runs the workflow (Facade, Command, State), the client
 * works through its proxy (Proxy), observers are told about every change (Observer),
 * the worker's strategy does the task (Strategy) and changes can be undone (Memento).
 */
#include <string>
#include <vector>

#include "doctest/doctest.h"

#include "Client.h"
#include "HistoryLogger.h"
#include "OutputRegistry.h"
#include "RuleExecution.h"
#include "SystemProcessor.h"
#include "WorkItem.h"
#include "Worker.h"
#include "WorkOutputFeature.h"

namespace {

/// Name of the work item's current state, e.g. "Assigned".
std::string stateOf(WorkItem& item) {
    return item.process();
}

/// True if any log line contains the text.
bool logContains(const std::vector<std::string>& log, const std::string& text) {
    for (size_t i = 0; i < log.size(); ++i) {
        if (log[i].find(text) != std::string::npos) return true;
    }
    return false;
}

/**
 * @brief A donut-shop setup: a registry filled by the worker, and a facade that uses it.
 */
struct DonutShop {
    OutputRegistry registry;
    SystemProcessor processor;
    Worker baker;
    Client client;

    DonutShop() : baker("Sipho"), client("Thandi") {
        baker.setRegistry(&registry);
        baker.setWorkItemName("Party order");
        baker.setWorkItemDescription("Three dozen donuts for a birthday");
        // Factory Method creates each product, the registry keeps it as a prototype.
        baker.addToRegistry("Glazed donut", std::vector<std::string>(), false);
        baker.addToRegistry("Chocolate donut", std::vector<std::string>(), false);
        processor.setOutputRegistry(&registry);
    }
};

}  // namespace

TEST_CASE("Integration: processWorkItem runs a whole work item from start to completion") {
    DonutShop shop;
    std::vector<std::string> outputs = shop.processor.getAvailableOutputs();
    REQUIRE(outputs.size() == 2);

    CHECK(shop.processor.processWorkItem(&shop.baker, &shop.client, outputs));
}

TEST_CASE("Integration: a work item moves through every state, notifies observers and can be undone") {
    DonutShop shop;
    WorkItem item("Party order", "Three dozen donuts");
    HistoryLogger logger("audit");

    REQUIRE(shop.processor.startWorkflow(&item));
    item.addObserver(&logger);
    item.addObserver(&shop.client);
    CHECK(stateOf(item) == "InProgress");

    REQUIRE(shop.processor.assignWorkItem(&shop.baker));
    CHECK(stateOf(item) == "Assigned");
    CHECK(logContains(logger.getLog(), "is now Assigned"));

    // Proxy: the client adds a request through its ClientProcessor.
    shop.client.requestProcessor(&shop.baker);
    std::string glazed = shop.processor.getAvailableOutputs()[0];
    CHECK(shop.client.addRequest(glazed));
    CHECK(item.getRequests().size() == 1);

    // Memento: undo takes the request away again.
    shop.baker.undoLastAction();
    CHECK(item.getRequests().empty());
    CHECK(stateOf(item) == "Assigned");

    CHECK(shop.client.addRequest(glazed));

    // Strategy + Facade: the worker completes the item and the client gets the outputs.
    CHECK(shop.baker.completeWorkItem());
    CHECK(stateOf(item) == "Completed");
    CHECK(logContains(logger.getLog(), "is now Completed"));
}

TEST_CASE("Integration: a client cannot add a request that is not in the registry") {
    DonutShop shop;
    WorkItem item("Party order", "Three dozen donuts");
    shop.processor.startWorkflow(&item);
    item.addObserver(&shop.client);
    shop.processor.assignWorkItem(&shop.baker);
    shop.client.requestProcessor(&shop.baker);

    CHECK_FALSE(shop.client.addRequest("Pizza"));
    CHECK(item.getRequests().empty());
}

TEST_CASE("Integration: a special request decorates a registered product with a registered feature") {
    DonutShop shop;
    // The worker registers a feature prototype the client may choose from.
    shop.registry.registerOutput("Sprinkles", new WorkOutputFeature("Sprinkles"));

    WorkItem item("Party order", "Three dozen donuts");
    shop.processor.startWorkflow(&item);
    item.addObserver(&shop.client);
    shop.processor.assignWorkItem(&shop.baker);
    shop.client.requestProcessor(&shop.baker);

    std::string glazed;
    std::vector<std::string> available = shop.processor.getAvailableOutputs();
    for (size_t i = 0; i < available.size(); ++i) {
        if (available[i].find("Glazed donut") != std::string::npos) glazed = available[i];
    }
    REQUIRE_FALSE(glazed.empty());

    CHECK(shop.client.addSpecialRequest(&shop.baker, glazed, std::vector<std::string>(1, "Sprinkles")));
    REQUIRE(item.getRequests().size() == 1);
    CHECK(item.getRequests()[0].find("Sprinkles") != std::string::npos);
}

TEST_CASE("Integration: a strategy that refuses leaves the work item unfinished") {
    DonutShop shop;
    shop.baker.setTaskExecutionStrategy(new RuleExecution());
    WorkItem item("Party order", "");   // RuleExecution needs a description
    shop.processor.startWorkflow(&item);
    item.addObserver(&shop.client);
    shop.processor.assignWorkItem(&shop.baker);

    CHECK_FALSE(shop.baker.completeWorkItem());
    CHECK(stateOf(item) == "Assigned");
}

TEST_CASE("Integration: the client can cancel through its proxy") {
    DonutShop shop;
    WorkItem item("Party order", "Three dozen donuts");
    shop.processor.startWorkflow(&item);
    item.addObserver(&shop.client);
    shop.processor.assignWorkItem(&shop.baker);
    shop.client.requestProcessor(&shop.baker);

    shop.client.cancelWork();
    CHECK(stateOf(item) == "Rejected");
}

TEST_CASE("Integration: two workflow instances do not affect each other") {
    DonutShop shopA;
    DonutShop shopB;
    WorkItem itemA("Order A", "first");
    WorkItem itemB("Order B", "second");

    shopA.processor.startWorkflow(&itemA);
    shopB.processor.startWorkflow(&itemB);
    shopA.processor.assignWorkItem(&shopA.baker);

    CHECK(stateOf(itemA) == "Assigned");
    CHECK(stateOf(itemB) == "InProgress");
}
