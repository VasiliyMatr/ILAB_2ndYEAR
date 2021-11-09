
#include "geom3D.hh"
#include "geom3D-split.hh"

namespace geom3D
{

SplittedTrsGroup::SplittedTrsGroup(
    const IndexedTrsGroup& group, size_t targetGroupsSize ) :
    splitDepth_ {group.size () <= targetGroupsSize ? size_t {0} :
        static_cast<size_t> (std::log2 (group.size () / targetGroupsSize) / 3 + 1)
    }

{
    splitGroups (group);
    calcBorders ();
}

SplittedTrsGroup::~SplittedTrsGroup()
{
    for (int depth = splitDepth_; depth >= 0; --depth)
    for (DepthIter dIt {root_, static_cast<size_t> (depth)},
         end = DepthIter::end (); dIt != end;)
    {
        DepthIter toDel = dIt;
        ++dIt;
        delete toDel.node_;
    }
}

namespace
{
    // Removes all repeats and sorts TrsIndexes.
    void removeRepeatsNSort( TrsIndexes& );
} // namespace

TrsIndexes SplittedTrsGroup::cross() const
{
    TrsIndexes ids {};

    for (DepthIter dIt {root_, splitDepth_}, end = DepthIter::end ();
         dIt != end; ++dIt)
    {
        const IndexedTrsGroup& intr = dIt.node_->internalTrs_;
        const IndexedTrsGroup& bord = dIt.node_->borderTrs_;

        concatVectors (ids, geom3D::cross (intr));
        concatVectors (ids, geom3D::cross (bord));
        concatVectors (ids, geom3D::cross (intr, bord));
    }

    removeRepeatsNSort (ids);
    return ids;
}

void SplittedTrsGroup::splitGroups( const IndexedTrsGroup& group )
{
    root_ = new SubGroup;
    root_->internalTrs_ = group;
    root_->spaceDomain_ = SpaceDomain {group};

    for (size_t depth = 0; depth < splitDepth_; ++depth)
    for (DepthIter dIt {root_, depth}, end = DepthIter::end ();
         dIt != end; ++dIt)
    {
        splitGroup (dIt.node_);
    }
}

void SplittedTrsGroup::calcBorders()
{
    for (DepthIter dIt {root_, splitDepth_}, end = DepthIter::end ();
         dIt != end; ++dIt)
    {
        caclBorder (dIt.node_);
    }
}

SplittedTrsGroup::DepthIter SplittedTrsGroup::DepthIter::operator++()
{
    if (node_ == nullptr)
        return *this;

    size_t seekDepth = depth_;
    while (true)
    {
        node_ = node_->parent_;
        if (++branchesIds[seekDepth--] != SUB_GROUPS_NUM)
            break;
        branchesIds[seekDepth + 1] = 0;
    }

    // All nodes on this level are passed.
    if (branchesIds[0] != 0)
    {
        node_ = nullptr;
        return *this;
    }

    for (; seekDepth < depth_; ++seekDepth)
        node_ = node_->children_[branchesIds[seekDepth + 1]];

    return *this;
}

void SplittedTrsGroup::splitGroup( SubGroup* group )
{
    size_t trNum = group->internalTrs_.size ();

    PointSplitter splitter {group->internalTrs_};
    std::vector<IndexedTrsGroup> subGroups (SUB_GROUPS_NUM + 1);

    for (size_t i = 0; i < SUB_GROUPS_NUM; ++i)
    {
        SubGroup* child = new SubGroup;
        Point upper {};
        Point lower {};

        for (size_t j = 0; j < DNUM; ++j)
        {
            int jField = i & (1 << j);
            upper[j] = !jField ?
                splitter[j] : group->spaceDomain_.upper ()[j];
            lower[j] = jField ?
                splitter[j] : group->spaceDomain_.lower ()[j];
        }

        child->parent_ = group;
        child->spaceDomain_ = SpaceDomain {upper, lower};
        group->children_[i] = child;
    }

    for (size_t i = 0; i < trNum; ++i)
    {
        auto currTrInfo = group->internalTrs_[i];
        auto eighth = splitter.getOctant (currTrInfo.first);
        if (eighth == SpaceOctant::SEVERAL)
            group->borderTrs_.emplace_back (currTrInfo);
        else
            group->children_[eighth]->internalTrs_.emplace_back (currTrInfo);
    }

    group->internalTrs_.clear ();
}

void SplittedTrsGroup::caclBorder( SubGroup* gr )
{
    IndexedTrsGroup allPossibleBorderTrs {};

    for (SubGroup* curN = gr->parent_; curN != nullptr; curN = curN->parent_)
        concatVectors (allPossibleBorderTrs, curN->borderTrs_);

    size_t trNum = allPossibleBorderTrs.size ();
    for (size_t i = 0; i < trNum; ++i)
    {
        auto curTr = allPossibleBorderTrs[i];
        if (gr->spaceDomain_.crosses (curTr.first))
            gr->borderTrs_.push_back (curTr);
    }
}

TrsIndexes cross( const IndexedTrsGroup& group )
{
    size_t trNum = group.size ();

    std::vector<bool> crossMask (trNum, false);

    for (size_t i = 0; i < trNum; ++i)
    for (size_t j = i + 1; j < trNum; j++)
        if (group[i].first.crosses (group[j].first))
            crossMask [i] = crossMask [j] = true;

    TrsIndexes crossedIds {};

    for (size_t i = 0; i < trNum; ++i)
        if (crossMask[i])
            crossedIds.emplace_back (group[i].second);

    return crossedIds;
}

TrsIndexes cross( const IndexedTrsGroup& ft, const IndexedTrsGroup& sd )
{
    size_t ftTrNum = ft.size ();
    size_t sdTrNum = sd.size ();

    std::vector<bool> ftCrossMask (ftTrNum, false);
    std::vector<bool> sdCrossMask (sdTrNum, false);

    for (size_t ftId = 0; ftId < ftTrNum; ++ftId)
    for (size_t sdId = 0; sdId < sdTrNum; ++sdId)
        if (ft[ftId].first.crosses (sd[sdId].first))
            ftCrossMask[ftId] = sdCrossMask[sdId] = true;

    TrsIndexes crossedIds {};

    for (size_t i = 0; i < ftTrNum; ++i)
        if (ftCrossMask[i]) crossedIds.push_back (ft[i].second);

    for (size_t i = 0; i < sdTrNum; ++i)
        if (sdCrossMask[i]) crossedIds.push_back (sd[i].second);

    return crossedIds;
}

namespace
{

void removeRepeatsNSort( TrsIndexes& indexes )
{
    size_t maxIndex = 0;
    size_t indexesNum = indexes.size ();

    for (size_t i = 0; i < indexesNum; ++i)
        maxIndex = std::max (maxIndex, indexes[i]);

    std::vector<bool> mask (maxIndex + 1, false);

    for (size_t i = 0; i < indexesNum; ++i)
        mask[indexes[i]] = true;

    TrsIndexes withNoRepeats {};

    for (size_t i = 0; i < maxIndex + 1; ++i)
        if (mask[i]) withNoRepeats.push_back (i);

    indexes = std::move (withNoRepeats);
}

} // namespace

} // namespace geom3D