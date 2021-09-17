
#include <list>
#include <unordered_map>
#include <iostream>
#include <cassert>

#ifndef CACHE_HH_INCL
#define CACHE_HH_INCL

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

    size_t hitsCount_ = 0;

    T load2Cache( T_id );

public:
    Cache2Q( size_t AM_Size, size_t ALin_Size, size_t ALout_Size );
    T getPage( T_id );

    // testing stuff
    void resetHitsCount() { hitsCount_ = 0; }
    size_t getHitsCount() { return hitsCount_; }
    static void test( char * filename );

};

template <typename T, typename T_id>
Cache2Q<T, T_id>::Cache2Q( size_t AM_Size, size_t ALin_Size, size_t ALout_Size ) :
    AM_Size_ (AM_Size), ALin_Size_(ALin_Size), ALout_Size_(ALout_Size) {}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::getPage( T_id elem_id )
{
    // std::cout << "searching for " << elem_id << " element:";

    auto amHIt = AM_HashTable_.find (elem_id);
    if (amHIt != AM_HashTable_.end ()) // move elem to the head of AM
    {
        // std::cout << "AM hit" << std::endl;
        ++hitsCount_;

        auto amIt = amHIt->second;

        AM_List_.splice (AM_List_.begin (), AM_List_, amIt);

        return AM_List_.front ().first;
    }

    auto aloutHIt = ALout_HashTable_.find (elem_id);
    if (aloutHIt != ALout_HashTable_.end ()) // reclaim + add to the head of AM
    {
        // std::cout << "AL out hit" << std::endl;
        ++hitsCount_;

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
        // std::cout << "AL in hit" << std::endl;
        ++hitsCount_;

        return alinHIt->second->first;
    }

    // std::cout << "miss" << std::endl;

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

template <typename T, typename T_id>
void Cache2Q<T, T_id>::test( char * filename )
{
    assert (filename != nullptr);
    FILE * file = fopen (filename, "r");
    if (file == nullptr)
    {
        std::cout << "Can't open file " << filename << std::endl;
        return;
    }

    size_t AM_Size = 0;
    size_t ALin_Size = 0;
    size_t ALout_Size = 0;
    size_t expectedHitsNum = 0;
    size_t inputSize = 0;

    // cache sizes
    fscanf (file, " %lu", &AM_Size);
    fscanf (file, " %lu", &ALin_Size);
    fscanf (file, " %lu", &ALout_Size);

    // input info
    fscanf (file, " %lu", &expectedHitsNum);
    fscanf (file, " %lu", &inputSize);

    pageId_t * pIds = new pageId_t[inputSize];
    for (size_t i = 0; i < inputSize; ++i)
        fscanf (file, " %u", pIds + i);

    fclose (file);

    Cache2Q<Page, pageId_t> cache { AM_Size, ALin_Size, ALout_Size };
    cache.resetHitsCount ();

    for (size_t i = 0; i < inputSize; ++i)
        cache.getPage (pIds [i]);

    delete pIds;

    std::cout << "Testing with input file " << '\"' << filename << '\"' << ':';

    size_t hitsNum = cache.getHitsCount ();
    if (hitsNum != expectedHitsNum)
    {
        std::cout << "ERROR " << std::endl;
        std::cout << "EXPECTED: " << expectedHitsNum << std::endl;
        std::cout << "OUTPUT: " << hitsNum << std::endl;

        return;
    }

    std::cout << "PASSED" << std::endl;
}

} // namespace caches

#endif // #ifndef CACHE_HH_INCL