/**
 * @file StrategyTests.cpp
 * @brief Unit tests for the Strategy pattern: Worker (Context),
 *        TaskExecutionStrategy and the concrete strategies RoleExecution,
 *        WorkloadExecution and RuleExecution.
 */

#include "doctest/doctest.h"

#include "Worker.h"
#include "WorkItem.h"
#include "TaskExecutionStrategy.h"
#include "RoleExecution.h"
#include "WorkloadExecution.h"
#include "RuleExecution.h"

#include <iostream>
#include <sstream>
#include <string>

namespace {

/// Silences std::cout while it is in scope, so Worker notifications don't clutter test output.
struct MuteCout {
    std::ostringstream sink;
    std::streambuf* old;
    MuteCout() : old(std::cout.rdbuf(sink.rdbuf())) {}
    ~MuteCout() { std::cout.rdbuf(old); }
};

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

} // namespace

TEST_CASE("Strategy: a new Worker starts with RoleExecution") {
    Worker worker("Bob");

    CHECK(dynamic_cast<RoleExecution*>(worker.getTaskExecutionStrategy()) != nullptr);
}

TEST_CASE("Strategy: the Worker's strategy can be swapped at runtime") {
    Worker worker("Bob");

    worker.setTaskExecutionStrategy(new WorkloadExecution());
    CHECK(dynamic_cast<WorkloadExecution*>(worker.getTaskExecutionStrategy()) != nullptr);

    worker.setTaskExecutionStrategy(new RuleExecution());
    CHECK(dynamic_cast<RuleExecution*>(worker.getTaskExecutionStrategy()) != nullptr);
}

TEST_CASE("Strategy: setting a null or identical strategy keeps the current one") {
    Worker worker("Bob");
    TaskExecutionStrategy* current = worker.getTaskExecutionStrategy();

    worker.setTaskExecutionStrategy(nullptr);
    CHECK(worker.getTaskExecutionStrategy() == current);

    worker.setTaskExecutionStrategy(current);
    CHECK(worker.getTaskExecutionStrategy() == current);
}

TEST_CASE("Strategy: RoleExecution only runs assigned WorkItems") {
    MuteCout mute;
    RoleExecution strategy;
    Worker worker("Bob");

    SUBCASE("refuses an unassigned WorkItem and leaves it unchanged") {
        WorkItem item("Report", "Write Q3 report");
        std::string result = strategy.execute(&item);
        CHECK(contains(result, "is not assigned"));
        CHECK(item.process() == "Available");
    }
    SUBCASE("runs an assigned WorkItem to Completed") {
        WorkItem item("Report", "Write Q3 report");
        item.assign(&worker);
        std::string result = strategy.execute(&item);
        CHECK(contains(result, "executed by its assigned role"));
        CHECK(item.process() == "Completed");
    }
}

TEST_CASE("Strategy: WorkloadExecution moves a WorkItem forward from where it is") {
    MuteCout mute;
    WorkloadExecution strategy;
    Worker worker("Bob");

    SUBCASE("Assigned is started and completed") {
        WorkItem item("Report", "Write Q3 report");
        item.assign(&worker);
        CHECK(contains(strategy.execute(&item), "started and completed"));
        CHECK(item.process() == "Completed");
    }
    SUBCASE("InProgress is completed") {
        WorkItem item("Report", "Write Q3 report");
        item.assign(&worker);
        item.start();
        CHECK(contains(strategy.execute(&item), "completed"));
        CHECK(item.process() == "Completed");
    }
    SUBCASE("Available cannot be moved forward") {
        WorkItem item("Report", "Write Q3 report");
        CHECK(contains(strategy.execute(&item), "could not be moved forward"));
        CHECK(item.process() == "Available");
    }
}

TEST_CASE("Strategy: RuleExecution checks its rules before running") {
    MuteCout mute;
    RuleExecution strategy;
    Worker worker("Bob");

    SUBCASE("missing description fails and leaves the WorkItem Assigned") {
        WorkItem item("Report", "");
        item.assign(&worker);
        CHECK(contains(strategy.execute(&item), "has no description"));
        CHECK(item.process() == "Assigned");
    }
    SUBCASE("unassigned WorkItem fails the Assigned rule") {
        WorkItem item("Report", "Write Q3 report");
        CHECK(contains(strategy.execute(&item), "must be Assigned"));
        CHECK(item.process() == "Available");
    }
    SUBCASE("all rules pass and the WorkItem is Completed") {
        WorkItem item("Report", "Write Q3 report");
        item.assign(&worker);
        CHECK(contains(strategy.execute(&item), "all rules passed"));
        CHECK(item.process() == "Completed");
    }
}

TEST_CASE("Strategy: every strategy handles a null WorkItem") {
    RoleExecution role;
    WorkloadExecution workload;
    RuleExecution rule;

    CHECK(contains(role.execute(nullptr), "no WorkItem given"));
    CHECK(contains(workload.execute(nullptr), "no WorkItem given"));
    CHECK(contains(rule.execute(nullptr), "no WorkItem given"));
}

TEST_CASE("Strategy: the Worker executes through its current strategy") {
    MuteCout mute;
    Worker worker("Bob");
    WorkItem item("Report", "");
    item.assign(&worker);

    worker.setTaskExecutionStrategy(new RuleExecution());
    std::string result = worker.getTaskExecutionStrategy()->execute(&item);

    CHECK(contains(result, "RuleExecution"));
    CHECK(item.process() == "Assigned");
}
