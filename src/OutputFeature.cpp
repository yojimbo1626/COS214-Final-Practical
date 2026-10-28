/**
 * @file OutputFeature.cpp
 * @brief Implements OutputFeature.
 */

#include "../include/OutputFeature.h"

OutputFeature::OutputFeature(const std::string& name)
    : OutputComponent(name), workOutput(nullptr)
{
    //inline
}

OutputFeature::OutputFeature(const OutputFeature& other)
    : OutputComponent(other), workOutput(other.workOutput ? other.workOutput->clone() : nullptr)
{
    //inline
}

OutputFeature::~OutputFeature()
{
    delete workOutput;
}

std::string OutputFeature::printOutput() const
{
    if (workOutput == nullptr)
    {
        return "" ;
    }
    return workOutput->printOutput() ;
}

void OutputFeature::add(OutputComponent* newWorkOutput)
{
    // Wrapping your own class (yourself kinda) would make printOutput() recurse forever.
    if (newWorkOutput == this || newWorkOutput == workOutput) //checks if you are the same class (you are you?)
    {
        return ;
    }

    delete workOutput ;
    workOutput = newWorkOutput ;
}
