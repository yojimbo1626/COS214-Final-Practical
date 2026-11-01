/**
 * @file SessionObserver.cpp
 * @brief Implements SessionObserver.
 */
#include "SessionObserver.h"

#include <ctime>

SessionObserver::SessionObserver(std::string name) : Participant(name) {}

void SessionObserver::receiveUpdate(std::string update) {
    char stamp[16] = "";
    std::time_t now = std::time(nullptr);
    std::strftime(stamp, sizeof stamp, "%H:%M:%S", std::localtime(&now));
    notifications.push_back(std::string(stamp) + "  " + update);
}

std::string SessionObserver::toString() {
    return "SessionObserver #" + std::to_string(getId()) + " (" + getName() + "): "
           + std::to_string(notifications.size()) + " notifications";
}

std::vector<std::string> SessionObserver::getNotifications() const {
    return notifications;
}
