/**
 * @file OutputComponent.h
 * @brief Declares OutputComponent, the abstract base for every output TIVIDY produces.
 */

#ifndef OUTPUTCOMPONENT_H
#define OUTPUTCOMPONENT_H

#include <string>

/**
 * @class OutputComponent
 * @brief Abstract output of a work item, for example a report or a document a client asked for.
 * A clone is a copy, so it keeps the number of the object it was cloned from.
 */
class OutputComponent
{
public:
    /**
     * @brief Creates an output with the given name and gives it the next unique number.
     * @param name Readable name of the output, for example "Admission form".
     */
    explicit OutputComponent(const std::string& name);

    /**
     * @brief Virtual destructor so that deleting through an OutputComponent* deletes the full object.
     */
    virtual ~OutputComponent();

    /**
     * @brief Builds a printable description of this output.
     *
     * Pure virtual, so OutputComponent stays abstract
     * This is how subclasses print the private name without needing a getter.
     * @return The description.
     */
    virtual std::string printOutput() const = 0;

    /**
     * @brief Returns the unique number of this output.
     * @return The number given to this object when it was constructed or cloned.
     */
    int getOutputComponentNumber() const;

    /**
     * @brief Creates a deep copy of this output (Prototype).
     *
     * The caller owns the returned object and must delete it.
     * @return A new object of the same concrete type as this one.
     */
    virtual OutputComponent* clone() const = 0;

private:
    std::string name; ///< Name of the output.
    int outputComponentNumber; ///< Unique number of this output.
    static int outputCount; ///< Number of outputs numbered so far (shared by all outputs).
};

#endif