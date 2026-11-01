/**
 * @file FactoryMethodTests.cpp
 * @brief Unit tests for the Factory Method pattern (Producer, OutputProducer).
 */

#include "doctest/doctest.h"

#include "../include/OutputProducer.h"
#include "../include/WorkOutput.h"

#include <memory>
#include <string>

TEST_SUITE("FactoryMethod")
{
    TEST_CASE("OutputProducer produces a WorkOutput with the requested name")
    {
        OutputProducer producer; 
        std::unique_ptr<OutputComponent> output(producer.produceOutput("Discharge letter")); // creates a raw ptr the calles must delete

        REQUIRE(output != nullptr); // stops the test if nothing was created
        CHECK(dynamic_cast<WorkOutput*>(output.get()) != nullptr); // returns nullptr if the object is not really a WorkOutput
        CHECK(output->printOutput() == "Output #" + std::to_string(output->getOutputComponentNumber()) + ": Discharge letter");
    }

    TEST_CASE("a producer used through the Producer interface still creates the concrete product")
    { // client only knows Producer, the Abstract Creator
       // the call is virtual, so it still reach OutputProducer::factoryMethod(), and a WorkOutput comes back
        std::unique_ptr<Producer> producer(new OutputProducer());
        std::unique_ptr<OutputComponent> output(producer->produceOutput("Claim report"));

        REQUIRE(output != nullptr);
        CHECK(dynamic_cast<WorkOutput*>(output.get()) != nullptr);
    }

    TEST_CASE("every produced output gets a different number")
    {  // factory must create a new obj on every call, not return back the same obj
        OutputProducer producer;
        std::unique_ptr<OutputComponent> first(producer.produceOutput("A"));
        std::unique_ptr<OutputComponent> second(producer.produceOutput("B"));

        CHECK(first->getOutputComponentNumber() != second->getOutputComponentNumber());
    }

    TEST_CASE("an empty name is rejected")
    {
        OutputProducer producer; 
        CHECK(producer.produceOutput("") == nullptr);
    }
}
