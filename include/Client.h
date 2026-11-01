#ifndef CLIENT_H
#define CLIENT_H

#include "Participant.h"

class Client : public Participant {
public:
    explicit Client(std::string name);
    ~Client();

    void receiveUpdate(std::string update);
    std::string toString();
};

#endif
