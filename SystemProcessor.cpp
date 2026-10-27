#include "SystemProcessor.h"
#include "WorkflowController.h"

SystemProcessor::SystemProcessor(): workItem(nullptr), workflowController(nullptr), outputRegistry(nullptr) {}

SystemProcessor::~SystemProcessor() {
    delete workflowController;
}

bool SystemProcessor::addRequest(std::string request) {
    if (workflowController == nullptr) return false;
    workItemHistory->save(workItem->createSnapshot());
    return workflowController->addRequest(request);
}

void SystemProcessor::cancelWork() {
    if (workflowController == nullptr) return;
    workItemHistory->save(workItem->createSnapshot());
    workflowController->cancelWorkItem();
}

bool SystemProcessor::startWorkflow(WorkItem* workItem) {
    if (workItem == nullptr) return false;
    delete workflowController;
    this->workItem = workItem;
    workflowController = new WorkflowController(workItem);

    workItemHistory->save(workItem->createSnapshot());
    return workflowController->startWorkflow();
}

bool SystemProcessor::assignWorkItem(Worker* worker) {
    if (workflowController == nullptr) return false;
    workItemHistory->save(workItem->createSnapshot());
    return workflowController->assignWorkItem(worker);
}

bool SystemProcessor::completeWork() {
    if (workflowController == nullptr) return false;
    workItemHistory->save(workItem->createSnapshot());
    return workflowController->completeWorkItem();
}

vector<OutputComponent*> SystemProcessor::produceOutput() {
    vector<OutputComponent*> outputs;
    for (const string& request : workItem->getRequests()) 
        outputs.push_back(outputRegistry->createOutput(request));
    return outputs;
}

void SystemProcessor::printStatus() {
    if (workItem == NULL) {
        cout << "No work item available." << endl;
        return;
    }

    cout << "\n========== Work Item Status ==========" << endl;

    cout << "Name: " << workItem->getName() << endl;
    cout << "Description: " << workItem->getDescription() << endl;

    cout << "\nRequests:" << endl;
    vector<string> requests = workItem->getRequests();

    if (requests.empty()) {
        cout << "  No requests." << endl;
    } else {
        for (size_t i = 0; i < requests.size(); i++) {
            cout << "  - " << requests[i] << endl;
        }
    }

    cout << "\nClient: " << workItem->getClient()->toString() << endl; 

    cout << "\nCurrent Status: "
         << workItem->getState()->process() << endl;

    cout << "======================================" << endl;
}

void SystemProcessor::setOutputRegistry(OutputRegistry* outputRegistry) {
    this->outputRegistry = outputRegistry;
}

void SystemProcessor::undoLastAction() {
    workItem->setSnapshot(workItemHistory->undo());
}