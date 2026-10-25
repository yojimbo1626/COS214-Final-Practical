#include "WorkOutput.h"

WorkOutput::WorkOutput(std::string name) : OutputComponent(name)
{

}

WorkOutput::~WorkOutput()
{

}

std::string WorkOutput::printOutput()
{

    return "something"; //change later
}

OutputComponent* WorkOutput::clone()
{

    return nullptr; //change pointer later with impl
}