/**
 * @file SessionObserver.h
 * @brief The GUI's own Observer: collects every update the work items push.
 */
#ifndef SESSION_OBSERVER_H
#define SESSION_OBSERVER_H

#include <string>
#include <vector>

#include "Participant.h"

/**
 * @class SessionObserver
 * @brief ConcreteObserver (Observer pattern) that feeds the GUI's Notifications panel.
 *
 * Every WorkItem in a workflow subscribes one SessionObserver, next to its Client, its
 * Worker and the HistoryLogger. When a WorkItem changes state it pushes the update to all
 * of them, and this observer keeps a time-stamped copy for the page to show. It replaces
 * reading the engine's console output.
 */
class SessionObserver : public Participant {
public:
    /** @param name Display name, e.g. "GUI notifications". */
    explicit SessionObserver(std::string name);

    /**
     * @brief Records an update pushed by a WorkItem (push model).
     * @param update The message, e.g. "WorkItem 'Party order' is now Assigned".
     */
    void receiveUpdate(std::string update);

    /** @return "SessionObserver #id (name): n notifications". */
    std::string toString();

    /** @return Every update received so far, oldest first, each starting with the time. */
    std::vector<std::string> getNotifications() const;

private:
    std::vector<std::string> notifications;
};

#endif