
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
    // To store elements + ids.
    typedef typename std::pair<T, T_id> TnId;
    // To store ptr to elements + ids.
    typedef typename std::pair<T*, T_id> TPnId;

    // Type for iterator for list with elements.
    typedef typename std::list<TnId>::iterator tListIt;
    // Type for iterator for list with elements ptrs.
    typedef typename std::list<TPnId>::iterator tPListIt;

    // AM & ALin stores elements.
    // ALout stores only ptrs to elements, stored in memory.

    // Stores elements that are probably hot.
    std::list<TnId> AM_List_;
    std::unordered_map<T_id, tListIt> AM_HashTable_;

    // Stores once accesed elements. Managed as FIFO.
    std::list<TnId> ALin_List_; 
    std::unordered_map<T_id, tListIt> ALin_HashTable_;

    // Stores ptrs to way back accessed elements. Managed as FIFO.
    std::list<TPnId> ALout_List_;
    std::unordered_map<T_id, tPListIt> ALout_HashTable_;

    // Max lists sizes.
    const size_t AM_Size_ = 0;
    const size_t ALin_Size_ = 0;
    const size_t ALout_Size_ = 0;

    // Numbers of currently cached elements.
    size_t AM_CachedElemNum_ = 0;
    size_t ALin_CachedElemNum_ = 0;
    size_t ALout_CachedElemNum_ = 0;

    // To count number of hits for tests.
    size_t hitsCount_ = 0;

    // Loads uncached element to ALin.
    T load2Cache( T_id );

public:
    // cacheSize = number of elements that can be cached in memory.
    Cache2Q( size_t cacheSize );
    // Searches element by it's id. Caches frequiently accessed elements.
    T getPage( T_id );

    // Testing stuff
    void resetHitsCount() { hitsCount_ = 0; }
    size_t getHitsCount() { return hitsCount_; }
    // Tests
    static void test( char * filename );
    // Counts hits for given cache size & elems ids sequence.
    static size_t countHits( size_t elemsNum, T_id elemsIds[] );

};

template <typename T, typename T_id>
Cache2Q<T, T_id>::Cache2Q( size_t cacheSize )
{
    // ?safe?
    constexpr double AM_QUOTA = 0.25;
    constexpr double ALIN_QUOTA = 0.25;
    #error AM_QUOTA + ALIN_QUOTA > 1.0

    AM_Size_ = AM_QUOTA * cacheSize;
    ALin_Size_ = ALIN_QUOTA * cacheSize;
    ALout_Size_ = cacheSize - AM_Size_ - ALin_Size_;
}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::getPage( T_id elem_id )
{
    auto amHIt = AM_HashTable_.find (elem_id);
    // move elem to the head of AM
    if (amHIt != AM_HashTable_.end ())
    {
        ++hitsCount_;

        auto amIt = amHIt->second;

        AM_List_.splice (AM_List_.begin (), AM_List_, amIt);

        return AM_List_.front ().first;
    }

    auto aloutHIt = ALout_HashTable_.find (elem_id);
    // move element form memory to element slot
    if (aloutHIt != ALout_HashTable_.end ())
    {
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
    // do nothing
    if (alinHIt != ALin_HashTable_.end ())
    {
        ++hitsCount_;

        return alinHIt->second->first;
    }

    // elem isn't cached => load elem to the head of ALin
    return load2Cache (elem_id);
}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::load2Cache( T_id elem_id )
{
    T elem = slowGetDataFunc (elem_id);

    // need to free space in ALin
    if (ALin_Size_ <= ALin_CachedElemNum_)
    {
        // need to free space in ALout
        if (ALout_Size_ <= ALout_CachedElemNum_)
        {
            auto aloutBack = ALout_List_.back ();
            ALout_HashTable_.erase (aloutBack.second);
            delete aloutBack.first;
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

    // placing cached element in ALin
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