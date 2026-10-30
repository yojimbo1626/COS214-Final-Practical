/**
    * @class OutputRegistryIterator.cpp
    * @brief This is the ConcreteIterator interface which iterates through the ConcreteAggregate OutputRegistry

    * @date 29 Oct, 17:11
*/


#ifndef OUTPUT_REGISTRY_ITERATOR_H
#define OUTPUT_REGISTRY_ITERATOR_H

#include "OutputComponent.h"
#include "Iterator.h"

#include <string>
#include <vector>

class OutputRegistry;

/**
    * @note uses type OutputComponent because this is for the PrototypeManager

*/
class OutputRegistryIterator : public Iterator<OutputComponent> {
    private:
        std::vector<OutputComponent*> toVisit;
        size_t current;
    public:
        OutputRegistryIterator(OutputRegistry* outReg);
        ~OutputRegistryIterator() override;


        /**
            * @brief These traversal methods go through the vector without removing it. Going next from the last item goes back to the start of the vector.
            * @return Returns the OutputComponent which has been or can be cloned, or null if vector is empty
        */
        bool hasNext() override;
        OutputComponent* next() override;
        OutputComponent* first() override;
};


#endif