#include "../include/OutputRegistryIterator.h"
#include "../include/OutputRegistry.h"

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

OutputComponent* OutputRegistryIterator::next() {
    if (toVisit.empty()) {
        return nullptr;
    }

    if (!hasNext()) {
        return toVisit[0];
    }
    return toVisit[++current];
}