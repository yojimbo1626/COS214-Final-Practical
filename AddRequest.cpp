#include "AddRequest.h"
#include "WorkItem.h"

AddRequest::AddRequest(WorkItem* workItem, const std::string& request)
    : Command(workItem), request(request) {}

bool AddRequest::execute() {
    if (workItem == nullptr) return false;
    return workItem->addRequest(request);
}

void AddRequest::defineRequest(const std::string& request) {
    this->request = request;
}
