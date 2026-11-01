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

std::vector<string> SystemProcessor::getAvailableOutputs() {
    if (outputRegistry == nullptr) 
        return std::vector<string>();

    std::vector<string> outputs;
    Iterator<OutputComponent>* it = outputRegistry->createIterator();

    for (OutputComponent* component = it->first();
         component != nullptr;
         component = it->hasNext() ? it->next() : nullptr) {
        outputs.push_back(component->printOutput());
    }

    delete it;
    return outputs;
}

bool SystemProcessor::addRequest(std::string request) {
    if (outputRegistry == nullptr || workflowController == nullptr || workItem == nullptr)
        return false;

    OutputComponent* output = outputRegistry->createOutput(request);

    if (output == nullptr)
        return false;

    delete output;

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

bool SystemProcessor::processWorkItem(Worker* worker, Client* client, std::vector<std::string> requests) {
    if (!worker || !client) {
        std::cout << "Failed to process work item.\n";
        return false;
    }
    WorkItem* newWorkItem = new WorkItem(worker->getWorkItemName(), worker->getWorkItemDescription());

    std::cout << "==========\nPROCESSING WORK ITEM\n==========\n\n";
    if (!startWorkflow(newWorkItem)) {
        std::cout << "Failed to process work item.\n";
        workItem = nullptr;
        delete newWorkItem;
        return false;
    }
        
    if (!assignWorkItem(worker)) {
        std::cout << "Failed to process work item.\n";
        cancelWork();
        delete newWorkItem;
        return false;
    }

    client->requestProcessor(worker);
    for (std::string request : requests) {
        if (!client->addRequest(request)) {
            std::cout << "Failed to process work item.\n";
            cancelWork();
            delete newWorkItem;
            return false;
        }
    }
    printWorkItem();
    std::cout << endl;

    if (!worker->completeWorkItem()) {
        std::cout << "Failed to process work item.\n";
        cancelWork();
        delete newWorkItem;
        return false;
    }
    std::cout << "==========\n";
    delete newWorkItem;
    return true;
}

std::vector<OutputComponent*> SystemProcessor::produceOutput() {
    std::vector<OutputComponent*> outputs;
    if (workItem == nullptr || outputRegistry == nullptr) return outputs;
    for (const string& request : workItem->getRequests()) 
        outputs.push_back(outputRegistry->createOutput(request));
    return outputs;
}

void SystemProcessor::printWorkItem() {
    if (workItem == nullptr) {
        std::cout << "No work item available." << endl;
        return;
    }

    std::cout << "\n========== Work Item ==========" << endl;

    std::cout << "Name: " << workItem->getName() << endl;
    std::cout << "Description: " << workItem->getDescription() << endl;

    std::cout << "\nRequests:" << endl;
    std::vector<string> requests = workItem->getRequests();

    if (requests.empty()) {
        std::cout << "  No requests." << endl;
    } else {
        for (size_t i = 0; i < requests.size(); i++) {
            std::cout << "  - " << requests[i] << endl;
        }
    }

    std::cout << "\nClient: ";
    if (workItem->getClient() != nullptr)
        std::cout << workItem->getClient()->getName() << endl;
    else
        std::cout << "No client assigned." << endl;

    std::cout << "\nCurrent Status: "
         << workItem->getState()->process() << endl;

    std::cout << "======================================" << endl;
}

void SystemProcessor::setOutputRegistry(OutputRegistry* outputRegistry) {
    this->outputRegistry = outputRegistry;
}