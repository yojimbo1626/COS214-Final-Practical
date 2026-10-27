#ifndef OUTPUT_REGISTRY_ITERATOR_H
#define OUTPUT_REGISTRY_ITERATOR_H

#include "OutputComponent.h"
#include "OutputRegistry.h"
#include "Iterator.h"

#include <string>
#include <vector>

using namespace std;

class OutputRegistry;
class OutputRegistryIterator : public Iterator{
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