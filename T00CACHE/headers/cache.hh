
#include <list>
#include <unordered_map>
#include <iostream>

#include "page.hh"

namespace caches {

template <typename T, typename T_id>
class Cache2Q
{
    // to store elements + ids
    typedef typename std::pair<T, T_id> TnId;
    // to store ptr to elements + ids
    typedef typename std::pair<T*, T_id> TPnId;

    // type for iterator for list with elements
    typedef typename std::list<TnId>::iterator tListIt;
    // type for iterator for list with elements ptrs
    typedef typename std::list<TPnId>::iterator tPListIt;

    std::list<TnId> AM_List_;
    std::unordered_map<T_id, tListIt> AM_HashTable_;

    std::list<TnId> ALin_List_;
    std::unordered_map<T_id, tListIt> ALin_HashTable_;

    std::list<TPnId> ALout_List_;
    std::unordered_map<T_id, tPListIt> ALout_HashTable_;

    const size_t AM_Size_ = 0;
    const size_t ALin_Size_ = 0;
    const size_t ALout_Size_ = 0;

    size_t AM_CachedElemNum_ = 0;
    size_t ALin_CachedElemNum_ = 0;
    size_t ALout_CachedElemNum_ = 0;

    T load2Cache( T_id );

public:
    Cache2Q( size_t AM_Size, size_t ALin_Size, size_t ALout_Size );

    T getPage( T_id );
};

template <typename T, typename T_id>
Cache2Q<T, T_id>::Cache2Q( size_t AM_Size, size_t ALin_Size, size_t ALout_Size ) :
    AM_Size_ (AM_Size), ALin_Size_(ALin_Size), ALout_Size_(ALout_Size) {}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::getPage( T_id elem_id )
{
    std::cout << "searching for " << elem_id << " element:";

    auto amHIt = AM_HashTable_.find (elem_id);
    if (amHIt != AM_HashTable_.end ()) // move elem to the head of AM
    {
        std::cout << "AM hit" << std::endl;

        auto amIt = amHIt->second;

        AM_List_.splice (AM_List_.begin (), AM_List_, amIt);

        return AM_List_.front ().first;
    }

    auto aloutHIt = ALout_HashTable_.find (elem_id);
    if (aloutHIt != ALout_HashTable_.end ()) // reclaim + add to the head of AM
    {
        std::cout << "AL out hit" << std::endl;

        auto aloutIt = aloutHIt->second;

        if (AM_Size_ <= AM_CachedElemNum_)
        {
            AM_HashTable_.erase (AM_List_.back ().second);
            AM_List_.pop_back ();
            --AM_CachedElemNum_;
        }

        AM_List_.emplace_front (TnId (*(aloutIt->first), elem_id));
        AM_HashTable_[elem_id] = AM_List_.begin ();
        ++AM_CachedElemNum_;

        return AM_List_.front ().first;
    }

    auto alinHIt = ALin_HashTable_.find (elem_id);
    if (alinHIt != ALin_HashTable_.end ()) // do nothing
    {
        std::cout << "AL in hit" << std::endl;
        return alinHIt->second->first;
    }

    std::cout << "miss" << std::endl;

    // elem isn't cached => add elem to the head of ALin
    return load2Cache (elem_id);
}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::load2Cache( T_id elem_id )
{
    T elem = slowGetDataFunc (elem_id);
    if (ALin_Size_ <= ALin_CachedElemNum_)
    {
        if (ALout_Size_ <= ALout_CachedElemNum_)
        {
            ALout_HashTable_.erase (ALout_List_.back ().second);
            delete ALout_List_.back ().first;
            ALout_List_.pop_back ();
            --ALout_CachedElemNum_;
        }

        auto alinBack = ALin_List_.back ();
        ALin_HashTable_.erase (alinBack.second);
        ALin_List_.pop_back ();
        --ALin_CachedElemNum_;

        T * aloutElemPtr = new T;
        *aloutElemPtr = alinBack.first;
        ALout_List_.emplace_front (TPnId (aloutElemPtr, alinBack.second));
        ALout_HashTable_[alinBack.second] = ALout_List_.begin ();
        ++ALout_CachedElemNum_;
    }

    ALin_List_.emplace_front (TnId (elem, elem_id));
    ALin_HashTable_[elem_id] = ALin_List_.begin ();
    ++ALin_CachedElemNum_;

    return elem;
}

} // namespace caches