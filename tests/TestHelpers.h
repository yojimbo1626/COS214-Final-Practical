/**
 * @file TestHelpers.h
 * @brief Shared test doubles for the unit tests (FacadeTests, ProxyTests, IteratorTests).
 *
 * This is a header, not a test file, so CMake's glob for the .cpp files in tests/ does not compile it on its own.
 * The test files include it with #include "TestHelpers.h", which the compiler finds in the
 * same folder as the test file.
 */
#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <string>

#include "OutputComponent.h"

/**
 * @class NamedOutput
 * @brief Minimal OutputComponent for tests: printOutput() returns just its name.
 *
 * The tests use it so that they don't depend on WorkOutput's "Output #n: name" text,
 * which changes with the global output counter.
 */
class NamedOutput : public OutputComponent {
public:
    /** @param name Name of the output. also what printOutput() returns. */
    explicit NamedOutput(const std::string& name) : OutputComponent(name), name(name) {}

    /** @return The name given to the constructor. */
    std::string printOutput() const override { return name; }

    /** @return A new NamedOutput with the same name, owned by the caller. */
    OutputComponent* clone() const override { return new NamedOutput(name); }

private:
    std::string name; ///< Copy of the name, since OutputComponent keeps its own name private.
};

#endif