#include "../include/OutputRegistry.h"

OutputRegistryIterator* OutputRegistry::createIterator() {
    return new OutputRegistryIterator(this);
}

OutputComponent* OutputRegistry::createOutput(string output) {
    outputCom = new OutputComponent(output);
    return outputCom.clone();
}

void OutputRegistry::removeOutput(string output) {
    workOutputs.erase(output);
}

OutputComponent* OutputRegistry::registerOutput(string name, OutputComponent* out) {
    workOutputs.insert({name,out});
}