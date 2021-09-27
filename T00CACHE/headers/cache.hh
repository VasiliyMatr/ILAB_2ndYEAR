
#include <list>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cmath>

#ifndef CACHE_HH_INCL
#define CACHE_HH_INCL

#include "page.hh"

/*  TODO:

    1) Separate tests & Cache2Q class.
    2) Remove names with word "page".
    3) Add slowGetDataFunc as template param.
    4) Isolate pairs (list, unorderedMap) into separate class.

*/

namespace caches {

// To store element + its id.
template <typename T, typename T_id>
using EnId = typename std::pair<T, T_id>;

// Iterator type for list of EnId pairs.
template <typename T, typename T_id>
using EListIt = typename std::list<EnId<T, T_id>>::iterator;

// TODO:
template <typename T, typename T_id>
class MappedList
{

};

template <typename T, typename T_id>
class CacheLRU
{
    std::list<EnId<T, T_id>> elemsList_;
    std::unordered_map<T_id, EListIt<T, T_id>> hashTable_;

    // Max cached elems num.
    const size_t capacity_;
    // Min maxSize_ value.
    static const size_t MIN_CAPACITY_ = 1;
    // Currently cached elements number.
    size_t cachedElemsNum_ = 0;

public:

    CacheLRU( size_t capacity );

   ~CacheLRU() = default;
    CacheLRU( const CacheLRU& ) = delete;
    CacheLRU( const CacheLRU&& ) = delete;
    CacheLRU operator=( const CacheLRU& ) = delete;
    CacheLRU operator=( const CacheLRU&& ) = delete;

    // Searches element by it's id. Caches frequiently accessed elements.
    T getPage( T_id );
    // Forces element caching.
    void addPage( EnId<T, T_id> );
    // To check if element cached.
    bool isCached( T_id );

};

template <typename T, typename T_id>
class Cache2Q
{
    // LRU cache to store hottest elements
    CacheLRU<T, T_id> am_;

    // For once accesed elements. Managed as FIFO.
    std::list<EnId<T, T_id>> alinList_; 
    std::unordered_map<T_id, EListIt<T, T_id>> alinHashTable_;

    // For way back accessed elements. Managed as FIFO.
    std::list<EnId<T, T_id>> aloutList_;
    std::unordered_map<T_id, EListIt<T, T_id>> aloutHashTable_;

    // Good proportions for lists capacities.
    static const double AM_QUOTA_ = 0.25;
    static const double ALIN_QUOTA_ = 0.25;
    static_assert (AM_QUOTA_ + ALIN_QUOTA_ <= 1,
        "AM_QUOTA_ + ALIN_QUOTA_ cant be above 1.0");

    // Minimum capacities for lists.
    static const size_t MIN_AM_CAPACITY_ = 1;
    static const size_t MIN_ALIN_CAPACITY_ = 1;
    static const size_t MIN_ALOUT_CAPACITY_ = 2;

    // Public const for users to know.
public: static const size_t MIN_2Q_CAPACITY_ =
    MIN_AM_CAPACITY_ + MIN_ALIN_CAPACITY_ + MIN_ALOUT_CAPACITY_;
private:

    // Lists capacities.
    const size_t alinCapacity_ = 0;
    const size_t aloutCapacity_ = 0;

    // Numbers of currently cached elements.
    size_t alinCachedElemsNum_ = 0;
    size_t aloutCachedElemsNum_ = 0;

    // Counter for number of hits in tests.
    size_t hitsCount_ = 0;

    // Loads uncached element to ALin.
    T load2Cache( T_id );

public:
    // capacity = number of elements that can be cached in memory.
    // But there is minimum capacity value.
    Cache2Q( size_t capacity );

   ~Cache2Q() = default;
    Cache2Q( const Cache2Q& ) = delete;
    Cache2Q( const Cache2Q&& ) = delete;
    Cache2Q operator=( const Cache2Q& ) = delete;
    Cache2Q operator=( const Cache2Q&& ) = delete;

    // Searches element by it's id. Caches frequiently accessed elements.
    T getPage( T_id );

    // Efficiency tests stuff.
    // To test with data from stdin. Format for data:
    //     <CACHE CAPACITY> <SEQUENCE SIZE> <SEQUENCE>
    //
    //     Returns number of hits for given inputs.
    static size_t test();

    // To test with data from file. Format for test data:
    //     <CACHE CAPACITY> <SEQUENCE SIZE> <SEQUENCE> <EXPECTED NUM OF HITS>
    //
    //     Prints test result in stdin.
    static void test( const char * filename );
    void resetHitsCount() { hitsCount_ = 0; }
    size_t getHitsCount() const { return hitsCount_; }
};

} // namespace caches

#include "cache-impl.hh"

#endif // #ifndef CACHE_HH_INCL