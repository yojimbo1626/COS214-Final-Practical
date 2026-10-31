/**
 * @file ObserverTests.cpp
 * @brief Unit tests for the Observer pattern: WorkUnit (Subject), WorkItem
 *        (ConcreteSubject), Participant (Observer) and the concrete observers
 *        Client, Worker and HistoryLogger.
 */

#include "doctest/doctest.h"

#include "WorkItem.h"
#include "Client.h"
#include "Worker.h"
#include "HistoryLogger.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

/// Captures everything written to std::cout while it is in scope.
struct CaptureCout {
    std::ostringstream buffer;
    std::streambuf* old;
    CaptureCout() : old(std::cout.rdbuf(buffer.rdbuf())) {}
    ~CaptureCout() { std::cout.rdbuf(old); }
    std::string text() const { return buffer.str(); }
};

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

} // namespace

TEST_CASE("Observer: a subscribed observer receives every state change") {
    CaptureCout capture;
    WorkItem item("Report", "Write Q3 report");
    HistoryLogger logger("Audit");
    Worker worker("Bob");
    item.addObserver(&logger);

    item.assign(&worker);
    item.start();
    item.complete();

    std::vector<std::string> log = logger.getLog();
    REQUIRE(log.size() == 3);
    CHECK(log[0] == "WorkItem 'Report' is now Assigned");
    CHECK(log[1] == "WorkItem 'Report' is now InProgress");
    CHECK(log[2] == "WorkItem 'Report' is now Completed");
}

TEST_CASE("Observer: a refused request sends no notification") {
    WorkItem item("Report", "Write Q3 report");
    HistoryLogger logger("Audit");
    item.addObserver(&logger);

    CHECK_FALSE(item.start());
    CHECK_FALSE(item.complete());

    CHECK(logger.getLog().empty());
}

TEST_CASE("Observer: removeObserver stops updates") {
    WorkItem item("Report", "Write Q3 report");
    HistoryLogger logger("Audit");
    item.addObserver(&logger);

    item.blocked();
    item.removeObserver(&logger);
    item.blocked();

    REQUIRE(logger.getLog().size() == 1);
    CHECK(logger.getLog()[0] == "WorkItem 'Report' is now Blocked");
}

TEST_CASE("Observer: duplicate and null subscriptions are ignored") {
    WorkItem item("Report", "Write Q3 report");
    HistoryLogger logger("Audit");

    item.addObserver(&logger);
    item.addObserver(&logger);
    item.addObserver(nullptr);
    item.blocked();

    CHECK(logger.getLog().size() == 1);
}

TEST_CASE("Observer: removing an observer that was never added does nothing") {
    WorkItem item("Report", "Write Q3 report");
    HistoryLogger subscribed("Audit");
    HistoryLogger stranger("Other");
    item.addObserver(&subscribed);

    item.removeObserver(&stranger);
    item.blocked();

    CHECK(subscribed.getLog().size() == 1);
    CHECK(stranger.getLog().empty());
}

TEST_CASE("Observer: assigning a Worker subscribes it automatically") {
    CaptureCout capture;
    WorkItem item("Report", "Write Q3 report");
    Worker worker("Bob");

    item.assign(&worker);
    item.start();

    std::string out = capture.text();
    CHECK(contains(out, worker.toString() + "] received update: WorkItem 'Report' is now Assigned"));
    CHECK(contains(out, worker.toString() + "] received update: WorkItem 'Report' is now InProgress"));
}

TEST_CASE("Observer: Client prints the pushed update") {
    CaptureCout capture;
    WorkItem item("Report", "Write Q3 report");
    Client client("Thandi");
    item.addObserver(&client);

    item.blocked();

    CHECK(contains(capture.text(), "[" + client.toString() + "] received update: WorkItem 'Report' is now Blocked"));
}

TEST_CASE("Observer: getClient finds the subscribed Client") {
    CaptureCout capture;
    WorkItem item("Report", "Write Q3 report");
    Client client("Thandi");
    Worker worker("Bob");
    HistoryLogger logger("Audit");

    CHECK(item.getClient() == nullptr);

    item.addObserver(&logger);
    item.assign(&worker);
    CHECK(item.getClient() == nullptr);

    item.addObserver(&client);
    CHECK(item.getClient() == &client);
}

TEST_CASE("Observer: each WorkItem notifies only its own observers") {
    WorkItem a("A", "First item");
    WorkItem b("B", "Second item");
    HistoryLogger logA("Audit A");
    HistoryLogger logB("Audit B");
    a.addObserver(&logA);
    b.addObserver(&logB);

    a.blocked();
    b.cancel();

    REQUIRE(logA.getLog().size() == 1);
    REQUIRE(logB.getLog().size() == 1);
    CHECK(logA.getLog()[0] == "WorkItem 'A' is now Blocked");
    CHECK(logB.getLog()[0] == "WorkItem 'B' is now Rejected");
}

TEST_CASE("Observer: Participants get unique, increasing ids") {
    Client client("Thandi");
    Worker worker("Bob");
    HistoryLogger logger("Audit");

    CHECK(worker.getId() == client.getId() + 1);
    CHECK(logger.getId() == worker.getId() + 1);
    CHECK(client.getName() == "Thandi");
}

TEST_CASE("Observer: toString formats for each concrete observer") {
    Client client("Thandi");
    Worker worker("Bob");
    HistoryLogger logger("Audit");

    CHECK(client.toString() == "Client #" + std::to_string(client.getId()) + " (Thandi)");
    CHECK(worker.toString() == "Worker #" + std::to_string(worker.getId()) + " (Bob)");
    CHECK(logger.toString() == "HistoryLogger #" + std::to_string(logger.getId()) + " (Audit): 0 entries");
}