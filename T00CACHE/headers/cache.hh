
#include <list>
#include <unordered_map>
#include <iostream>

#include "page.hh"

template <typename T, typename T_id>
class Cache2Q
{
    // type for iterator for list with elements
    typedef typename std::list<T>::iterator tListIt;
    // type for iterator for list with elements ptrs
    typedef typename std::list<*T>::iterator tPListIt;

    // to know new pacement (choosen list) for reclaimed page.
    enum class slotPlacement_t
    {
        UNDEF_  ,
        AM_     ,
        ALIN_
    };

    std::list<T> AM_List_;
    std::unordered_map<T_id, tListIt> AM_HashTable_;

    std::list<T> ALin_List_;
    std::unordered_map<T_id, tListIt> ALin_HashTable_;

    std::list<*T> ALout_List_;
    std::unordered_map<T_id, tPListIt> ALout_HashTable_;

    const size_t AM_Size_ = 0;
    const size_t ALin_Size_ = 0;
    const size_t ALout_Size_ = 0;

    size_t AM_CachedElemNum_ = 0;
    size_t ALin_CachedElemNum_ = 0;
    size_t ALout_CachedElemNum_ = 0;

    Cache2Q( size_t AM_Size, size_t ALin_Size, size_t ALout_Size );
    tListIt reclaim( T_id, slotPlacement_t & sp );

public:
    T getPage( T_id );
};

template <typename T, typename T_id>
Cache2Q<T, T_id>::Cache2Q( size_t AM_Size, size_t ALin_Size, size_t ALout_Size ) :
    AM_Size_ (AM_Size), ALin_Size_(ALin_Size), ALout_Size_(ALout_Size) {}

template <typename T, typename T_id>
T Cache2Q<T, T_id>::getPage( T_id elem_id )
{
    auto am_It = AM_HashTable_.find (elem_id);

    if (am_It != AM_HashTable_.end ()) // move elem to the head of AM
    {
        AM_List_.splice (AM_List_.begin (), AM_List_, am_It);

        return AM_List_.front ();
    }

    auto alout_It = ALout_HashTable_.find (elem_id);
    if (alout_It != ALout_HashTable_.end ()) // reclaim + add to the head of AM
    {
        slotPlacement_t sp = slotPlacement_t::UNDEF_;
        auto recl_It = reclaim (elem_id, &sp);
        // TODO: normal check
        assert (sp != slotPlacement_t::UNDEF_);

        if (sp == slotPlacement_t::AM_)
            AM_List_.splice (AM_List_.begin (), AM_List_, recl_It);

        else
        {
            if (AM_CachedElemNum_ < AM_Size_)
            {
                AM_List.splice (AM_List_.begin (), ALin_List_, recl_It);
                ++AM_CachedElemNum_;
            }

            else
            {
                AM_HashTable_.erase (AM_List_.back.id_);
                ALin_List_.splice (ALin_List_.begin (), AM_List_, AM_List_.end ()--);
                ALin_HashTable_[AM_List_.back.id_] = ALin_List_.begin ();
                ++ALin_CachedElemNum_;

                AM_List.splice (AM_List_.begin (), ALin_List_, recl_It);
            }
        }

        AM_HashTable_[elem_id] = AM_List_.begin ();

        return *alout_It;
    }

    auto alin_It = ALin_HashTable_.find (elem_id);
    if (alin_It != ALin_HashTable_.end ()) // do nothing
    {
        return *alin_It;
    }

    // elem isn't cached => add elem to the head of ALin
    slotPlacement_t sp = slotPlacement_t::UNDEF_;
    auto recl_It = reclaim (elem_id, &sp);
    // TODO: normal check
    assert (sp != slotPlacement_t::UNDEF_);

    if (sp == slotPlacement_t::ALIN_)
        ALin_List_.splice (ALin_List_.begin (), ALin_List_, recl_It);

    else // ...


    return slowGetDataFunc (elem_id);
}
