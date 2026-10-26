/**
 * @file WorkOutputFeature.h
 * @brief Declares WorkOutputFeature, a concrete decorator that attaches a feature to an output.
 */

#ifndef WORKOUTPUTFEATURE_H
#define WORKOUTPUTFEATURE_H

#include "OutputFeature.h"

/**
 * @class WorkOutputFeature
 * @brief Adds one feature to an output. Several can be stacked on the same output.
 */
class WorkOutputFeature : public OutputFeature
{
public:
    /**
     * @brief Creates a feature that wraps nothing yet. use add() to wrap an output.
     *
     * Needed because OutputFeature's only public constructor takes a name.
     * @param featureName Name of the feature.
     */
    explicit WorkOutputFeature(const std::string& featureName);

    /** @brief Destructor (OutputFeature deletes the wrapped output) */
    virtual ~WorkOutputFeature();

    /**
     * @brief Prints the wrapped output followed by this feature's own description.
     * @return The wrapped output's text, then " + ", then "Output #<number>: <featureName>".
     */
    virtual std::string printOutput() const;

    /**
     * @brief Sets the output this feature wraps.
     * @param workOutput The output to wrap. The feature takes ownership.
     */
    virtual void add(OutputComponent* workOutput);

    /**
     * @brief Copies this feature and the chain it wraps (Prototype).
     * @return A new WorkOutputFeature. the caller owns it.
     */
    virtual OutputComponent* clone() const;
};

#endif