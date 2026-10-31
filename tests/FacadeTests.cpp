/**
 * @file FacadeTests.cpp
 * @brief Unit tests for the Facade pattern (SystemProcessor).
 */
#include <string>
#include <vector>

#include "doctest.h"

#include "SystemProcessor.h"
#include "OutputRegistry.h"
#include "WorkItem.h"
#include "Worker.h"
#include "TestHelpers.h"

namespace {

/**
 * @brief A processor wired up with a registry and a work item, ready to use.
 *
 * The registry owns every output registered in it. It is declared first so it
 * outlives the processor that points at it.
 */
struct Fixture {
    OutputRegistry registry;
    WorkItem item;
    SystemProcessor processor;

    Fixture() : item("Task", "description") {
        processor.setOutputRegistry(&registry);
    }

    /** @brief Registers an output whose text is the given name. */
    void addOutput(const std::string& name) {
        registry.registerOutput(name, new NamedOutput(name));
    }
};

}  
TEST_CASE("SystemProcessor refuses work before a workflow has been started") {
    SystemProcessor processor;
    Worker worker("Alice");

    CHECK_FALSE(processor.addRequest("anything"));
    CHECK_FALSE(processor.assignWorkItem(&worker));
    CHECK_FALSE(processor.completeWork());
    CHECK_NOTHROW(processor.cancelWork());
}

TEST_CASE("SystemProcessor cannot start a workflow without a work item") {
    SystemProcessor processor;

    CHECK_FALSE(processor.startWorkflow(nullptr));
}

TEST_CASE("startWorkflow behaves like starting the work item directly") {
    WorkItem viaFacade("Task", "description");
    WorkItem direct("Task", "description");
    SystemProcessor processor;

    bool facadeResult = processor.startWorkflow(&viaFacade);
    bool directResult = direct.start();

    CHECK(facadeResult == directResult);
}

TEST_CASE("completeWork behaves like completing the work item directly") {
    WorkItem viaFacade("Task", "description");
    WorkItem direct("Task", "description");
    SystemProcessor processor;

    processor.startWorkflow(&viaFacade);
    direct.start();

    bool facadeResult = processor.completeWork();
    bool directResult = direct.complete();

    CHECK(facadeResult == directResult);
}

TEST_CASE("cancelWork behaves like cancelling the work item directly") {
    WorkItem viaFacade("Task", "description");
    WorkItem direct("Task", "description");
    SystemProcessor processor;

    processor.startWorkflow(&viaFacade);
    direct.start();

    processor.cancelWork();
    direct.cancel();

    CHECK(viaFacade.process() == direct.process());
}

TEST_CASE("Two SystemProcessors keep their workflows isolated") {
    WorkItem itemA("Task", "description");
    SystemProcessor processorA;
    SystemProcessor processorB;

    processorA.startWorkflow(&itemA);

    CHECK_FALSE(processorB.completeWork());
}

TEST_CASE("addRequest is refused when no registry has been set") {
    WorkItem item("Task", "description");
    SystemProcessor processor;
    processor.startWorkflow(&item);

    CHECK_FALSE(processor.addRequest("report"));
    CHECK(item.getRequests().empty());
}

TEST_CASE("addRequest is refused when the registry is empty") {
    Fixture f;
    f.processor.startWorkflow(&f.item);

    CHECK_FALSE(f.processor.addRequest("report"));
    CHECK(f.item.getRequests().empty());
}

TEST_CASE("addRequest is refused when the text matches no output in the registry") {
    Fixture f;
    f.addOutput("report");
    f.addOutput("invoice");
    f.processor.startWorkflow(&f.item);

    CHECK_FALSE(f.processor.addRequest("receipt"));
    CHECK(f.item.getRequests().empty());
}

TEST_CASE("addRequest needs an exact match, not a partial one") {
    Fixture f;
    f.addOutput("report");
    f.processor.startWorkflow(&f.item);

    CHECK_FALSE(f.processor.addRequest("repo"));
    CHECK_FALSE(f.processor.addRequest("report "));
    CHECK_FALSE(f.processor.addRequest("Report"));
    CHECK_FALSE(f.processor.addRequest(""));
    CHECK(f.item.getRequests().empty());
}

TEST_CASE("addRequest accepts a matching request and it reaches the work item") {
    Fixture f;
    f.addOutput("report");
    f.processor.startWorkflow(&f.item);

    CHECK(f.processor.addRequest("report"));

    std::vector<std::string> requests = f.item.getRequests();
    REQUIRE(requests.size() == 1);
    CHECK(requests[0] == "report");
}

TEST_CASE("addRequest finds a match wherever it sits in the registry") {
    const char* names[] = {"alpha", "bravo", "charlie"};

    for (int i = 0; i < 3; ++i) {
        CAPTURE(names[i]);

        Fixture f;
        f.addOutput("alpha");
        f.addOutput("bravo");
        f.addOutput("charlie");
        f.processor.startWorkflow(&f.item);

        CHECK(f.processor.addRequest(names[i]));
        CHECK(f.item.getRequests().size() == 1);
    }
}

TEST_CASE("addRequest works when the registry has exactly one output") {
    Fixture f;
    f.addOutput("only");
    f.processor.startWorkflow(&f.item);

    CHECK(f.processor.addRequest("only"));
    CHECK_FALSE(f.processor.addRequest("other"));
    CHECK(f.item.getRequests().size() == 1);
}

TEST_CASE("addRequest is refused before the workflow has started, even for a valid request") {
    Fixture f;
    f.addOutput("report");

    CHECK_FALSE(f.processor.addRequest("report"));
    CHECK(f.item.getRequests().empty());
}

TEST_CASE("A valid request can be added more than once") {
    Fixture f;
    f.addOutput("report");
    f.addOutput("invoice");
    f.processor.startWorkflow(&f.item);

    CHECK(f.processor.addRequest("report"));
    CHECK(f.processor.addRequest("invoice"));
    CHECK(f.processor.addRequest("report"));

    CHECK(f.item.getRequests().size() == 3);
}


TEST_CASE("getAvailableOutputs is empty when no registry has been set") {
    SystemProcessor processor;

    CHECK(processor.getAvailableOutputs().empty());
}

TEST_CASE("getAvailableOutputs is empty when the registry is empty") {
    Fixture f;

    CHECK(f.processor.getAvailableOutputs().empty());
}

TEST_CASE("getAvailableOutputs lists every output in the registry") {
    Fixture f;
    f.addOutput("charlie");
    f.addOutput("alpha");
    f.addOutput("bravo");

    std::vector<std::string> outputs = f.processor.getAvailableOutputs();

    REQUIRE(outputs.size() == 3);
    CHECK(outputs[0] == "alpha");
    CHECK(outputs[1] == "bravo");
    CHECK(outputs[2] == "charlie");
}

TEST_CASE("getAvailableOutputs lists a registry with a single output") {
    Fixture f;
    f.addOutput("only");

    std::vector<std::string> outputs = f.processor.getAvailableOutputs();

    REQUIRE(outputs.size() == 1);
    CHECK(outputs[0] == "only");
}
