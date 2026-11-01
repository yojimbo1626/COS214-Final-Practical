/**
 * @file WorkOutput.h
 * @brief Declares WorkOutput, the plain (undecorated) output of a work item.
 */

#ifndef WORKOUTPUT_H
#define WORKOUTPUT_H

#include "OutputComponent.h"

/**
 * @class WorkOutput
 * @brief The basic product a client asks for, before any features are added to it.
 */
class WorkOutput : public OutputComponent
{
public:
    /**
     * @brief Creates a work output with the given name.
     *
     * Needed because OutputComponent's only constructor takes a name.
     * @param name Name of the output.
     */
    explicit WorkOutput(const std::string& name);

    /** @brief Destructor. */
    virtual ~WorkOutput();

    /**
     * @brief Describes this output.
     * @return Text in the form "Output #<number>: <name>".
     */
    virtual std::string printOutput() const;

    /**
     * @brief Copies this output (Prototype).
     * @return A new WorkOutput with the same name and number. The caller owns it.
     */
    virtual OutputComponent* clone() const;
};

#endif