/**
 * @file OutputProducer.h
 * @brief Declares OutputProducer, the concrete creator that makes WorkOutput objects.
 *
 * Pattern role:
 *  Factory Method: ConcreteCreator
 */

#ifndef OUTPUTPRODUCER_H
#define OUTPUTPRODUCER_H

#include "Producer.h"

/**
 * @class OutputProducer
 * @brief Creates plain WorkOutput objects.
 */
class OutputProducer : public Producer
{
public:
    /**
     * @brief Produces a WorkOutput with the given name by calling factoryMethod().
     * @param name Name of the output to produce. Must not be empty.
     * @return A new WorkOutput, or nullptr if the name is empty. The caller owns it.
     */
    virtual OutputComponent* produceOutput(const std::string& name);

protected:
    /**
     * @brief Creates a WorkOutput.
     * @param name Name to give the new output.
     * @return A new WorkOutput; the caller owns it.
     */
    virtual OutputComponent* factoryMethod(const std::string& name);
};

#endif
