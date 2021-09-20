
#include <list>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
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

    // Type for iterator for list with elements.
    typedef typename std::list<TnId>::iterator TListIt;

    // Lists to store cached elements & their ids.
    // Stores elements that are probably hot.
    std::list<TnId> AM_List_;
    std::unordered_map<T_id, TListIt> AM_HashTable_;

    // Stores once accesed elements. Managed as FIFO.
    std::list<TnId> ALin_List_; 
    std::unordered_map<T_id, TListIt> ALin_HashTable_;

    // Stores way back accessed elements. Managed as FIFO.
    std::list<TnId> ALout_List_;
    std::unordered_map<T_id, TListIt> ALout_HashTable_;

    // Max lists sizes.
    const size_t AM_Size_ = 0;
    const size_t ALin_Size_ = 0;
    const size_t ALout_Size_ = 0;

    // Numbers of currently cached elements.
    size_t AM_CachedElemNum_ = 0;
    size_t ALin_CachedElemNum_ = 0;
    size_t ALout_CachedElemNum_ = 0;

    // Counter for number of hits in tests.
    size_t hitsCount_ = 0;

    // Loads uncached element to ALin.
    T load2Cache( T_id );
    // Counts hits for given cache size & elems ids sequence.
    static size_t countHits( size_t elemsNum, T_id elemsIds[] );

public:
    // cacheSize = number of elements that can be cached in memory.
    Cache2Q( size_t cacheSize );
    // Searches element by it's id. Caches frequiently accessed elements.
    T getPage( T_id );

    // Efficiency tests stuff
    static size_t test();
    static void test( const char * filename );
    void resetHitsCount() { hitsCount_ = 0; }
    size_t getHitsCount() { return hitsCount_; }
};

// Good proportions
constexpr double AM_QUOTA = 0.25;
constexpr double ALIN_QUOTA = 0.25;

#if AM_QUOTA + ALIN_QUOTA > 1
#error AM_QUOTA + ALIN_QUOTA cant be above 1.0
#endif

// Minimum sizes
constexpr size_t MIN_AM_SIZE = 1;
constexpr size_t MIN_ALIN_SIZE = 1;
constexpr size_t MIN_ALOUT_SIZE = 2;

template <typename T, typename T_id>
Cache2Q<T, T_id>::Cache2Q( size_t cacheSize ) :
    AM_Size_ (std::max<size_t>
        (AM_QUOTA * cacheSize, MIN_AM_SIZE)),
    ALin_Size_ (std::max<size_t>
        (ALIN_QUOTA * cacheSize, MIN_ALIN_SIZE)),
    ALout_Size_ (std::max<size_t>
        (cacheSize - AM_Size_ - ALin_Size_, MIN_ALOUT_SIZE))
{}

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

        AM_List_.emplace_front (*aloutIt);
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
            ALout_HashTable_.erase (ALout_List_.back ().second);
            ALout_List_.pop_back ();
            --ALout_CachedElemNum_;
        }

        auto alinBack = ALin_List_.back ();
        ALin_HashTable_.erase (alinBack.second);
        ALin_List_.pop_back ();
        --ALin_CachedElemNum_;

        ALout_List_.emplace_front (alinBack);
        ALout_HashTable_[alinBack.second] = ALout_List_.begin ();
        ++ALout_CachedElemNum_;
    }

    // placing cached element in ALin
    ALin_List_.emplace_front (TnId {elem, elem_id});
    ALin_HashTable_[elem_id] = ALin_List_.begin ();
    ++ALin_CachedElemNum_;

    return elem;
}

template <typename T, typename T_id>
void Cache2Q<T, T_id>::test( const char * filename )
{
    assert (filename != nullptr);

    std::ifstream in(filename);
    if (!in.is_open ())
    {
        std::cout << "Cannot open file " << filename <<std::endl;
        return;
    }
    std::streambuf * cinbuf = std::cin.rdbuf ();
    std::cin.rdbuf (in.rdbuf ());

    size_t hitsNum = test ();
    size_t expectedHitsNum = 0;
    std::cin >> expectedHitsNum;

    std::cin.rdbuf (cinbuf);

    static const char FAIL_COLOR_ON[] = "\033[0;31m";
    static const char PASS_COLOR_ON[] = "\033[0;32m";
    static const char RESET_COLOR[] = "\033[0m";

    std::cout << "Testing with input file " << '\"' << filename << '\"' << std::endl;
    if (expectedHitsNum != hitsNum)
    {
        std::cout << FAIL_COLOR_ON;
        std::cout << "Failed: ";
        std::cout << "expeced " << expectedHitsNum << " hits, ";
        std::cout << "but got " << hitsNum << " hits" << std::endl;
        std::cout << RESET_COLOR;
        return;
    }
    std::cout << PASS_COLOR_ON;
    std::cout << "Passed: " <<"Hits number = " << hitsNum << std::endl;
    std::cout << RESET_COLOR;
}

template <typename T, typename T_id>
size_t Cache2Q<T, T_id>::test()
{
    size_t cacheSize = 0;
    size_t inputSize = 0;

    std::cin >> cacheSize >> inputSize;

    pageId_t * pIds = new pageId_t[inputSize];
    for (size_t i = 0; i < inputSize; ++i)
        std::cin >> pIds[i];

    Cache2Q<Page, pageId_t> cache { cacheSize };
    cache.resetHitsCount ();

    for (size_t i = 0; i < inputSize; ++i)
        cache.getPage (pIds [i]);

    delete pIds;

    return cache.getHitsCount ();
}

} // namespace caches

#endif // #ifndef CACHE_HH_INCL