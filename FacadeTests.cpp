/**
 * @file FacadeTests.cpp
 * @brief Unit tests for the Facade pattern (SystemProcessor).
 */
#include <string>

#include "doctest.h"

#include "SystemProcessor.h"
#include "WorkItem.h"
#include "Worker.h"

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
