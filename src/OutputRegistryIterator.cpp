/**
    * @class OutputRegistryIterator.cpp
    * @brief Implementation of the ConcreteIterator

    * @note since OutputRegistry is a PrototypeManager, this class may behave differently from future potential ConcreteIterators

    * @date 29 Oct, 17:11
*/


#include "../include/OutputRegistryIterator.h"



OutputRegistryIterator::OutputRegistryIterator(OutputRegistry* outReg) {
    
    for (const auto& temp : outReg->workOutputs) {
        toVisit.push_back(temp.second);
    }
    this->current = 0;
}

OutputRegistryIterator::~OutputRegistryIterator() {}

bool OutputRegistryIterator::hasNext() {
    if (current+1 >= toVisit.size()) {
        return false;
    }
    return true;
}

OutputComponent* OutputRegistryIterator::first() {
    current = 0;
    if (toVisit.empty()) {
        return nullptr;
    }
    return toVisit[0];
}


/**
    * @brief loops through vector without removing/modifying
    * @return if hasNext is false, then we return the first item in the vector. If vector is empty, return nullptr

*/

OutputComponent* OutputRegistryIterator::next() {
    if (toVisit.empty()) {
        return nullptr;
    }

    if (!hasNext()) {
        return toVisit[0];
    }
    return toVisit[++current];
}