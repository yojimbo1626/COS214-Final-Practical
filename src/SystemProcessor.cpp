#include "SystemProcessor.h"
#include "WorkItem.h"
#include "WorkflowController.h"
#include "WorkItemHistory.h"
#include "OutputRegistry.h"
#include "OutputRegistryIterator.h"
#include "OutputComponent.h"
#include "Worker.h"
#include "Client.h"

SystemProcessor::SystemProcessor()
    : workItem(nullptr), workflowController(nullptr), outputRegistry(nullptr), workItemHistory(nullptr) {}

SystemProcessor::~SystemProcessor() {
    delete workflowController;
    delete workItemHistory;
}

vector<string> SystemProcessor::getAvailableOutputs() {
    if (outputRegistry == nullptr) return vector<string>();

    vector<string> outputs;
    OutputRegistryIterator* it = outputRegistry->createIterator();

    while (it->hasNext()) {
        outputs.push_back(it->next()->printOutput());
    }

    delete it;
    return outputs;
}

bool SystemProcessor::addRequest(std::string request) {
    if (outputRegistry == nullptr || workflowController == nullptr || workItem == nullptr) 
        return false;

    OutputRegistryIterator* it = outputRegistry->createIterator();
    bool found = false;
    for (OutputComponent* component = it->first();
         component != nullptr;
         component = it->hasNext() ? it->next() : nullptr) {
        if (component->printOutput() == request) {
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
    if (workflowController == nullptr || workItem == nullptr)
        return;

    workItemHistory->save(workItem->createSnapshot());
    workflowController->cancelWorkItem();
    workItem = nullptr;
}

bool SystemProcessor::startWorkflow(WorkItem* workItem) {
    if (workItem == nullptr) return false;

    this->workItem = workItem;

    delete workflowController;
    workflowController = new WorkflowController(workItem);

    delete workItemHistory;
    workItemHistory = new WorkItemHistory;

    workItemHistory->save(workItem->createSnapshot());

    return workflowController->startWorkflow();
}

bool SystemProcessor::assignWorkItem(Worker* worker) {
    if (workflowController == nullptr || worker == nullptr || workItem == nullptr) 
        return false;

    worker->assignProcessor(this);
    workItemHistory->save(workItem->createSnapshot());
    return workflowController->assignWorkItem(worker);
}

bool SystemProcessor::completeWork() {
    if (workflowController == nullptr || workItem == nullptr)
        return false;

    workItemHistory->save(workItem->createSnapshot());

    if (!workflowController->completeWorkItem())
        return false;

    workItem = nullptr;
    return true;
}

bool SystemProcessor::processWorkItem(Worker* worker, Client* client, vector<string> requests) {
    if (!worker || !client) {
        cout << "Failed to process work item.\n";
        return false;
    }
    WorkItem* newWorkItem = new WorkItem(worker->getWorkItemName(), worker->getWorkItemDescription());

    cout << "==========\nPROCESSING WORK ITEM\n==========\n\n";
    if (!startWorkflow(newWorkItem)) {
        cout << "Failed to process work item.\n";
        workItem = nullptr;
        delete newWorkItem;
        return false;
    }
        
    if (!assignWorkItem(worker)) {
        cout << "Failed to process work item.\n";
        cancelWork();
        delete newWorkItem;
        return false;
    }

    client->requestProcessor(worker);
    for (string request : requests) {
        if (!client->addRequest(request)) {
            cout << "Failed to process work item.\n";
            cancelWork();
            delete newWorkItem;
            return false;
        }
    }
    printWorkItem();
    cout << endl;

    if (!worker->completeWorkItem()) {
        cout << "Failed to process work item.\n";
        cancelWork();
        delete newWorkItem;
        return false;
    }
    cout << "==========\n";
    delete newWorkItem;
    return true;
}

vector<OutputComponent*> SystemProcessor::produceOutput() {
    vector<OutputComponent*> outputs;
    if (workItem == nullptr || outputRegistry == nullptr) return outputs;
    for (const string& request : workItem->getRequests()) 
        outputs.push_back(outputRegistry->createOutput(request));
    return outputs;
}

void SystemProcessor::printWorkItem() {
    if (workItem == nullptr) {
        cout << "No work item available." << endl;
        return;
    }

    cout << "\n========== Work Item ==========" << endl;

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

    cout << "\nClient: ";
    if (workItem->getClient() != nullptr)
        cout << workItem->getClient()->getName() << endl;
    else
        cout << "No client assigned." << endl;

    cout << "\nCurrent Status: "
         << workItem->getState()->process() << endl;

    cout << "======================================" << endl;
}

void SystemProcessor::setOutputRegistry(OutputRegistry* outputRegistry) {
    this->outputRegistry = outputRegistry;
}