#ifndef OUTPUTCOMPONENT_H
#define OUTPUTCOMPONENT_H

#include <string>

class OutputComponent
{
private:
    std::string name;
    int outputComponentNumber;
    static int outputCount;

public:
    OutputComponent(std::string name);
    virtual ~OutputComponent() = 0;

    virtual std::string printOutput() = 0;
    int getOutputComponentNumber();
    virtual OutputComponent* clone() = 0;
};

#endif