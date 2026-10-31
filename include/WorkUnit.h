#ifndef WorkUnit_H
#define WorkUnit_H

#include <string>
#include <vector>

class Participant;

/**
 * @class WorkUnit
 * @brief Abstract WorkUnit in the Observer pattern.
 *
 * Keeps the list of observers and pushes updates to them.
 */
class WorkUnit {
public:
    void addObserver(Participant* participant);
    void removeObserver(Participant* participant);
    void notify(std::string update);
    virtual ~WorkUnit() = 0;

protected:
    std::vector<Participant*> observers;
};

#endif
