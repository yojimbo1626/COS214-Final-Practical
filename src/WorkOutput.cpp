/**
 * @file WorkOutput.cpp
 * @brief Implements WorkOutput.
 */

#include "../include/WorkOutput.h"

WorkOutput::WorkOutput(const std::string& name)
    : OutputComponent(name)
{
    //inline
}

WorkOutput::~WorkOutput()
{

}

std::string WorkOutput::printOutput() const
{
    return OutputComponent::printOutput();
}

OutputComponent* WorkOutput::clone() const
{
    return new WorkOutput(*this); //clone deep copy
}
