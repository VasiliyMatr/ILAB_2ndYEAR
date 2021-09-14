
#include <list>
#include <unordered_map>
#include <iostream>

#include "page.hh"

template <typename T, typename T_id>
struct Cache
{
    typedef typename std::list<T>::iterator tListIt;

    std::list<T> list_;
    std::unordered_map<T_id, tListIt> hashTable_;

    const size_t cacheSize_ = 0;
    size_t cachedElemsNum_ = 0;

    Cache( size_t cacheSize );
    T getPage( T_id );
};

template <typename T, typename T_id>
Cache<T, T_id>::Cache( size_t cacheSize ) : cacheSize_ (cacheSize) {}

template <typename T, typename T_id>
T Cache<T, T_id>::getPage( T_id elem_id )
{
    auto hIter = hashTable_.find (elem_id);

    if (hIter == hashTable_.end ()) // miss
    {
        std::cout << "miss " << elem_id << std::endl;

        T elem = slowGetDataFunc (elem_id);

        if (cachedElemsNum_ < cacheSize_) // have free cache
        {
            ++cachedElemsNum_;
            list_.push_front (elem);
            hashTable_[elem_id] = list_.begin ();
            return elem;
        }

        hashTable_.erase (list_.back ().id_);
        list_.pop_back ();
        list_.push_front (elem);
        hashTable_[elem_id] = list_.begin ();
        return elem;
    }

    std::cout << "hit  " << elem_id << std::endl;

    auto lIter = hIter->second;
    hashTable_.erase (hIter);
    T elem = *lIter;
    list_.erase (lIter);
    list_.push_front (elem);
    hashTable_[elem_id] = list_.begin ();
    return elem;
}
