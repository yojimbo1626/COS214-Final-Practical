/**
 * @file OutputProducer.cpp
 * @brief Implements OutputProducer.
 */

#include "../include/OutputProducer.h"

#include "../include/WorkOutput.h"

OutputComponent* OutputProducer::produceOutput(const std::string& name)
{
    if (name.empty())
    {
        return nullptr;
    }
    return factoryMethod(name);
}

OutputComponent* OutputProducer::factoryMethod(const std::string& name)
{
    return new WorkOutput(name);
}
