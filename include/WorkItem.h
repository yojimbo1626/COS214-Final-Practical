#ifndef WORKITEM_H
#define WORKITEM_H

#include <string>

class WorkItemState;
class Participant;

/**
 * @class WorkItem
 * @brief Context in the State pattern.
 *
 * Owns its current state (deletes it on change and on destruction).
 * Only state-related members are stubbed here; requests, observers and
 * snapshot members from the full UML are left for their own patterns.
 */
class WorkItem {
public:
    WorkItem(const std::string& name, const std::string& description);
    WorkItem(const WorkItem& workItem);
    ~WorkItem();

    std::string getName() const;
    std::string getDescription() const;

    // Requests: each one forwards to the current state
    bool start();
    bool assign(Participant* p);
    bool complete();
    bool cancel();
    std::string process();

    WorkItemState* getState() const;

private:
    friend class WorkItemState;          // only the base state may call setState
    void setState(WorkItemState* state); // deletes the old state

    WorkItem& operator=(const WorkItem&); // disabled: owns a raw pointer

    std::string name;
    std::string description;
    WorkItemState* state;
};

#endif
