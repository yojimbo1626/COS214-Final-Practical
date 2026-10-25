#ifndef OUTPUT_REGISTRY
#define OUTPUT_REGISTRY

#include <string>
#include <map>
#include "OutputRegistryIterator.h"

using namespace std;


class OutputRegistry {
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