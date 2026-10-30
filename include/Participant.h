#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <string>

class Participant {
public:
    explicit Participant(std::string name);
    virtual ~Participant();

    int getId() const;
    std::string getName() const;

    virtual void receiveUpdate(std::string update) = 0;
    virtual std::string toString() = 0;

private:
    int id;
    std::string name;
    static int idCounter;
};

#endif
