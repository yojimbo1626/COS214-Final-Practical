/**
 * @file Producer.h
 * @brief Declares Producer, the abstract creator of outputs.
 *
 * Pattern role:
 * - Factory Method: Creator
 */

#ifndef PRODUCER_H
#define PRODUCER_H

#include <string>

class OutputComponent;

/**
 * @class Producer
 * @brief Produces outputs without knowing which concrete output class is created.
 */
class Producer
{
public:
    /** @brief Virtual destructor so subclasses are destroyed correctly. */
    virtual ~Producer();

    /**
     * @brief Produces a new output with the given name.
     * @param name Name of the output to produce.
     * @return A new output; the caller owns it.
     */
    virtual OutputComponent* produceOutput(const std::string& name) = 0;

protected:
    /**
     * @brief The factory method: subclasses decide which concrete output to create.
     * @param name Name to give the new output (see the team decision on this parameter).
     * @return A new output; the caller owns it.
     */
    virtual OutputComponent* factoryMethod(const std::string& name) = 0;
};

#endif
