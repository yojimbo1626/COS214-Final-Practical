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
    WorkItem(const std::string& name, const std::string& description);
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

    WorkItemState* getState() const;

    // Observer
    void addObserver(Participant* participant);
    void removeObserver(Participant* participant);
    void notify();

private:
    friend class WorkItemState;          // only the base state may call setState
    void setState(WorkItemState* state); // deletes the old state, then notifies

    WorkItem& operator=(const WorkItem&); // disabled: owns a raw pointer

    std::string name;
    std::string description;
    WorkItemState* state;
    std::vector<Participant*> observers;
};

#endif
