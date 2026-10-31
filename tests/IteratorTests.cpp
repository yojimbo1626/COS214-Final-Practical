/**
 * @file IteratorTests.cpp
 * @brief Unit tests for the Iterator pattern (Iterator<Item>, Registry<Item>, OutputRegistryIterator).
 *
 * 
 * Outputs are visited in name order, because the registry keeps them in a std::map.
 *
 * Because next() wraps instead of returning nullptr, every loop in these tests is
 * guarded by hasNext() (a loop that only waits for nullptr would never end).
 */
#include <memory>
#include <string>
#include <vector>

#include "doctest/doctest.h"

#include "OutputRegistry.h"  // keep this first: these headers include each other
#include "OutputRegistryIterator.h"
#include "Iterator.h"
#include "Registry.h"
#include "OutputComponent.h"
#include "TestHelpers.h"

namespace {

/**
 * @brief Creates an output and registers it under the given name.
 * The registry owns it, so the tests never delete it themselves.
 */
OutputComponent* addOutput(OutputRegistry& registry, const std::string& name) {
    OutputComponent* component = new NamedOutput(name);
    registry.registerOutput(name, component);
    return component;
}

/**
 * @brief Walks an iterator from first() to the last output and returns what it visited.
 */
std::vector<OutputComponent*> visitAll(Iterator<OutputComponent>& it) {
    std::vector<OutputComponent*> visited;
    OutputComponent* c = it.first();
    if (c == nullptr) return visited;
    visited.push_back(c);
    while (it.hasNext()) {
        visited.push_back(it.next());
    }
    return visited;
}

}  
TEST_CASE("Iterator over an empty registry has nothing to visit") {
    OutputRegistry registry;
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    CHECK_FALSE(it->hasNext());
    CHECK(it->first() == nullptr);
    CHECK(it->next() == nullptr);
}

TEST_CASE("Iterator over a registry with one output visits just that output") {
    OutputRegistry registry;
    OutputComponent* only = addOutput(registry, "only");
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    CHECK(it->first() == only);
    CHECK_FALSE(it->hasNext());
}

TEST_CASE("With one output, next() wraps straight back to that same output") {
    OutputRegistry registry;
    OutputComponent* only = addOutput(registry, "only");
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    CHECK(it->first() == only);
    CHECK(it->next() == only);
    CHECK(it->next() == only);
    CHECK_FALSE(it->hasNext());
}

TEST_CASE("first() returns the output that comes first by name") {
    OutputRegistry registry;
    addOutput(registry, "charlie");
    OutputComponent* alpha = addOutput(registry, "alpha");
    addOutput(registry, "bravo");
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    CHECK(it->first() == alpha);
}

TEST_CASE("next() walks the outputs in name order and hasNext() stops at the last one") {
    OutputRegistry registry;
    OutputComponent* charlie = addOutput(registry, "charlie");
    OutputComponent* alpha = addOutput(registry, "alpha");
    OutputComponent* bravo = addOutput(registry, "bravo");
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    CHECK(it->first() == alpha);
    CHECK(it->hasNext());
    CHECK(it->next() == bravo);
    CHECK(it->hasNext());
    CHECK(it->next() == charlie);
    CHECK_FALSE(it->hasNext());
}

TEST_CASE("A full walk visits every output exactly once, in order") {
    OutputRegistry registry;
    std::vector<OutputComponent*> expected;
    expected.push_back(addOutput(registry, "o1"));
    expected.push_back(addOutput(registry, "o2"));
    expected.push_back(addOutput(registry, "o3"));
    expected.push_back(addOutput(registry, "o4"));
    expected.push_back(addOutput(registry, "o5"));
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    std::vector<OutputComponent*> visited = visitAll(*it);

    CHECK(visited == expected);
}

TEST_CASE("hasNext() only looks ahead, it does not move the cursor") {
    OutputRegistry registry;
    OutputComponent* alpha = addOutput(registry, "alpha");
    OutputComponent* bravo = addOutput(registry, "bravo");
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    CHECK(it->first() == alpha);
    CHECK(it->hasNext());
    CHECK(it->hasNext());
    CHECK(it->hasNext());
    CHECK(it->next() == bravo);
}


TEST_CASE("next() from the last output wraps round to the first") {
    OutputRegistry registry;
    OutputComponent* alpha = addOutput(registry, "alpha");
    OutputComponent* bravo = addOutput(registry, "bravo");
    OutputComponent* charlie = addOutput(registry, "charlie");
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    it->first();
    it->next();
    CHECK(it->next() == charlie);
    CHECK_FALSE(it->hasNext());

    CHECK(it->next() == alpha);

    CHECK(it->hasNext());
    CHECK(it->next() == bravo);
}

TEST_CASE("first() restarts the walk after the iterator has reached the end") {
    OutputRegistry registry;
    OutputComponent* alpha = addOutput(registry, "alpha");
    addOutput(registry, "bravo");
    addOutput(registry, "charlie");
    std::unique_ptr<OutputRegistryIterator> it(registry.createIterator());

    visitAll(*it);
    CHECK_FALSE(it->hasNext());

    CHECK(it->first() == alpha);
    CHECK(it->hasNext());
}


TEST_CASE("Iterators from the same registry do not affect each other") {
    OutputRegistry registry;
    OutputComponent* alpha = addOutput(registry, "alpha");
    addOutput(registry, "bravo");
    addOutput(registry, "charlie");
    std::unique_ptr<OutputRegistryIterator> one(registry.createIterator());
    std::unique_ptr<OutputRegistryIterator> two(registry.createIterator());

    CHECK(one.get() != two.get());

    one->first();
    one->next();
    one->next();
    CHECK_FALSE(one->hasNext());

    CHECK(two->first() == alpha);
    CHECK(two->hasNext());
}


TEST_CASE("The iterator can be used through the abstract Iterator interface") {
    OutputRegistry registry;
    OutputComponent* alpha = addOutput(registry, "alpha");
    OutputComponent* bravo = addOutput(registry, "bravo");
    std::unique_ptr<Iterator<OutputComponent> > it(registry.createIterator());

    std::vector<OutputComponent*> visited = visitAll(*it);

    REQUIRE(visited.size() == 2);
    CHECK(visited[0] == alpha);
    CHECK(visited[1] == bravo);
}

TEST_CASE("The registry hands out iterators through the Registry interface") {
    OutputRegistry registry;
    OutputComponent* alpha = addOutput(registry, "alpha");
    OutputComponent* bravo = addOutput(registry, "bravo");
    Registry<OutputComponent>& asInterface = registry;
    std::unique_ptr<Iterator<OutputComponent> > it(asInterface.createIterator());

    CHECK(it->first() == alpha);
    CHECK(it->next() == bravo);
    CHECK_FALSE(it->hasNext());
}

TEST_CASE("An OutputRegistryIterator can also be built directly from a registry") {
    OutputRegistry registry;
    OutputComponent* alpha = addOutput(registry, "alpha");
    OutputComponent* bravo = addOutput(registry, "bravo");
    OutputRegistryIterator it(&registry);

    CHECK(it.first() == alpha);
    CHECK(it.next() == bravo);
    CHECK_FALSE(it.hasNext());
}
