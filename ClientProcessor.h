#ifndef CLIENTPROCESSOR_H
#define CLIENTPROCESSOR_H

#include <string>
#include "WorkflowProcessor.h"

class SystemProcessor;
using namespace std;

/**
 * @class ClientProcessor
 * @brief Proxy: stands in for the SystemProcessor on behalf of a Client.
 *
 * Exposes only the operations a client is allowed to use (add a request,
 * cancel work). Anything else on the SystemProcessor is out of reach.
 */
class ClientProcessor : public WorkflowProcessor {
private:
    SystemProcessor* systemProcessor;

public:
    /**
     * @brief Creates a proxy for the given system processor.
     * @param processor The real processor to forward calls to.
     */
    explicit ClientProcessor(SystemProcessor* processor);

    /**
     * @brief Forwards a request to the real processor.
     * @param request The request text.
     * @return true on success, false otherwise.
     */
    bool addRequest(string request) override;

    /**
     * @brief Forwards a cancel call to the real processor.
     */
    void cancelWork() override;
};

#endif
