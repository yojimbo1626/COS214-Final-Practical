#ifndef CLIENT_H
#define CLIENT_H

#include "Participant.h"

#include <vector>

class Worker;
class OutputComponent;
class ClientProcessor;

class Client : public Participant {
public:
    explicit Client(std::string name);
    ~Client();

    void requestProcessor(Worker* worker);
    void viewAvailableOutputs();
    bool addRequest(std::string request);
    bool addSpecialRequest(Worker* worker, std::string output, std::vector<std::string> features);
    void receiveResults(std::vector<OutputComponent*> outputs);
    void cancelWork();

    void receiveUpdate(std::string update);
    std::string toString();
private:
    WorkflowProcessor* processor;
};

#endif
