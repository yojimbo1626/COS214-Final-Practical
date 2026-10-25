#ifndef WORKOUTPUT_H
#define WORKOUTPUT_H

#include "OutputComponent.h"
#include <string>

class WorkOutput : public OutputComponent
{
public:
    WorkOutput(std::string name);
    ~WorkOutput();

    std::string printOutput();
    OutputComponent* clone();
};

#endif