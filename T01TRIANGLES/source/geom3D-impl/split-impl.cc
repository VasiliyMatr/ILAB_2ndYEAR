
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

namespace
{
    // Removes all repeats and sorts TrsIndexes.
    void removeRepeatsNSort( TrsIndexes& );
} // namespace

TrsIndexes SplittedTrsGroup::cross()
{
    CrossBypassHandler handler {};
    leafsBypass<CrossBypassHandler> (handler);

    removeRepeatsNSort (handler.crossedIds_);
    return handler.crossedIds_;
}

template <class LeafsHandler>
void SplittedTrsGroup::leafsBypass( LeafsHandler& handler )
{
    assert (root_ != nullptr);
    SubGroup* curNode = root_;

    for (size_t i = 1; i < height_; ++i)
        curNode = curNode->children_[0];

    // Currently process nodes ids for each tree level.
    std::vector<size_t> nodesIds (height_, 0);

    while (true)
    {
        handler (curNode);

        // Seeking for next node.
        size_t seekDepth = height_ - 1;
        for (; true; --seekDepth)
        {
            curNode = curNode->parent_;
            if (++nodesIds[seekDepth] != SUB_GROUPS_NUM)
                break;
            nodesIds[seekDepth] = 0;
        }

        if (curNode == nullptr) return;

        for (; seekDepth < height_; ++seekDepth)
            curNode = curNode->children_[nodesIds[seekDepth]];
    }
}

void SplittedTrsGroup::splitGroups( const IndexedTrsGroup& group )
{
    root_ = new SubGroup;
    root_->internalTrs_ = group;
    root_->spaceDomain_ = SpaceDomain {group};

    auto handler = SplitBypassHandler {};
    for (; height_ < splitDepth_ + 1; ++height_)
        leafsBypass<SplitBypassHandler> (handler);
}

void SplittedTrsGroup::splitGroup( SubGroup* group )
{
    size_t trNum = group->internalTrs_.size ();

    PointSplitter splitter {group->internalTrs_};
    std::vector<IndexedTrsGroup> subGroups (SUB_GROUPS_NUM + 1);

    for (size_t i = 0; i < SUB_GROUPS_NUM; ++i)
    {
        SubGroup* child = new SubGroup;
        SpaceDomain curGroupSpaceDomain {};

        for (size_t j = 0; j < DNUM; ++j)
        {
            int jField = i & (1 << j);
            curGroupSpaceDomain.upper_[j] = !jField ?
                splitter[j] : group->spaceDomain_.upper_[j];
            curGroupSpaceDomain.lower_[j] = jField ?
                splitter[j] : group->spaceDomain_.lower_[j];
        }

        child->parent_ = group;
        child->spaceDomain_ = curGroupSpaceDomain;
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

void SplittedTrsGroup::calcBorders()
{
    CalcBordersBypassHandler handler {};
    leafsBypass<CalcBordersBypassHandler> (handler);
}

void SplittedTrsGroup::CrossBypassHandler::operator()( SubGroup* group )
{
    concatVectors (crossedIds_, geom3D::cross (group->internalTrs_));
    concatVectors (crossedIds_, geom3D::cross (group->borderTrs_));
    concatVectors (crossedIds_, geom3D::cross (group->borderTrs_, group->internalTrs_));
}

void SplittedTrsGroup::CalcBordersBypassHandler::operator()( SubGroup* gr )
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