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
#include <string>

namespace {

/// Silences std::cout while it is in scope, so Worker notifications don't clutter test output.
struct MuteCout {
    std::ostringstream sink;
    std::streambuf* old;
    MuteCout() : old(std::cout.rdbuf(sink.rdbuf())) {}
    ~MuteCout() { std::cout.rdbuf(old); forward(sink.str()); }
    /// Restores std::cout, then re-prints anything that isn't a Worker/Client
    /// notification, so doctest's own failure reports are never hidden.
    void forward(const std::string& text) const {
        std::istringstream lines(text);
        std::string line;
        while (std::getline(lines, line)) {
            if (line.rfind("[Worker #", 0) != 0 && line.rfind("[Client #", 0) != 0) {
                std::cout << line << '\n';
            }
        }
    }
};

} // namespace

TEST_CASE("State: a new WorkItem starts Available") {
    WorkItem item("Report", "Write Q3 report");

    CHECK(item.process() == "Available");
    CHECK(item.getState() != nullptr);
}

TEST_CASE("State: happy path Available -> InProgress -> Assigned -> Completed") {
    MuteCout mute;
    WorkItem item("Report", "Write Q3 report");
    Worker worker("Bob");

    CHECK(item.start());
    CHECK(item.process() == "InProgress");

    CHECK(item.assign(&worker));
    CHECK(item.process() == "Assigned");

    CHECK(item.complete());
    CHECK(item.process() == "Completed");
}

TEST_CASE("State: Available refuses invalid requests") {
    MuteCout mute;
    WorkItem item("Report", "Write Q3 report");
    Worker worker("Bob");

    CHECK_FALSE(item.assign(&worker));
    CHECK_FALSE(item.complete());
    CHECK(item.process() == "Available");
}

TEST_CASE("State: InProgress refuses a second start, an early complete and blocked") {
    MuteCout mute;
    WorkItem item("Report", "Write Q3 report");
    item.start();

    CHECK_FALSE(item.start());
    CHECK_FALSE(item.complete());
    CHECK_FALSE(item.blocked());
    CHECK_FALSE(item.assign(nullptr));
    CHECK(item.process() == "InProgress");
}

TEST_CASE("State: Assigned refuses start, a second assign and blocked") {
    MuteCout mute;
    WorkItem item("Report", "Write Q3 report");
    Worker first("Bob");
    Worker second("Ann");
    item.start();
    item.assign(&first);

    CHECK_FALSE(item.start());
    CHECK_FALSE(item.assign(&second));
    CHECK_FALSE(item.blocked());
    CHECK(item.process() == "Assigned");
}

TEST_CASE("State: cancel moves to Rejected from every non-final state") {
    MuteCout mute;
    Worker worker("Bob");

    SUBCASE("from Available") {
        WorkItem item("Report", "Write Q3 report");
        CHECK(item.cancel());
        CHECK(item.process() == "Rejected");
    }
    SUBCASE("from InProgress") {
        WorkItem item("Report", "Write Q3 report");
        item.start();
        CHECK(item.cancel());
        CHECK(item.process() == "Rejected");
    }
    SUBCASE("from Assigned") {
        WorkItem item("Report", "Write Q3 report");
        item.start();
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

    SUBCASE("refuses start, assign and complete while blocked") {
        CHECK_FALSE(item.start());
        CHECK_FALSE(item.assign(&worker));
        CHECK_FALSE(item.complete());
        CHECK(item.process() == "Blocked");
    }
    SUBCASE("blocked() again releases it back to Available, then it can be started") {
        CHECK(item.blocked());
        CHECK(item.process() == "Available");
        CHECK(item.start());
        CHECK(item.process() == "InProgress");
    }
}

TEST_CASE("State: Completed and Rejected are final") {
    MuteCout mute;
    Worker worker("Bob");

    SUBCASE("Completed refuses everything") {
        WorkItem item("Report", "Write Q3 report");
        item.start();
        item.assign(&worker);
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

    a.start();
    a.assign(&worker);
    b.blocked();

    CHECK(a.process() == "Assigned");
    CHECK(b.process() == "Blocked");
}

TEST_CASE("State: copy constructor copies data but starts Available") {
    MuteCout mute;
    WorkItem original("Report", "Write Q3 report");
    original.start();

    WorkItem copy(original);

    CHECK(copy.getName() == "Report");
    CHECK(copy.getDescription() == "Write Q3 report");
    CHECK(copy.process() == "Available");
    CHECK(original.process() == "InProgress");
    CHECK(copy.getState() != original.getState());
}