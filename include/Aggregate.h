#ifndef AGGREGATE_H
#define AGGREGATE_H

#include <string>
#include <map>
#include "OutputComponent.h"

using namespace std;

class OutputRegistryIterator;
class Aggregate {
    private:
        map<string, OutputComponent*> workOutputs;
        friend class OutputRegistryIterator;
    public:
        virtual ~Aggregate();
        virtual OutputComponent* createOutput(string output) =0;
        virtual void registerOutput(string name, OutputComponent* output) =0;
        virtual void removeOutput(string name) =0;
        virtual OutputRegistryIterator* createIterator() =0;

};


#endif