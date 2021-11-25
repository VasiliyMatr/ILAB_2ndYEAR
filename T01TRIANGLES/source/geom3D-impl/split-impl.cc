
#include "geom3D-split.hh"
#include "geom3D.hh"

namespace geom3D
{

SpaceDomain::SpaceDomain(const Point &up, const Point &lo) : upper_{up}, lower_{lo}
{
    for (size_t i = 0; i < DNUM; ++i)
        if (upper_[i] < lower_[i])
        {
            lower_ = Point{};
            upper_ = Point{};
            return;
        }
}

bool SpaceDomain::crosses(const Triangle &tr) const
{
    if (!lower_.isValid() || !upper_.isValid())
        return false;

    for (size_t coordId = 0; coordId < DNUM; ++coordId)
    {
        size_t j = 0;
        for (; j < TR_POINT_NUM; ++j)
            if (fpCmpW{tr[j][coordId]} >= lower_[coordId])
                break;
        if (j == TR_POINT_NUM)
            return false;
        for (j = 0; j < TR_POINT_NUM; ++j)
            if (fpCmpW{tr[j][coordId]} <= upper_[coordId])
                break;
        if (j == TR_POINT_NUM)
            return false;
    }
    return true;
}

PointSplitter::PointSplitter(const IndexedTrsGroup &group) : Point{0, 0, 0}
{
    size_t trNum = group.size();

    for (size_t i = 0; i < trNum; ++i)
    {
        Coordinates trMassCenter = group[i].first;

        for (size_t coordId = 0; coordId < DNUM; ++coordId)
            coord_[coordId] += trMassCenter[coordId];
    }

    for (size_t i = 0; i < DNUM; ++i)
        coord_[i] /= trNum;
}

SpaceOctant PointSplitter::getOctant(const Point &P) const
{
    if (!isValid())
        return SpaceOctant::SEVERAL;

    for (size_t i = 0; i < DNUM; ++i)
        if (fpCmpW{P[i]} == coord_[i])
            return SpaceOctant::SEVERAL;

    char octant = 0;
    for (size_t i = 0; i < DNUM; ++i)
        octant += (fpCmpW{P[i]} > coord_[i]) << i;

    return SpaceOctant(octant);
}

SpaceOctant PointSplitter::getOctant(const Triangle &tr) const
{
    std::array<SpaceOctant, TR_POINT_NUM> eighths{};

    for (size_t i = 0; i < TR_POINT_NUM; ++i)
        eighths[i] = getOctant(tr[i]);

    if (eighths[0] != eighths[1] || eighths[1] != eighths[2])
        return SpaceOctant::SEVERAL;

    return eighths[0];
}

SplittedTrsGroup::SplittedTrsGroup(const IndexedTrsGroup &group, size_t targetGroupsSize)
    : splitDepth_{group.size() <= targetGroupsSize
                      ? size_t{0}
                      : static_cast<size_t>(std::log2(group.size() / targetGroupsSize) / 3 + 1)}

{
    splitGroups(group);
    calcBorders();
}

SplittedTrsGroup::~SplittedTrsGroup()
{
    for (int depth = splitDepth_; depth >= 0; --depth)
        for (DepthIter dIt{root_, static_cast<size_t>(depth)}, end = DepthIter::end(); dIt != end;)
        {
            DepthIter toDel = dIt;
            ++dIt;
            delete toDel.node_;
        }
}

namespace
{
// Removes all repeats and sorts TrsIndexes.
void removeRepeatsNSort(TrsIndexes &);
} // namespace

fp_t SplittedTrsGroup::calcСomplexityRatio() const
{
    fp_t fullCrossesNum = std::pow(root_->internalTrs_.size(), 2);
    fp_t splitCrossesNum = 0;

    for (DepthIter dIt{root_, splitDepth_}, end = DepthIter::end(); dIt != end; ++dIt)
    {
        fp_t iSz = dIt.node_->internalTrs_.size();
        fp_t bSz = dIt.node_->borderTrs_.size();

        splitCrossesNum += iSz * iSz + bSz * bSz + iSz * bSz;
    }

    return splitCrossesNum / fullCrossesNum;
}

TrsIndexes SplittedTrsGroup::cross() const
{
    TrsIndexes ids{};

// Removed in debug build for representative unit tests.
#if !DEBUG
    if (calcСomplexityRatio() < 1)
#endif
        for (DepthIter dIt{root_, splitDepth_}, end = DepthIter::end(); dIt != end; ++dIt)
        {
            const IndexedTrsGroup &intr = dIt.node_->internalTrs_;
            const IndexedTrsGroup &bord = dIt.node_->borderTrs_;

            concatVectors(ids, geom3D::cross(intr));
            concatVectors(ids, geom3D::cross(bord));
            concatVectors(ids, geom3D::cross(intr, bord));
        }
#if !DEBUG
    else
        ids = geom3D::cross(root_->internalTrs_);
#endif

    removeRepeatsNSort(ids);
    return ids;
}

void SplittedTrsGroup::splitGroups(const IndexedTrsGroup &group)
{
    root_ = new SubGroup;
    root_->internalTrs_ = group;
    root_->spaceDomain_ = SpaceDomain{group};

    for (size_t depth = 0; depth < splitDepth_; ++depth)
        for (DepthIter dIt{root_, depth}, end = DepthIter::end(); dIt != end; ++dIt)
        {
            splitGroup(dIt.node_);
        }
}

void SplittedTrsGroup::calcBorders()
{
    for (DepthIter dIt{root_, splitDepth_}, end = DepthIter::end(); dIt != end; ++dIt)
    {
        caclBorder(dIt.node_);
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
        if (++branchesIds_[seekDepth--] != SUB_GROUPS_NUM)
            break;
        branchesIds_[seekDepth + 1] = 0;
    }

    // All nodes on this level are passed.
    if (branchesIds_[0] != 0)
    {
        node_ = nullptr;
        return *this;
    }

    for (; seekDepth < depth_; ++seekDepth)
        node_ = node_->children_[branchesIds_[seekDepth + 1]];

    return *this;
}

void SplittedTrsGroup::splitGroup(SubGroup *group)
{
    size_t trNum = group->internalTrs_.size();

    PointSplitter splitter = trNum == 0 ? PointSplitter{group->spaceDomain_} : PointSplitter{group->internalTrs_};

    std::vector<IndexedTrsGroup> subGroups(SUB_GROUPS_NUM + 1);

    for (size_t i = 0; i < SUB_GROUPS_NUM; ++i)
    {
        SubGroup *child = new SubGroup;
        Point upper{};
        Point lower{};

        for (size_t coordId = 0; coordId < DNUM; ++coordId)
        {
            SpaceOctant octant = SpaceOctant(i & (1 << coordId));
            upper[coordId] = !octant ? splitter[coordId] : group->spaceDomain_.upper()[coordId];
            lower[coordId] = octant ? splitter[coordId] : group->spaceDomain_.lower()[coordId];
        }

        child->parent_ = group;
        child->spaceDomain_ = SpaceDomain{upper, lower};
        group->children_[i] = child;
    }

    for (size_t i = 0; i < trNum; ++i)
    {
        auto currTrInfo = group->internalTrs_[i];
        auto eighth = splitter.getOctant(currTrInfo.first);
        if (eighth == SpaceOctant::SEVERAL)
            group->borderTrs_.emplace_back(currTrInfo);
        else
            group->children_[eighth]->internalTrs_.emplace_back(currTrInfo);
    }

    if (group->parent_ != nullptr)
        group->internalTrs_.clear();
}

void SplittedTrsGroup::caclBorder(SubGroup *gr)
{
    IndexedTrsGroup allPossibleBorderTrs{};

    for (SubGroup *curN = gr->parent_; curN != nullptr; curN = curN->parent_)
        concatVectors(allPossibleBorderTrs, curN->borderTrs_);

    size_t trNum = allPossibleBorderTrs.size();
    for (size_t i = 0; i < trNum; ++i)
    {
        auto curTr = allPossibleBorderTrs[i];
        if (gr->spaceDomain_.crosses(curTr.first))
            gr->borderTrs_.push_back(curTr);
    }
}

TrsIndexes cross(const IndexedTrsGroup &group)
{
    size_t trNum = group.size();

    std::vector<bool> crossMask(trNum, false);

    for (size_t i = 0; i < trNum; ++i)
        for (size_t j = i + 1; j < trNum; j++)
            if (group[i].first.crosses(group[j].first))
                crossMask[i] = crossMask[j] = true;

    TrsIndexes crossedIds{};

    for (size_t i = 0; i < trNum; ++i)
        if (crossMask[i])
            crossedIds.emplace_back(group[i].second);

    return crossedIds;
}

TrsIndexes cross(const IndexedTrsGroup &ft, const IndexedTrsGroup &sd)
{
    size_t ftTrNum = ft.size();
    size_t sdTrNum = sd.size();

    std::vector<bool> ftCrossMask(ftTrNum, false);
    std::vector<bool> sdCrossMask(sdTrNum, false);

    for (size_t ftId = 0; ftId < ftTrNum; ++ftId)
        for (size_t sdId = 0; sdId < sdTrNum; ++sdId)
            if (ft[ftId].first.crosses(sd[sdId].first))
                ftCrossMask[ftId] = sdCrossMask[sdId] = true;

    TrsIndexes crossedIds{};

    for (size_t i = 0; i < ftTrNum; ++i)
        if (ftCrossMask[i])
            crossedIds.push_back(ft[i].second);

    for (size_t i = 0; i < sdTrNum; ++i)
        if (sdCrossMask[i])
            crossedIds.push_back(sd[i].second);

    return crossedIds;
}

namespace
{

void removeRepeatsNSort(TrsIndexes &indexes)
{
    size_t maxIndex = 0;
    size_t indexesNum = indexes.size();

    for (size_t i = 0; i < indexesNum; ++i)
        maxIndex = std::max(maxIndex, indexes[i]);

    std::vector<bool> mask(maxIndex + 1, false);

    for (size_t i = 0; i < indexesNum; ++i)
        mask[indexes[i]] = true;

    TrsIndexes withNoRepeats{};

    for (size_t i = 0; i < maxIndex + 1; ++i)
        if (mask[i])
            withNoRepeats.push_back(i);

    indexes = std::move(withNoRepeats);
}

} // namespace

} // namespace geom3D
