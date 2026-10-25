#ifndef WORKOUTPUTFEATURE_H
#define WORKOUTPUTFEATURE_H

#include "OutputFeature.h"
#include <string>

class WorkOutputFeature : public OutputFeature {
public:
    WorkOutputFeature(std::string name);
    ~WorkOutputFeature();

    std::string printOutput();
    void add(OutputComponent* workOutput);
    OutputComponent* clone();
};

#endif