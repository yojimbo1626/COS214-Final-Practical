#include "SystemProcessor.h"
#include "WorkflowController.h"

SystemProcessor::SystemProcessor(): workItem(nullptr), workflowController(nullptr), outputRegistry(nullptr) {}

SystemProcessor::~SystemProcessor() {
    delete workflowController;
}

bool SystemProcessor::addRequest(std::string request) {
    if (workflowController == nullptr) return false;
    return workflowController->addRequest(request);
}

void SystemProcessor::cancelWork() {
    if (workflowController == nullptr) return;
    workflowController->cancelWorkItem();
}

bool SystemProcessor::startWorkflow(WorkItem* workItem) {
    if (workItem == nullptr) return false;
    delete workflowController;
    this->workItem = workItem;
    workflowController = new WorkflowController(workItem);

    return workflowController->startWorkflow();
}

bool SystemProcessor::assignWorkItem(Worker* worker) {
    if (workflowController == nullptr) return false;
    return workflowController->assignWorkItem(worker);
}

bool SystemProcessor::completeWork() {
    if (workflowController == nullptr) return false;
    return workflowController->completeWorkItem();
}
