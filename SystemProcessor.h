#ifndef SYSTEMPROCESSSOR_H
#define SYSTEMPROCESSSOR_H
#include <vector>

#include <string>
#include "Worker.h"
#include "WorkItem.h"
#include "OutputRegistry.h"
#include "OutputComponent.h"
#include "WorkFlowController.h"

using namespace std;

class SystemProcessor{
private:
    WorkItem* workItem;
    WorkflowController* workflowController;
    OutputRegistry* outputRegistry;
public:
    SystemProcessor();
    bool addRequest(string request);
    void cancelWork();
    bool startWorkflow(WorkItem* workItem);
    bool assignWorkItem(Worker* worker);
    bool completeWork();
    vector<OutputComponent*> produceOutput();
    void printStatus();
    void setOutputRegistry(OutputRegistry* outputRegistry);
    void saveWorkItemProgress();
    void undoLastAction();
};

#endif
