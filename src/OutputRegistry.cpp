/**
 * @file OutputRegistry.cpp
 * @brief Implements OutputRegistry, the PrototypeManager and ConcreteAggregate
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


void OutputRegistry::removeOutput(const std::string& output) 
{
    auto it = workOutputs.find(output);

    if (it != workOutputs.end()) 
    {
        delete it->second;
        workOutputs.erase(output);
    }
    
}

void OutputRegistry::registerOutput(const std::string& name, OutputComponent* output) 
{
    if (name.empty() || output == nullptr)
    {
        delete output;
        return;
    }

    auto it = workOutputs.find(name);
    if (it != workOutputs.end())
    {
        // Registering the same object twice must not delete it
        if (it->second != output)
        {
            delete it->second;
            it->second = output;
        }

        return;
    }
    
    workOutputs[name] = output;
}
