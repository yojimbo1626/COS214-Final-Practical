#include "ClientProcessor.h"
#include "SystemProcessor.h"

ClientProcessor::ClientProcessor(SystemProcessor* processor): systemProcessor(processor) {}

vector<string> CLientProcessor::getAvailableOutputs() {
    if (systemProcessor == nullptr) return vector<string>();
    return systemProcessor->getAvailableOutputs();
}

bool ClientProcessor::addRequest(std::string request) {
    if (systemProcessor == nullptr) return false;
    return systemProcessor->addRequest(request);
}

void ClientProcessor::cancelWork() {
    if (systemProcessor == nullptr) return;
    systemProcessor->cancelWork();
}
