
#include <list>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cmath>

#ifndef CACHE_HH_INCL
#define CACHE_HH_INCL

/*  TODO:

    1) Isolate pairs (list, unorderedMap) into separate class?

*/

namespace caches {

// To store element + its id.
template <typename T, typename T_id>
using EnId = typename std::pair<T, T_id>;

// Iterator type for list of EnId pairs.
template <typename T, typename T_id>
using EListIt = typename std::list<EnId<T, T_id>>::iterator;

template <typename T, typename T_id>
class MappedList
{
    std::list<EnId<T, T_id>> elemsList_;
    std::unordered_map<T_id, EListIt<T, T_id>> hashTable_;

    // Just basic functionallity.
    void pushFront( EnId<T, T_id> );
    void pushBack( EnId<T, T_id> );
    void move2Front( T_id );
    void move2Back( T_id );
    void popFront();
    void popBack();
    T front();
    T back();
};

template <typename T, typename T_id>
class CacheLRU
{
    std::list<EnId<T, T_id>> elemsList_;
    std::unordered_map<T_id, EListIt<T, T_id>> hashTable_;

    // Max cached elems num.
    const size_t capacity_;
    // Min maxSize_ value.
    static constexpr size_t MIN_CAPACITY_ = 1;
    // Currently cached elements number.
    size_t cachedElemsNum_ = 0;

public:

    CacheLRU( size_t capacity );

   ~CacheLRU() = default;
    CacheLRU( const CacheLRU& ) = default;
    CacheLRU( CacheLRU&& ) = default;
    CacheLRU& operator=( const CacheLRU& ) = default;
    CacheLRU& operator=( CacheLRU&& ) = default;

    // Searches element by it's id. Caches frequiently accessed elements.
    T getElem( T_id );
    // Forces element caching.
    void addElem( EnId<T, T_id> );
    // To check if element cached.
    bool isCached( T_id );

};

template <typename T, typename T_id>
class Cache2Q
{
    // For once accesed elements. Managed as FIFO.
    std::list<EnId<T, T_id>> alinList_; 
    std::unordered_map<T_id, EListIt<T, T_id>> alinHashTable_;

    // For way back accessed elements. Managed as FIFO.
    std::list<EnId<T, T_id>> aloutList_;
    std::unordered_map<T_id, EListIt<T, T_id>> aloutHashTable_;

    // Good proportions for lists capacities.
    static constexpr double AM_QUOTA_ = 0.25;
    static constexpr double ALIN_QUOTA_ = 0.25;
    static_assert (AM_QUOTA_ + ALIN_QUOTA_ <= 1,
        "AM_QUOTA_ + ALIN_QUOTA_ cant be above 1.0");

    // Minimum capacities for lists.
    static constexpr size_t MIN_AM_CAPACITY_ = 1;
    static constexpr size_t MIN_ALIN_CAPACITY_ = 1;
    static constexpr size_t MIN_ALOUT_CAPACITY_ = 2;

    // Public const for users to know.
public: static constexpr size_t MIN_2Q_CAPACITY_ =
    MIN_AM_CAPACITY_ + MIN_ALIN_CAPACITY_ + MIN_ALOUT_CAPACITY_;
private:

    // Lists capacities.
    const size_t amCapacity_;
    const size_t alinCapacity_;
    const size_t aloutCapacity_;

    // LRU cache to store hottest elements.
    CacheLRU<T, T_id> am_;

    // Numbers of currently cached elements.
    size_t alinCachedElemsNum_ = 0;
    size_t aloutCachedElemsNum_ = 0;

    // Loads uncached element to ALin.
    T load2Cache( T_id );

public:
    // capacity = number of elements that can be cached in memory.
    // But there is minimum capacity value.
    Cache2Q( size_t capacity );

   ~Cache2Q() = default;
    Cache2Q( const Cache2Q& ) = default;
    Cache2Q( Cache2Q&& ) = default;
    Cache2Q& operator=( const Cache2Q& ) = default;
    Cache2Q& operator=( Cache2Q&& ) = default;

    // Searches element by it's id. Caches frequiently accessed elements.
    T getElem( T_id );
};

} // namespace caches

#include "cache-impl.hh"

#endif // #ifndef CACHE_HH_INCL