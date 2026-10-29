#ifndef WORKITEM_H
#define WORKITEM_H

#include <string>
#include <vector>

class WorkItemState;
class Participant;

/**
 * @brief Context in the State pattern, and Subject / ConcreteSubject in the
 *        Observer pattern.
 */
class WorkItem {
public:
    WorkItem(std::string name, std::string description);
    WorkItem(const WorkItem& workItem);
    ~WorkItem();

    std::string getName() const;
    std::string getDescription() const;

    // State requests: each one forwards to the current state
    bool start();
    bool assign(Participant* p);
    bool complete();
    bool cancel();
    std::string process();
    bool blocked();

    WorkItemState* getState() const;
    void setState(WorkItemState* state); // deletes the old state, then notifies

    // Observer
    void addObserver(Participant* participant);
    void removeObserver(Participant* participant);
    void notify();

private:
    std::string name;
    std::string description;
    WorkItemState* state;
    std::vector<Participant*> observers;
};

#endif
