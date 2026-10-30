/**
 * @file OutputRegistry.cpp
 * @brief Implements OutputRegistry, the PrototypeManager and ConcreteAggregate.
 *
 * Each member function is documented where it is declared, in OutputRegistry.h.
 */



#include "../include/OutputRegistry.h"
#include "../include/OutputRegistryIterator.h"

OutputRegistry::~OutputRegistry() 
{
    for (auto& entry : workOutputs) 
    {
        delete entry.second;
    }
}

Iterator<OutputComponent>* OutputRegistry::createIterator() 
{
    return new OutputRegistryIterator(this);
}


OutputComponent* OutputRegistry::createOutput(const std::string& output) 
{
    auto it = workOutputs.find(output);

    if (it == workOutputs.end()) 
    {
        return nullptr;
    }

    return it->second->clone();
}
