#include "OutputRegistry.h"

OutputRegistryIterator* OutputRegistry::createIterator() {
    return new OutputRegistryIterator(this);
}

OutputComponent* OutputRegistry::createOutput(string output) {

}

void OutputRegistry::removeOutput(string output) {
    workOutputs.erase(output);
}

OutputComponent* OutputRegistry::registerOutput(string name, OutputComponent* out) {
    workOutputs.insert({name,out});
}