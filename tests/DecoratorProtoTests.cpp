/**
 * @file DecoratorProtoTests.cpp
 * @brief Unit tests for the Decorator pattern (OutputComponent, WorkOutput, OutputFeature, WorkOutputFeature)
 * and the Prototype pattern (clone() and OutputRegistry as the prototype manager).
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "../include/OutputRegistry.h"
#include "../include/WorkOutput.h"
#include "../include/WorkOutputFeature.h"

#include <memory>
#include <string>

namespace
{
/// Text a component prints for itself. "Output #<number>: <name>".
std::string label(const OutputComponent& component, const std::string& name)
{
    return "Output #" + std::to_string(component.getOutputComponentNumber()) + ": " + name;
}
}

TEST_SUITE("Decorator")
{
    TEST_CASE("a plain WorkOutput prints its number and name")
    {
        WorkOutput output("Admission form");
        CHECK(output.printOutput() == label(output, "Admission form"));
    }

    TEST_CASE("a feature adds its own text after the wrapped output")
    {
        WorkOutput* plain = new WorkOutput("Invoice");
        std::string plainText = plain->printOutput();
        WorkOutputFeature signedInvoice("Signed");
        signedInvoice.add(plain);

        CHECK(signedInvoice.printOutput() == plainText + " + " + label(signedInvoice, "Signed"));
    }

    TEST_CASE("features can be stacked and print in wrapping order")
    {
        WorkOutput* plain = new WorkOutput("Report");
        std::string plainText = plain->printOutput();
        WorkOutputFeature* signedReport = new WorkOutputFeature("Signed");
        signedReport->add(plain);
        std::string signedLabel = label(*signedReport, "Signed");
        WorkOutputFeature urgent("Urgent");
        urgent.add(signedReport);

        CHECK(urgent.printOutput() == plainText + " + " + signedLabel + " + " + label(urgent, "Urgent"));
    }

    TEST_CASE("a feature with nothing wrapped prints only its own text")
    {
        WorkOutputFeature feature("Signed");
        CHECK(feature.printOutput() == " + " + label(feature, "Signed"));
    }

    TEST_CASE("add() replaces the wrapped output")
    {
        WorkOutputFeature feature("Signed");
        feature.add(new WorkOutput("Old"));
        WorkOutput* replacement = new WorkOutput("New");
        std::string replacementText = replacement->printOutput();
        feature.add(replacement);

        CHECK(feature.printOutput() == replacementText + " + " + label(feature, "Signed"));
    }

    TEST_CASE("add() ignores the feature itself, which would loop forever")
    {
        WorkOutput* plain = new WorkOutput("Report");
        std::string plainText = plain->printOutput();
        WorkOutputFeature feature("Signed");
        feature.add(plain);
        feature.add(&feature);

        CHECK(feature.printOutput() == plainText + " + " + label(feature, "Signed"));
    }
}

TEST_SUITE("Prototype")
{
    TEST_CASE("clone() returns a separate object that prints the same as the original")
    {
        WorkOutput original("Lab result");
        std::unique_ptr<OutputComponent> copy(original.clone());

        CHECK(copy.get() != &original);
        CHECK(dynamic_cast<WorkOutput*>(copy.get()) != nullptr);
        CHECK(copy->printOutput() == original.printOutput());
    }

    TEST_CASE("cloning a decorated output copies the whole chain")
    {
        WorkOutputFeature* original = new WorkOutputFeature("Signed");
        original->add(new WorkOutput("Contract"));
        std::unique_ptr<OutputComponent> copy(original->clone());
        std::string originalText = original->printOutput();

        // If the copy shared the original's wrapped output, deleting the original would leave the
        // copy pointing at freed memory
        delete original;

        CHECK(dynamic_cast<WorkOutputFeature*>(copy.get()) != nullptr);
        CHECK(copy->printOutput() == originalText);
    }

    TEST_CASE("the registry creates independent copies of a registered prototype")
    {
        OutputRegistry registry;
        WorkOutput* prototype = new WorkOutput("Invoice");
        registry.registerOutput("invoice", prototype);

        std::unique_ptr<OutputComponent> first(registry.createOutput("invoice"));
        std::unique_ptr<OutputComponent> second(registry.createOutput("invoice"));

        REQUIRE(first != nullptr);
        REQUIRE(second != nullptr);
        CHECK(first.get() != prototype);
        CHECK(first.get() != second.get());
        CHECK(first->printOutput() == prototype->printOutput());
    }

    TEST_CASE("unknown names return nullptr")
    {
        OutputRegistry registry;
        CHECK(registry.createOutput("missing") == nullptr);
    }

    TEST_CASE("registering under an existing name replaces the prototype")
    {
        OutputRegistry registry;
        registry.registerOutput("report", new WorkOutput("Old report"));
        WorkOutput* replacement = new WorkOutput("New report");
        registry.registerOutput("report", replacement);
        std::unique_ptr<OutputComponent> copy(registry.createOutput("report"));

        CHECK(copy->printOutput() == replacement->printOutput());
    }

    TEST_CASE("removeOutput() removes the prototype and unknown names are ignored")
    {
        OutputRegistry registry;
        registry.registerOutput("report", new WorkOutput("Report"));
        registry.removeOutput("missing");
        std::unique_ptr<OutputComponent> before(registry.createOutput("report"));
        CHECK(before != nullptr);

        registry.removeOutput("report");
        CHECK(registry.createOutput("report") == nullptr);
    }

    TEST_CASE("an empty name is rejected")
    {
        OutputRegistry registry;
        registry.registerOutput("", new WorkOutput("Nameless"));
        CHECK(registry.createOutput("") == nullptr);
    }
}
