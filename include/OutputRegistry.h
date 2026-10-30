/**
 * @file OutputRegistry.h
 * @brief Declares OutputRegistry, which stores named output prototypes and clones them on request.
 *
 * our Pattern roles:
 * Prototype: PrototypeManager (keeps the prototypes and hands out clones of them)
 * Iterator: ConcreteAggregate , it implements Registry::createIterator()
 */

#ifndef OUTPUTREGISTRY_H
#define OUTPUTREGISTRY_H

#include "Registry.h"
#include "OutputComponent.h"
#include <map>
#include <string>

class OutputComponent;
class OutputRegistryIterator;

/**
 * @class OutputRegistry
 * @brief Keeps one prototype per output name and hands out copies of it.
 *
 * The registry owns every prototype registered with it and deletes them when it is destroyed
 * or when they are replaced or removed. Objects returned by createOutput() are copies owned by
 * the caller, so changing one never changes the stored prototype or any other copy.
 *
 * We deceded that a registry cannot be copied or assigned: a copy would share the same prototypes, and both
 * registries would delete them.
 */
class OutputRegistry : public Registry<OutputComponent>
{
    private: /// Prototypes by their name.
        /// the Registry owns and deletes every OutputComponent thats stored here
        std::map<std::string, OutputComponent*> workOutputs;

        /// the Iterator reeds workOutputs directly to build its list of prototypes to look for.
        friend class OutputRegistryIterator;

    public:
        /** @brief Creates an empty registry */
        OutputRegistry() = default;

        /**
         * @brief We disabled Copying: a copy would share this registry's prototypes, and both registries would delete them, which would be a double delete
         */
        OutputRegistry(const OutputRegistry&) = delete;

        /**
         * @brief We Disabled Assignment for the same reason as Copying. It will have also leaked the prototypes the target registry already owns
         */
        OutputRegistry& operator=(const OutputRegistry&) = delete;

        /** @brief Deletes every stored prototype */
        ~OutputRegistry() override;

        /**
         * @brief Creates a new output by cloning the prototype registered under its given name
         * @param output Name of the prototype to clone.
         * @return A new copy owned by the caller (that must delete it), or nullptr if no prototype has that name.
         */
        OutputComponent* createOutput(const std::string& output);

        /**
         * @brief Stores a prototype under a name, replacing (and deleting) any prototype already stored there.
         * @param name Name to store the prototype under. Must not be empty.
         * @param output The prototype. The registry takes ownership of it. If name is empty or output is nullptr, nothing is stored and output is deleted.
         */
        void registerOutput(const std::string& name, OutputComponent* output);

        /**
         * @brief it Removes and deletes the prototype stored under a name. Does nothing if the name is unknown
         * @param name Name of the prototype to remove.
         * @warning An iterator created before this call still points to the deleted prototype, so create a new one.
         */
        void removeOutput(const std::string& name);

        /**
         * @brief Creates an iterator for browsing the prototypes in this registry (Iterator pattern)
         * @return A new OutputRegistryIterator, returned through the Iterator interface.
         * @warning unique_ptr is not used, so the caller owns the iterator and must delete it.
         */
        Iterator<OutputComponent>* createIterator() override;
};

#endif