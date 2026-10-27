#include "../include/OutputRegistryIterator.h"

OutputRegistryIterator::OutputRegistryIterator(OutputRegistry* outReg) {
    
    for (const auto& temp : outReg->workOutputs) {
        toVisit.push_back(temp.second);
    }
    this->current = 0;
}

bool OutputRegistryIterator::hasNext() {
    if (current+1 >= toVisit.size()) {
        return false;
    }
    return true;
}

OutputComponent* OutputRegistryIterator::first() {
    current = 0;
    return toVisit[0];
}

OutputComponent* OutputRegistryIterator::next() {
    if (!hasNext()) {
        return toVisit[0];
    }
    return toVisit[++current];
}