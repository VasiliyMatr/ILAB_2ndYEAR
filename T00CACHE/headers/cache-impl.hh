
// For calm code IDE parsing without 'errors' highlighting.
#include "cache.hh"

namespace caches
{

template <typename T, typename T_id>
CacheLRU<T, T_id>::CacheLRU( size_t capacity ) :
    capacity_(std::max<size_t> (capacity, MIN_CAPACITY_))
{}

template <typename T, typename T_id>
T CacheLRU<T, T_id>::getPage( T_id id )
{
    auto hashIt = hashTable_.find (id);

    if (hashIt == hashTable_.end ()) // Not cached element.
    {
        if (cachedElemsNum_ >= capacity_)
        {
            hashTable_.erase (elemsList_.back ().second);
            elemsList_.pop_back ();

            --cachedElemsNum_;
        }

        T elem = slowGetDataFunc (id);

        elemsList_.push_front (EnId<T, T_id> {elem, id});
        hashTable_ [id] = elemsList_.begin ();

        ++cachedElemsNum_;

        return elem;
    }

    elemsList_.splice (elemsList_.begin (), elemsList_, hashIt.second);

    return elemsList_.front ();
}

template <typename T, typename T_id>
void CacheLRU<T, T_id>::addPage( EnId<T, T_id> pair )
{
    if (isCached (id)) // Remove old value.
    {
        auto hashIt = hashTable_.find (pair.second);
        EnId<T, T_id> oldId = hashIt.second->second;
        hashTable_.erase (oldId);
        elemsList_.erase (hashIt.second);
    }

    else if (cachedElemsNum_ >= capacity_) // Free space.
    {
        hashTable_.erase (elemsList_.back.second);
        elemsList_.pop_back ();
    }

    elemsList_.emplace_front (pair.first);
    hashTable_[pair.second] = elemsList_.begin ();
}

template <typename T, typename T_id>
bool CacheLRU<T, T_id>::isCached( T_id id )
{
    return hashTable_.find (id) != hashTable_.end ();
}

template <typename T, typename T_id>
Cache2Q<T, T_id>::Cache2Q( size_t capacity ) :
    amCapacity_ (std::max<size_t>
        (std::trunc (AM_QUOTA_ * capacity), MIN_AM_CAPACITY_)),
    alinCapacity_ (std::max<size_t>
        (std::trunc (ALIN_QUOTA_ * capacity), MIN_ALIN_CAPACITY_)),
    aloutCapacity_ (std::max<size_t>
        (capacity - amapacity_ - alinCapacity_, MIN_ALOUT_CAPACITY_)),
    am_{ amCapacity_ }
{}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::getPage( T_id id )
{
    if (am_.isCached (id))
        return am_.getPage (id);

    auto aloutHashIt = aloutHashTable_.find (id);
    // Move element form alout to the head of am.
    if (aloutHashIt != aloutHashTable_.end ())
    {
        ++hitsCount_;

        auto aloutIt = aloutHashIt->second;

        am_.addPage (*aloutIt);

        return aloutIt->first;
    }

    auto alinHashIt = alinHashTable_.find (id);
    // Do nothing.
    if (alinHashIt != alinHashTable_.end ())
    {
        ++hitsCount_;

        return alinHashIt->second->first;
    }

    // Element isn't cached => load element to the head of alin.
    return load2Cache (id);
}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::load2Cache( T_id id )
{
    T elem = slowGetDataFunc (id);

    // Need to free space in alin.
    if (alinCapacity_ <= alinCachedElemsNum_)
    {
        // Need to free space in alout.
        if (aloutCapacity_ <= aloutCachedElemsNum_)
        {
            aloutHashTable_.erase (aloutList_.back ().second);
            aloutList_.pop_back ();
            --aloutCachedElemsNum_;
        }

        auto alinBack = alinList_.back ();
        alinHashTable_.erase (alinBack.second);
        alinList_.pop_back ();
        --alinCachedElemsNum_;

        aloutList_.emplace_front (alinBack);
        aloutHashTable_[alinBack.second] = aloutList_.begin ();
        ++aloutCachedElemsNum_;
    }

    // Placing new element in alin.
    alinList_.emplace_front (EnId<T, T_id> {elem, id});
    alinHashTable_[id] = alinList_.begin ();
    ++alinCachedElemsNum_;

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

    static const char SET_FAIL_COLOR[] = "\033[0;31m";
    static const char SET_PASS_COLOR[] = "\033[0;32m";
    static const char RESET_COLOR[] = "\033[0m";

    std::cout << "Testing with input file " << '\"' << filename << '\"' << std::endl;
    if (expectedHitsNum != hitsNum)
    {
        std::cout << SET_FAIL_COLOR;
        std::cout << "Failed: ";
        std::cout << "expeced " << expectedHitsNum << " hits, ";
        std::cout << "but got " << hitsNum << " hits" << std::endl;
        std::cout << RESET_COLOR;
        return;
    }
    std::cout << SET_PASS_COLOR;
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

    delete [] pIds;

    return cache.getHitsCount ();
}

} // namespace caches