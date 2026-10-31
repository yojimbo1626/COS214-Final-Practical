/**
 * @file StateTests.cpp
 * @brief Unit tests for the State pattern: WorkItem (Context), WorkItemState
 *        and the concrete states Available, Assigned, InProgress, Completed,
 *        Rejected and Blocked.
 */

#include "doctest/doctest.h"

#include "WorkItem.h"
#include "Worker.h"

#include <iostream>
#include <sstream>

namespace {

/// Silences std::cout while it is in scope, so Worker notifications don't clutter test output.
struct MuteCout {
    std::ostringstream sink;
    std::streambuf* old;
    MuteCout() : old(std::cout.rdbuf(sink.rdbuf())) {}
    ~MuteCout() { std::cout.rdbuf(old); }
};

} // namespace

TEST_CASE("State: a new WorkItem starts Available") {
    WorkItem item("Report", "Write Q3 report");

    CHECK(item.process() == "Available");
    CHECK(item.getState() != nullptr);
}

TEST_CASE("State: happy path Available -> Assigned -> InProgress -> Completed") {
    MuteCout mute;
    WorkItem item("Report", "Write Q3 report");
    Worker worker("Bob");

    CHECK(item.assign(&worker));
    CHECK(item.process() == "Assigned");

    CHECK(item.start());
    CHECK(item.process() == "InProgress");

    CHECK(item.complete());
    CHECK(item.process() == "Completed");
}

TEST_CASE("State: Available refuses invalid requests") {
    WorkItem item("Report", "Write Q3 report");

    CHECK_FALSE(item.start());
    CHECK_FALSE(item.complete());
    CHECK_FALSE(item.assign(nullptr));
    CHECK(item.process() == "Available");
}

TEST_CASE("State: Assigned refuses a second assign and an early complete") {
    MuteCout mute;
    WorkItem item("Report", "Write Q3 report");
    Worker first("Bob");
    Worker second("Ann");
    item.assign(&first);

    CHECK_FALSE(item.assign(&second));
    CHECK_FALSE(item.complete());
    CHECK_FALSE(item.blocked());
    CHECK(item.process() == "Assigned");
}

TEST_CASE("State: InProgress refuses start, assign, cancel and blocked") {
    MuteCout mute;
    WorkItem item("Report", "Write Q3 report");
    Worker worker("Bob");
    item.assign(&worker);
    item.start();

    CHECK_FALSE(item.start());
    CHECK_FALSE(item.assign(&worker));
    CHECK_FALSE(item.cancel());
    CHECK_FALSE(item.blocked());
    CHECK(item.process() == "InProgress");
}

TEST_CASE("State: cancel moves to Rejected before work starts") {
    MuteCout mute;
    Worker worker("Bob");

    SUBCASE("from Available") {
        WorkItem item("Report", "Write Q3 report");
        CHECK(item.cancel());
        CHECK(item.process() == "Rejected");
    }
    SUBCASE("from Assigned") {
        WorkItem item("Report", "Write Q3 report");
        item.assign(&worker);
        CHECK(item.cancel());
        CHECK(item.process() == "Rejected");
    }
    SUBCASE("from Blocked") {
        WorkItem item("Report", "Write Q3 report");
        item.blocked();
        CHECK(item.cancel());
        CHECK(item.process() == "Rejected");
    }
}

TEST_CASE("State: Blocked waits for its prerequisites") {
    MuteCout mute;
    WorkItem item("Ship order", "Depends on stock check and payment");
    Worker worker("Bob");

    REQUIRE(item.blocked());
    CHECK(item.process() == "Blocked");

    SUBCASE("refuses assign, start and complete while blocked") {
        CHECK_FALSE(item.assign(&worker));
        CHECK_FALSE(item.start());
        CHECK_FALSE(item.complete());
        CHECK(item.process() == "Blocked");
    }
    SUBCASE("blocked() again releases it back to Available, then it can be assigned") {
        CHECK(item.blocked());
        CHECK(item.process() == "Available");
        CHECK(item.assign(&worker));
        CHECK(item.process() == "Assigned");
    }
}

TEST_CASE("State: Completed and Rejected are final") {
    MuteCout mute;
    Worker worker("Bob");

    SUBCASE("Completed refuses everything") {
        WorkItem item("Report", "Write Q3 report");
        item.assign(&worker);
        item.start();
        item.complete();

        CHECK_FALSE(item.start());
        CHECK_FALSE(item.assign(&worker));
        CHECK_FALSE(item.complete());
        CHECK_FALSE(item.cancel());
        CHECK_FALSE(item.blocked());
        CHECK(item.process() == "Completed");
    }
    SUBCASE("Rejected refuses everything") {
        WorkItem item("Report", "Write Q3 report");
        item.cancel();

        CHECK_FALSE(item.start());
        CHECK_FALSE(item.assign(&worker));
        CHECK_FALSE(item.complete());
        CHECK_FALSE(item.cancel());
        CHECK_FALSE(item.blocked());
        CHECK(item.process() == "Rejected");
    }
}

TEST_CASE("State: WorkItems are isolated from each other") {
    MuteCout mute;
    WorkItem a("A", "First item");
    WorkItem b("B", "Second item");
    Worker worker("Bob");

    a.assign(&worker);
    a.start();
    b.blocked();

    CHECK(a.process() == "InProgress");
    CHECK(b.process() == "Blocked");
}

TEST_CASE("State: copy constructor copies data but starts Available") {
    MuteCout mute;
    WorkItem original("Report", "Write Q3 report");
    Worker worker("Bob");
    original.assign(&worker);

    WorkItem copy(original);

    CHECK(copy.getName() == "Report");
    CHECK(copy.getDescription() == "Write Q3 report");
    CHECK(copy.process() == "Available");
    CHECK(original.process() == "Assigned");
    CHECK(copy.getState() != original.getState());
}