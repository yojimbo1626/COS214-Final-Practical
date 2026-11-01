/**
 * @file TestHelpers.h
 * @brief Small helpers shared by the Facade and Proxy tests.
 */
#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <string>

#include "OutputComponent.h"

/**
 * @class NamedOutput
 * @brief An OutputComponent whose toString() is exactly the text it was given.
 *
 * SystemProcessor::addRequest compares the request with each output's toString(),
 * so the tests need outputs whose text they control.
 *
 * ADJUST IF NEEDED: this assumes OutputComponent has a constructor that takes a
 * name and a virtual toString(). If your OutputComponent is abstract, also
 * implement its pure virtual functions here (for example printOutput and clone).
 */
class NamedOutput : public OutputComponent {
private:
    std::string text; ///< The text returned by toString().

public:
    /**
     * @brief Creates an output with the given text.
     * @param text The text toString() will return.
     */
    explicit NamedOutput(const std::string& text) : OutputComponent(text), text(text) {}

    /** @brief Returns the text this output was created with. */
    std::string toString() override { return text; }
};

#endif