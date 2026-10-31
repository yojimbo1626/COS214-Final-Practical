/**
 * @file ProxyTests.cpp
 * @brief Unit tests for the Proxy pattern (ClientProcessor).
 */
#include <string>

#include "doctest.h"

#include "ClientProcessor.h"
#include "SystemProcessor.h"
#include "WorkflowProcessor.h"
#include "OutputRegistry.h"
#include "WorkItem.h"
#include "TestHelpers.h"

namespace {

/**
 * @brief Test double that records what the proxy forwards to it.
 */
class SpyProcessor : public SystemProcessor {
public:
    SpyProcessor() : addCalls(0), cancelCalls(0), nextResult(true) {}

    bool addRequest(std::string request) override {
        ++addCalls;
        lastRequest = request;
        return nextResult;
    }

    void cancelWork() override { ++cancelCalls; }

    int addCalls;
    int cancelCalls;
    std::string lastRequest;
    bool nextResult;
};

}  

TEST_CASE("ClientProcessor forwards addRequest to the real processor") {
    SpyProcessor spy;
    ClientProcessor proxy(&spy);

    CHECK(proxy.addRequest("print report"));

    CHECK(spy.addCalls == 1);
    CHECK(spy.lastRequest == "print report");
}

TEST_CASE("ClientProcessor returns the real processor's result") {
    SpyProcessor spy;
    ClientProcessor proxy(&spy);

    spy.nextResult = false;
    CHECK_FALSE(proxy.addRequest("anything"));

    spy.nextResult = true;
    CHECK(proxy.addRequest("anything"));
}

TEST_CASE("ClientProcessor forwards cancelWork to the real processor") {
    SpyProcessor spy;
    ClientProcessor proxy(&spy);

    proxy.cancelWork();

    CHECK(spy.cancelCalls == 1);
}

TEST_CASE("ClientProcessor can be used wherever a WorkflowProcessor is expected") {
    SpyProcessor spy;
    ClientProcessor proxy(&spy);
    WorkflowProcessor* processor = &proxy;

    CHECK(processor->addRequest("via interface"));
    processor->cancelWork();

    CHECK(spy.addCalls == 1);
    CHECK(spy.cancelCalls == 1);
}

TEST_CASE("ClientProcessor fails safely when it has no real processor") {
    ClientProcessor proxy(nullptr);

    CHECK_FALSE(proxy.addRequest("anything"));
    CHECK_NOTHROW(proxy.cancelWork());
}

TEST_CASE("Through the proxy, only requests that match a registered output get in") {
    OutputRegistry registry;
    registry.registerOutput("report", new NamedOutput("report"));
    registry.registerOutput("invoice", new NamedOutput("invoice"));

    WorkItem item("Task", "description");
    SystemProcessor real;
    real.setOutputRegistry(&registry);
    real.startWorkflow(&item);
    ClientProcessor proxy(&real);

    CHECK(proxy.addRequest("report"));
    CHECK(proxy.addRequest("invoice"));
    CHECK_FALSE(proxy.addRequest("receipt"));

    CHECK(item.getRequests().size() == 2);
}

TEST_CASE("Through the proxy, requests are refused until a workflow has started") {
    OutputRegistry registry;
    registry.registerOutput("report", new NamedOutput("report"));

    WorkItem item("Task", "description");
    SystemProcessor real;
    real.setOutputRegistry(&registry);
    ClientProcessor proxy(&real);

    CHECK_FALSE(proxy.addRequest("report"));
    CHECK(item.getRequests().empty());
}

TEST_CASE("Through the proxy, cancelWork cancels the real work item") {
    WorkItem viaProxy("Task", "description");
    WorkItem direct("Task", "description");
    SystemProcessor real;
    real.startWorkflow(&viaProxy);
    direct.start();
    ClientProcessor proxy(&real);

    proxy.cancelWork();
    direct.cancel();

    CHECK(viaProxy.process() == direct.process());
}
