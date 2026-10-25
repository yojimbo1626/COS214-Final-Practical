#ifndef OUTPUTFEATURE_H
#define OUTPUTFEATURE_H

#include "OutputComponent.h"
#include <string>

class OutputFeature : public OutputComponent
{
protected:
    OutputComponent* workOutput;

public:
    OutputFeature(std::string name);
    virtual ~OutputFeature() = 0;

    virtual std::string printOutput() = 0;
    virtual void add(OutputComponent* workOutput) = 0;
    virtual OutputComponent* clone() = 0;
};

#endif