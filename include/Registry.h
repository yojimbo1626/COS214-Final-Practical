/**
 * @file Registry.h
 * @brief Declares Registry, the interface for collections of outputs that can be traversed.
 */


#ifndef REGISTRY_H
#define REGISTRY_H

#include "Iterator.h"

template <class Item>
class Registry {
    public:
        virtual ~Registry() = default;
        virtual Iterator<Item>* createIterator() =0;

};


#endif