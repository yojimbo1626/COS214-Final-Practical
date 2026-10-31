#include "../include/WorkflowController.h"

#include "../include/Command.h"
#include "../include/AddRequest.h"
#include "../include/StartWorkflow.h"
#include "../include/AssignWorkItem.h"
#include "../include/CompleteWorkItem.h"
#include "../include/CancelWorkItem.h"
#include "../include/WorkItem.h"
#include "../include/Worker.h"

WorkflowController::WorkflowController(WorkItem* workItem)
    : workItem(workItem),
      addRequestCommand(new AddRequest(workItem, "")),
      start(new StartWorkflow(workItem)),
      assign(new AssignWorkItem(workItem)),
      complete(new CompleteWorkItem(workItem)),
      cancel(new CancelWorkItem(workItem)) {}

WorkflowController::~WorkflowController() {
    delete addRequestCommand;
    delete start;
    delete assign;
    delete complete;
    delete cancel;
}

bool WorkflowController::addRequest(std::string request) {
    static_cast<AddRequest*>(addRequestCommand)->defineRequest(request);
    return addRequestCommand->execute();
}

bool WorkflowController::startWorkflow() {
    return start->execute();
}

bool WorkflowController::assignWorkItem(Worker* worker) {
    if (workItem == nullptr || worker == nullptr) return false;

    workItem->addObserver(worker);

    bool assigned = assign->execute();
    if (!assigned) {
        workItem->removeObserver(worker);
    }
    return assigned;
}

bool WorkflowController::completeWorkItem() {
    return complete->execute();
}

bool WorkflowController::cancelWorkItem() {
    return cancel->execute();
}
