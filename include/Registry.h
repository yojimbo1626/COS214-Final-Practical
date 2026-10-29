/**
 * @file Registry.h
 * @brief Declares Registry, the interface for collections of outputs that can be traversed.
 */

#ifndef REGISTRY_H
#define REGISTRY_H

class Iterator;

/**
 * @class Registry
 * @brief Any collection of outputs that hands out an Iterator over its contents.
 *
 * Registry only declares createIterator(). Concrete registries decide how they store their outputs,
 * so callers can go through any of them without knowing the storage structure.
 * Managing prototypes is not part of this interface, that is OutputRegistry's job.
 */
class Registry
{
public:
    /** @brief Virtual destructor so that deleting through a Registry* deletes the full object. */
    virtual ~Registry();

    /**
     * @brief Creates an iterator positioned at the first output of this registry.
     * @return A new iterator, the caller owns it and must delete it.
     */
    virtual Iterator* createIterator() = 0;
};

#endif