#include "WorkOutputFeature.h"

WorkOutputFeature::WorkOutputFeature(std::string name) : OutputFeature(name)
{

}

WorkOutputFeature::~WorkOutputFeature()
{
    
}

std::string WorkOutputFeature::printOutput()
{
    
    return "something"; //change
}

void WorkOutputFeature::add(OutputComponent* workOutput)
{
    
}

OutputComponent* WorkOutputFeature::clone()
{
    
    return nullptr; //change
}