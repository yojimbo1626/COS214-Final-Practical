#include "ClientProcessor.h"
#include "SystemProcessor.h"

ClientProcessor::ClientProcessor(SystemProcessor* processor): systemProcessor(processor) {}

bool ClientProcessor::addRequest(std::string request) {
    if (systemProcessor == nullptr) return false;
    return systemProcessor->addRequest(request);
}

void ClientProcessor::cancelWork() {
    if (systemProcessor == nullptr) return;
    systemProcessor->cancelWork();
}
