
#ifndef CACHE_IMPL_HH_INCL
#define CACHE_IMPL_HH_INCL

namespace caches
{

template <typename T, typename T_id>
CacheLRU<T, T_id>::CacheLRU( size_t capacity ) :
    capacity_(std::max<size_t> (capacity, MIN_CAPACITY_))
{}

template <typename T, typename T_id>
T CacheLRU<T, T_id>::getElem( T_id id )
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

        T elem {id};

        elemsList_.push_front (EnId<T, T_id> {elem, id});
        hashTable_ [id] = elemsList_.begin ();

        ++cachedElemsNum_;

        return elem;
    }

    elemsList_.splice (elemsList_.begin (), elemsList_, hashIt->second);

    return elemsList_.front ().first;
}

template <typename T, typename T_id>
void CacheLRU<T, T_id>::addElem( EnId<T, T_id> pair )
{
    if (isCached (pair.second)) // Remove old value.
    {
        auto hashIt = hashTable_.find (pair.second);
        auto oldPairListIt = hashIt->second;
        hashTable_.erase (oldPairListIt->second);
        elemsList_.erase (oldPairListIt);
    }

    else if (cachedElemsNum_ >= capacity_) // Free space.
    {
        hashTable_.erase (elemsList_.back ().second);
        elemsList_.pop_back ();
    }

    elemsList_.emplace_front (pair);
    hashTable_[pair.second] = elemsList_.begin ();
}

template <typename T, typename T_id>
bool CacheLRU<T, T_id>::isCached( T_id id ) // const
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
        (capacity - amCapacity_ - alinCapacity_, MIN_ALOUT_CAPACITY_)),
    am_ (amCapacity_)
{}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::getElem( T_id id )
{
    if (am_.isCached (id))
        return am_.getElem (id);

    auto aloutHashIt = aloutHashTable_.find (id);
    // Move element form alout to the head of am.
    if (aloutHashIt != aloutHashTable_.end ())
    {
        auto aloutIt = aloutHashIt->second;

        am_.addElem (*aloutIt);

        return aloutIt->first;
    }

    auto alinHashIt = alinHashTable_.find (id);
    // Do nothing.
    if (alinHashIt != alinHashTable_.end ())
        return alinHashIt->second->first;

    // Element isn't cached => load element to the head of alin.
    return load2Cache (id);
}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::load2Cache( T_id id )
{
    T elem {id};

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

} // namespace caches

#endif // #ifndef CACHE_IMPL_HH_INCL