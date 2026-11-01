#ifndef CLIENT_H
#define CLIENT_H

#include "Participant.h"

#include <vector>

class Worker;
class OutputComponent;
class ClientProcessor;
class WorkflowProcessor;

/**
 * @class Client
 * @brief The participant who asks for work and receives the results.
 *
 * Pattern roles:
 * - Proxy: client of ClientProcessor (it only sees the WorkflowProcessor interface).
 * - Observer: ConcreteObserver (receives pushed state changes of its work item).
 */
class Client : public Participant {
public:
    /** @param name Display name. */
    explicit Client(std::string name);
    /** @brief Deletes the client's ClientProcessor, if it still has one. */
    ~Client();

    /** @brief Asks the worker for a ClientProcessor (Proxy) for the worker's current work item. */
    void requestProcessor(Worker* worker);
    /** @brief Prints the outputs the client can request. */
    void viewAvailableOutputs();
    /** @return true if the request was added through the proxy; false if there is no proxy or it is unknown. */
    bool addRequest(std::string request);
    /**
     * @brief Requests a product decorated with extra features (Decorator).
     * @return true if the decorated product was registered and requested.
     */
    bool addSpecialRequest(Worker* worker, std::string output, std::vector<std::string> features);
    /** @brief Receives (prints and then deletes) the finished outputs, and drops the proxy. */
    void receiveResults(std::vector<OutputComponent*> outputs);
    /** @brief Cancels the work item through the proxy. Does nothing without a proxy. */
    void cancelWork();

    /** @brief Observer: prints the pushed update. @param update Text describing the change. */
    void receiveUpdate(std::string update);
    /** @return "Client #id (name)". */
    std::string toString();
private:
    WorkflowProcessor* processor;   ///< The client's proxy (owned), or nullptr.
};

#endif
