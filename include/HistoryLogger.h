#ifndef HISTORYLOGGER_H
#define HISTORYLOGGER_H

#include "Participant.h"
#include <string>
#include <vector>

class HistoryLogger : public Participant {
public:
    explicit HistoryLogger(std::string name);

    void receiveUpdate(std::string update);
    std::vector<std::string> getLog() const;
    std::string toString();

private:
    std::vector<std::string> log;
};

#endif
