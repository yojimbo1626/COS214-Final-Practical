#ifndef ITERATOR_H
#define ITERATOR_H

#include "OutputComponent.h"
#include "OutputRegistry.h"

#include <string>
#include <vector>

using namespace std;

class OutputRegistry;
class Iterator {
    private:
        vector<OutputComponent*> toVisit;
        OutputRegistry* outRegistery;
        size_t current;
    public:
        Iterator(OutputRegistry* outReg);
        virtual ~Iterator();
        virtual bool hasNext() =0;
        virtual OutputComponent* next() =0;
        virtual OutputComponent* first() =0;
};


#endif