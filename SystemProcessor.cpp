#include "SystemProcessor.h"
#include "WorkflowController.h"
#include "OutputRegistry.h"
#include "OutputRegistryIterator.h"
#include "OutputComponent.h"

SystemProcessor::SystemProcessor()
    : workItem(nullptr), workflowController(nullptr), outputRegistry(nullptr) {}

SystemProcessor::~SystemProcessor() {
    delete workflowController;
}

vector<string> SystemProcessor::getAvailableOutputs() {
    if (outputRegistry == nullptr) return vector<string>();

    vector<string> outputs;
    OutputRegistryIterator* it = outputRegistry->createIterator();

    while (it->hasNext()) {
        outputs.push_back(it->next()->toString());
    }

    delete it;
    return outputs;
}

bool SystemProcessor::addRequest(std::string request) {
    if (outputRegistry == nullptr || workflowController == nullptr) return false;

    OutputRegistryIterator* it = outputRegistry->createIterator();
    bool found = false;
    for (OutputComponent* component = it->first();
         component != nullptr;
         component = it->hasNext() ? it->next() : nullptr) {
        if (component->toString() == request) {
            found = true;
            break;
        }
    }
    delete it;

    if (!found) return false;

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
    worker->assignProcessor(this);
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

void SystemProcessor::printWorkItem() {
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