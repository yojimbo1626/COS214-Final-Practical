/**
 * @file WorkOutputFeature.cpp
 * @brief Implements WorkOutputFeature.
 */

#include "WorkOutputFeature.h"

WorkOutputFeature::WorkOutputFeature(const std::string& featureName)
    : OutputFeature(featureName)
{
    //inline
}

WorkOutputFeature::~WorkOutputFeature()
{

}

std::string WorkOutputFeature::printOutput() const
{
    // wrapped output print first, then add this feature's own output to it
    return OutputFeature::printOutput() + " + " + OutputComponent::printOutput() ;
}

void WorkOutputFeature::add(OutputComponent* workOutput)
{
    OutputFeature::add(workOutput) ;
}

OutputComponent* WorkOutputFeature::clone() const
{
    // calls deep copy constructor
    return new WorkOutputFeature(*this) ;
}
