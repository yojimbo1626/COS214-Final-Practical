#include "ClientProcessor.h"
#include "SystemProcessor.h"

ClientProcessor::ClientProcessor(SystemProcessor* processor): systemProcessor(processor) {}

std::vector<std::string> ClientProcessor::getAvailableOutputs() {
    if (systemProcessor == nullptr) return std::vector<std::string>();
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