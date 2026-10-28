/**
 * @file OutputComponent.cpp
 * @brief Implements OutputComponent.
 */

#include "../include/OutputComponent.h"

#include <sstream>

int OutputComponent::outputCount = 0;

OutputComponent::OutputComponent(const std::string& name)
    : name(name), outputComponentNumber(++outputCount)
{
    //inline
}

OutputComponent::~OutputComponent()
{

}

std::string OutputComponent::printOutput() const
{
    std::ostringstream out;
    out << "Output #" << outputComponentNumber << ": " << name ; //returns string of component number
    return out.str();
}

int OutputComponent::getOutputComponentNumber() const
{
    return outputComponentNumber ; //return comp num as int
}
