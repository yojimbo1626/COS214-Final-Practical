/**
    * @class Iterator.h
    * @brief the abstract Iterator class. 
    * @note it is templatized; it follows a similar method to that done in the textbook

*/



#ifndef ITERATOR_H
#define ITERATOR_H

#include <string>
#include <vector>


/**
    @tparam Item is the object type to be iterated through
*/
template <class Item>
class Iterator {
    public:
        Iterator();
        virtual ~Iterator();
        virtual bool hasNext() =0;
        virtual Item* next() =0;
        virtual Item* first() =0;
};


#endif