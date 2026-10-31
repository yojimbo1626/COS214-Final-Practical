/**
 * @file CommandTests.cpp
 * @brief Unit tests for the Command pattern classes.
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

TEST_CASE("StartWorkflow gives the same result as starting the work item directly") {
    WorkItem viaCommand("Task", "description");
    WorkItem direct("Task", "description");
    StartWorkflow cmd(&viaCommand);

    bool commandResult = cmd.execute();
    bool directResult = direct.start();

    CHECK(commandResult == directResult);
}

TEST_CASE("AssignWorkItem gives the same result as assigning the work item directly") {
    WorkItem viaCommand("Task", "description");
    WorkItem direct("Task", "description");
    AssignWorkItem cmd(&viaCommand);

    bool commandResult = cmd.execute();
    bool directResult = direct.assign();

    CHECK(commandResult == directResult);
}

TEST_CASE("CompleteWorkItem gives the same result as completing the work item directly") {
    WorkItem viaCommand("Task", "description");
    WorkItem direct("Task", "description");
    CompleteWorkItem cmd(&viaCommand);

    bool commandResult = cmd.execute();
    bool directResult = direct.complete();

    CHECK(commandResult == directResult);
}

TEST_CASE("CancelWorkItem gives the same result as cancelling the work item directly") {
    WorkItem viaCommand("Task", "description");
    WorkItem direct("Task", "description");
    CancelWorkItem cmd(&viaCommand);

    bool commandResult = cmd.execute();
    bool directResult = direct.cancel();

    CHECK(commandResult == directResult);
}

TEST_CASE("Commands fail safely when they have no work item") {
    AddRequest add(nullptr, "request");
    StartWorkflow start(nullptr);
    AssignWorkItem assign(nullptr);
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

    std::vector<std::unique_ptr<Command> > commands;
    commands.push_back(std::unique_ptr<Command>(new AddRequest(&item, "first")));
    commands.push_back(std::unique_ptr<Command>(new AddRequest(&item, "second")));

    for (size_t i = 0; i < commands.size(); ++i) {
        CHECK(commands[i]->execute());
    }

    CHECK(item.getRequests().size() == 2);
}
