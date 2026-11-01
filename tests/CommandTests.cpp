/**
 * @file CommandTests.cpp
 * @brief Unit tests for the Command pattern classes.
 *
 * The expected results follow group1's state machine:
 * Available --start--> InProgress --assign--> Assigned --complete--> Completed,
 * and cancel moves Available, InProgress or Assigned to Rejected.
 */
#include <memory>
#include <string>
#include <vector>

#include "doctest/doctest.h"

#include "Command.h"
#include "AddRequest.h"
#include "StartWorkflow.h"
#include "AssignWorkItem.h"
#include "CancelWorkItem.h"
#include "CompleteWorkItem.h"
#include "WorkItem.h"
#include "WorkItemState.h"
#include "Worker.h"

namespace {
/// Name of the work item's current state, e.g. "InProgress".
std::string stateOf(const WorkItem& item) {
    return item.getState()->process();
}
}

TEST_CASE("AddRequest adds its request to the work item") {
    WorkItem item("Task", "description");
    AddRequest cmd(&item, "review form");

    CHECK(cmd.execute());

    std::vector<std::string> requests = item.getRequests();
    REQUIRE(requests.size() == 1);
    CHECK(requests[0] == "review form");
}

TEST_CASE("AddRequest uses the request set by defineRequest") {
    WorkItem item("Task", "description");
    AddRequest cmd(&item, "first");
    cmd.defineRequest("second");

    CHECK(cmd.execute());

    std::vector<std::string> requests = item.getRequests();
    REQUIRE(requests.size() == 1);
    CHECK(requests[0] == "second");
}

TEST_CASE("StartWorkflow moves an available work item to InProgress") {
    WorkItem item("Task", "description");
    StartWorkflow cmd(&item);

    CHECK(cmd.execute());
    CHECK(stateOf(item) == "InProgress");
}

TEST_CASE("StartWorkflow fails if the work item is already in progress") {
    WorkItem item("Task", "description");
    StartWorkflow cmd(&item);
    cmd.execute();

    CHECK_FALSE(cmd.execute());
    CHECK(stateOf(item) == "InProgress");
}

TEST_CASE("AssignWorkItem assigns an in-progress work item to the defined worker") {
    WorkItem item("Task", "description");
    Worker alice("Alice");
    item.start();

    AssignWorkItem cmd(&item);
    cmd.defineWorker(&alice);

    CHECK(cmd.execute());
    CHECK(stateOf(item) == "Assigned");
}

TEST_CASE("AssignWorkItem does nothing until a worker has been defined") {
    WorkItem item("Task", "description");
    item.start();
    AssignWorkItem cmd(&item);

    CHECK_FALSE(cmd.execute());
    CHECK(stateOf(item) == "InProgress");
}

TEST_CASE("AssignWorkItem refuses a null worker") {
    WorkItem item("Task", "description");
    item.start();
    AssignWorkItem cmd(&item);
    cmd.defineWorker(nullptr);

    CHECK_FALSE(cmd.execute());
    CHECK(stateOf(item) == "InProgress");
}

TEST_CASE("AssignWorkItem cannot assign a work item that is already assigned") {
    WorkItem item("Task", "description");
    Worker alice("Alice");
    Worker bob("Bob");
    item.start();
    AssignWorkItem cmd(&item);

    cmd.defineWorker(&alice);
    CHECK(cmd.execute());

    cmd.defineWorker(&bob);
    CHECK_FALSE(cmd.execute());
    CHECK(stateOf(item) == "Assigned");
}

TEST_CASE("CompleteWorkItem completes an assigned work item") {
    WorkItem item("Task", "description");
    Worker alice("Alice");
    item.start();
    item.assign(&alice);
    CompleteWorkItem cmd(&item);

    CHECK(cmd.execute());
    CHECK(stateOf(item) == "Completed");
}

TEST_CASE("CompleteWorkItem refuses a work item that has not been assigned") {
    WorkItem item("Task", "description");
    CompleteWorkItem cmd(&item);

    CHECK_FALSE(cmd.execute());
    CHECK(stateOf(item) == "Available");
}

TEST_CASE("CancelWorkItem rejects an available work item") {
    WorkItem item("Task", "description");
    CancelWorkItem cmd(&item);

    CHECK(cmd.execute());
    CHECK(stateOf(item) == "Rejected");
}

TEST_CASE("CancelWorkItem cannot cancel a completed work item") {
    WorkItem item("Task", "description");
    Worker alice("Alice");
    item.start();
    item.assign(&alice);
    item.complete();
    CancelWorkItem cmd(&item);

    CHECK_FALSE(cmd.execute());
    CHECK(stateOf(item) == "Completed");
}

TEST_CASE("Commands fail safely when they have no work item") {
    Worker alice("Alice");
    AddRequest add(nullptr, "request");
    StartWorkflow start(nullptr);
    AssignWorkItem assign(nullptr);
    assign.defineWorker(&alice);
    CompleteWorkItem complete(nullptr);
    CancelWorkItem cancel(nullptr);

    CHECK_FALSE(add.execute());
    CHECK_FALSE(start.execute());
    CHECK_FALSE(assign.execute());
    CHECK_FALSE(complete.execute());
    CHECK_FALSE(cancel.execute());
}

TEST_CASE("Commands can be run polymorphically through the Command interface") {
    WorkItem item("Task", "description");
    Worker alice("Alice");

    AssignWorkItem* assign = new AssignWorkItem(&item);
    assign->defineWorker(&alice);

    std::vector<std::unique_ptr<Command> > commands;
    commands.push_back(std::unique_ptr<Command>(new AddRequest(&item, "first")));
    commands.push_back(std::unique_ptr<Command>(new StartWorkflow(&item)));
    commands.push_back(std::unique_ptr<Command>(assign));
    commands.push_back(std::unique_ptr<Command>(new CompleteWorkItem(&item)));

    for (size_t i = 0; i < commands.size(); ++i) {
        CHECK(commands[i]->execute());
    }

    CHECK(item.getRequests().size() == 1);
    CHECK(stateOf(item) == "Completed");
}
