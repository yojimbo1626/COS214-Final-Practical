#ifndef OUTPUT_REGISTRY_H
#define OUTPUT_REGISTRY_H

#include <string>
#include <map>
#include "OutputRegistryIterator.h"
#include "Aggregate.h"

using namespace std;

class OutputRegistryIterator;
class OutputRegistry : public Aggregate {
    private:
        map<string, OutputComponent*> workOutputs;
        friend class OutputRegistryIterator;
    public:
        ~OutputRegistry();
        OutputComponent* createOutput(string output);
        void registerOutput(string name, OutputComponent* output);
        void removeOutput(string name);
        OutputRegistryIterator* createIterator();

};


#endif