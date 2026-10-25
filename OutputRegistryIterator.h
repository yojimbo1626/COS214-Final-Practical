#ifndef OUTPUT_REGISTRY_ITERATOR
#define OUTPUT_REGISTRY_ITERATOR

#include "OutputComponent.h"
#include "OutputRegistry.h"

#include <string>
#include <vector>

using namespace std;


class OutputRegistryIterator{
    private:
        vector<OutputComponent*> toVisit;
        OutputRegistry* outRegistery;
        size_t current;
    public:
        OutputRegistryIterator(OutputRegistry* outReg);
        ~OutputRegistryIterator();
        bool hasNext();
        OutputComponent* next();
        OutputComponent* first();
};


#endif