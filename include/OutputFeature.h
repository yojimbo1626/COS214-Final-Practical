/**
 * @file OutputFeature.h
 * @brief Declares OutputFeature, the abstract decorator for outputs.
 */

#ifndef OUTPUTFEATURE_H
#define OUTPUTFEATURE_H

#include "OutputComponent.h"

/**
 * @class OutputFeature
 * @brief Abstract decorator that adds a feature to another output without changing its class.
 *
 * An OutputFeature wraps zero or one OutputComponent (multiplicity 0..1)
 */
class OutputFeature : public OutputComponent
{
public:
    /**
     * @brief Creates a feature that wraps nothing yet. use add() to wrap output.
     *
     * Needed because OutputComponent's only constructor takes a name.
     * @param name Name of the feature.
     */
    explicit OutputFeature(const std::string& name);

    /** @brief Deletes the wrapped output and rest of the chain. */
    virtual ~OutputFeature();

    /**
     * @brief Pure virtual, with a body that forwards to the wrapped output.
     *
     * Concrete decorators call OutputFeature::printOutput() and add their own text.
     * @return The wrapped output's description, or an empty string if nothing is wrapped yet.
     */
    virtual std::string printOutput() const = 0;

    /**
     * @brief Pure virtual, with a body that sets the wrapped output.
     *
     * Any output wrapped before is deleted. Wrapping the feature itself is ignored,
     * because printOutput() would then call itself forever.
     * @param workOutput The output to wrap. The feature takes ownership.
     */
    virtual void add(OutputComponent* newWorkOutput) = 0;

    /**
     * @brief Copies this feature and everything it wraps (Prototype).
     * @return A new object, the caller owns it.
     */
    virtual OutputComponent* clone() const = 0;

protected:
    /**
     * @brief Deep copy constructor used by clone(). the copy gets its own clone of the wrapped output.
     * 
     * @param other The feature being copied.
     */
    OutputFeature(const OutputFeature& other);

private:
    /** @brief Assignment is disabled for the same reason the copy constructor is written by hand. */
    OutputFeature& operator=(const OutputFeature&);

    OutputComponent* workOutput; ///< The wrapped output (owned, may be nullptr).
};

#endif