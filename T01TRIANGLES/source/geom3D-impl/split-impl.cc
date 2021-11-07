
#include "geom3D.hh"
#include "geom3D-split.hh"

namespace geom3D
{

namespace
{

void removeRepeats( TrsIndexes& indexes )
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

SplittedTrsGroup::SplittedTrsGroup(
    const IndexedTrsGroup& group, size_t targetGroupsSize ) :
    treeHeight_ {group.size () <= targetGroupsSize ? size_t {0} :
        static_cast<size_t> (std::log2 (group.size () / targetGroupsSize) / 3 + 1)
    }

{
    splitGroups (group);
    calcBorders ();
}

TrsIndexes SplittedTrsGroup::cross()
{
    CrossBypassHandler handler {};
    leafsBypass<CrossBypassHandler> (handler);

    removeRepeats (handler.crossedIds_);
    return handler.crossedIds_;
}

template <class LeafsHandler>
void SplittedTrsGroup::leafsBypass( LeafsHandler& handler )
{
    SubGroup* curNode = root_;
    if (curNode == nullptr)
        return;

    size_t height = 0;
    for (; curNode->subGroups_[0] != nullptr; ++height)
        curNode = curNode->subGroups_[0];

    std::vector<size_t> nodesIds (height + 1, 0);

    while (true)
    {
        handler (curNode);

        // Seeking for next node.
        size_t seekHeight = height;
        bool endFlag = !seekHeight;
        for (; seekHeight != 0;)
        {
            curNode = curNode->parent_;
            if (++nodesIds[seekHeight--] != SUB_GROUPS_NUM)
                break;
            nodesIds[seekHeight + 1] = 0;
            if (seekHeight == 0)
                endFlag = true;
        }

        if (endFlag) return;

        for (; seekHeight < height; ++seekHeight)
            curNode = curNode->subGroups_[nodesIds[seekHeight + 1]];
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

TrsIndexes cross( const IndexedTrsGroup& ftG, const IndexedTrsGroup& sdG )
{
    size_t ftTrNum = ftG.size ();
    size_t sdTrNum = sdG.size ();

    std::vector<bool> ftCrossMask (ftTrNum, false);
    std::vector<bool> sdCrossMask (sdTrNum, false);

    for (size_t ftId = 0; ftId < ftTrNum; ++ftId)
    for (size_t sdId = 0; sdId < sdTrNum; ++sdId)
        if (ftG[ftId].first.crosses (sdG[sdId].first))
            ftCrossMask[ftId] = sdCrossMask[sdId] = true;

    TrsIndexes crossedIds {};

    for (size_t i = 0; i < ftTrNum; ++i)
        if (ftCrossMask[i])
            crossedIds.push_back (ftG[i].second);
    
    for (size_t i = 0; i < sdTrNum; ++i)
        if (sdCrossMask[i])
            crossedIds.push_back (sdG[i].second);

    return crossedIds;
}

void SplittedTrsGroup::splitGroups( const IndexedTrsGroup& group )
{
    root_ = new SubGroup;
    root_->internalTrs_ = group;
    root_->containingSpaceDomain_ = SpaceDomain {group};

    auto handler = SplitBypassHandler {};
    for (size_t height = 0; height < treeHeight_; ++height)
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
        child->parent_ = group;
        group->subGroups_[i] = child;
    }

    for (size_t i = 0; i < trNum; ++i)
    {
        auto currTrInfo = group->internalTrs_[i];
        auto eighth = splitter.getTriangleEighth (currTrInfo.first);
        if (eighth == SpaceOctant::SEVERAL)
            group->borderTrs_.emplace_back (currTrInfo);
        else
        group->subGroups_[static_cast<size_t> (eighth)]->internalTrs_.emplace_back (currTrInfo);
    }

    for (size_t i = 0; i < SUB_GROUPS_NUM; ++i)
    {
        SpaceDomain curGroupSD {};

        for (size_t j = 0; j < DNUM; ++j)
        {
            if (i & (1 << j))
            {
                curGroupSD.upper_[j] = splitter[j];
                curGroupSD.lower_[j] = group->containingSpaceDomain_.lower_[j];
            }
            else
            {
                curGroupSD.upper_[j] = group->containingSpaceDomain_.upper_[j];
                curGroupSD.lower_[j] = splitter[j];;
            }
        }

        group->subGroups_[i]->containingSpaceDomain_ = curGroupSD;
    }
}

void SplittedTrsGroup::calcBorders()
{
    CalcBordersBypassHandler handler {};
    leafsBypass<CalcBordersBypassHandler> (handler);
}

} // namespace geom3D