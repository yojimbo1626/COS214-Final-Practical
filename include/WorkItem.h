#ifndef WORKITEM_H
#define WORKITEM_H

#include <string>
#include <vector>


#include "WorkUnit.h"

class WorkItemState;
class WorkItemSnapshot;
class Participant;
class Client;

/**
 * @brief Context in the State pattern, and Subject / ConcreteSubject in the
 *        Observer pattern.
 */   

class WorkItem : public WorkUnit {
public:
    WorkItem(std::string name, std::string description);
    WorkItem(const WorkItem& workItem);
    ~WorkItem();
    WorkItem& operator=(const WorkItem&) = delete;
  
    WorkItemSnapshot* createSnapshot();
    void setSnapshot(WorkItemSnapshot* snapshot);

    std::string getName() const;
    std::string getDescription() const;
    Client* getClient() const;
    
    std::vector<std::string> getRequests() const;
    bool addRequest(std::string request);

    // State requests: each one forwards to the current state
    bool start();
    bool assign(Participant* p);
    bool complete();
    bool cancel();
    std::string process();
    bool blocked();

    WorkItemState* getState() const;
    void setState(WorkItemState* state); // deletes the old state, then notifies

private:
    std::string name;
    std::string description;
    WorkItemState* state;
    std::vector<std::string> requests;
};

#endif
