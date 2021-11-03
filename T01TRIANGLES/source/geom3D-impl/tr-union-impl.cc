
#include "geom3D.hh"

namespace geom3D
{

namespace
{

template<class Data>
void concatVectors( std::vector<Data>& dest, const std::vector<Data>& src )
    { dest.insert (dest.end (), src.begin (), src.end ()); }

void removeRepeats( TrIndexes& indexes )
{
    size_t maxIndex = 0;
    size_t indexesNum = indexes.size ();

    for (size_t i = 0; i < indexesNum; ++i)
        maxIndex = std::max (maxIndex, indexes[i]);

    std::vector<bool> mask (maxIndex + 1, false);

    for (size_t i = 0; i < indexesNum; ++i)
        mask[indexes[i]] = true;

    TrIndexes withNoRepeats {};

    for (size_t i = 0; i < maxIndex + 1; ++i)
        if (mask[i]) withNoRepeats.push_back (i);

    indexes = std::move (withNoRepeats);
}

} // namespace

OrganizedTriangles::OrganizedTriangles(
    IndexedTrGroup&& toCapture, size_t targetGroupsSize ) :
    internalTrGroups_ {toCapture},
    splitDepth_ { static_cast<size_t> (
        std::log10 (static_cast<fp_t> (internalTrGroups_[0].size ()) / targetGroupsSize) * 8/10 + 1
    )} // TODO!

    { splitGroups (); }

namespace
{

template<class Compare> // Callable to compare coords
struct PointBound : Point
{
    void extend( const PointBound& sd )
    {
        for (size_t i = 0; i < DNUM; ++i)
            coord_[i] = Compare{} (coord_[i], sd.coord_[i]);
    }

    PointBound( const Point& P ) : Point {P} {}

    PointBound( const Triangle& tr ) : Point {tr[0]}
    {
        this->extend (tr[1]);
        this->extend (tr[2]);
    }

    PointBound( const IndexedTrGroup& group ) : Point {}
    {
        size_t groupSize = group.size ();

        if (groupSize == 0) return;

        *this = PointBound {group[0].first};

        for (size_t i = 1; i < groupSize; ++i)
            this->extend (group[i].first);
    }
};

struct UpperBoundComparator
{
    fp_t operator()( fp_t ft, fp_t sd ) const
        { return std::max (ft, sd); }
};

struct LowerBoundComparator
{
    fp_t operator()( fp_t ft, fp_t sd ) const
        { return std::min (ft, sd); }
};

using UpperBound = PointBound<UpperBoundComparator>;
using LowerBound = PointBound<LowerBoundComparator>;

using Bounds = std::pair<UpperBound, LowerBound>;

bool checkPointAffinity( const Bounds& bounds, const Point& P )
{
    bool result = true;

    for (size_t i = 0; i < DNUM; ++i)
    {
        result = result && fpCmpW {bounds.first[i]} >= P[i] &&
                           fpCmpW {bounds.second[i]} <= P[i];
    }

    return result;
}

} // namespace


TrIndexes OrganizedTriangles::cross() const
{
    size_t groupsNum = internalTrGroups_.size ();
    size_t bordersNum = borderTrGroups_.size ();
    TrIndexes crossedTrIds {};

    for (size_t i = 0; i < groupsNum; ++i)
    {
        TrIndexes selfCrossedTrIds = cross (internalTrGroups_[i]);
        concatVectors (crossedTrIds, std::move (selfCrossedTrIds));

        IndexedTrGroup currBorderTrs {};
        IndexedTrGroup currPossibleBorderTrs {};
        size_t passedBordIds = 0;

        for (size_t j = groupsNum; j != 1; j /= 8)
        {
            IndexedTrGroup borderToAdd = borderTrGroups_[i / j + passedBordIds];

            concatVectors (currPossibleBorderTrs, borderToAdd);
            passedBordIds += groupsNum / j;
        }

        size_t possibleBorderTrsNum = currPossibleBorderTrs.size ();
        Bounds bounds {internalTrGroups_[i], internalTrGroups_[i]};
        for (size_t i = 0; i < possibleBorderTrsNum; ++i)
        {
            auto currTr = currPossibleBorderTrs[i];

            for (size_t j = 0; j < 3; ++j)
                if (checkPointAffinity (bounds, currTr.first[i]))
                {
                    currBorderTrs.push_back (currTr);
                    break;
                }
        }

        TrIndexes bordCrossedTrIds = cross (currBorderTrs, internalTrGroups_[i]);

        concatVectors (crossedTrIds, std::move (bordCrossedTrIds));
    }

    IndexedTrGroup allBorderTrs {};

    for (size_t i = 0; i < bordersNum; ++i)
        concatVectors (allBorderTrs, borderTrGroups_[i]);

    TrIndexes selfCrossedTrIds = cross (allBorderTrs);
    concatVectors (crossedTrIds, std::move (selfCrossedTrIds));

    removeRepeats (crossedTrIds);

    return crossedTrIds;
}

TrIndexes OrganizedTriangles::cross ( const IndexedTrGroup& group )
{
    size_t trNum = group.size ();

    std::vector<bool> crossMask (trNum, false);

    for (size_t i = 0; i < trNum; ++i)
    for (size_t j = i + 1; j < trNum; j++)
        if (group[i].first.crosses (group[j].first))
            crossMask [i] = crossMask [j] = true;

    TrIndexes crossedIds {};

    for (size_t i = 0; i < trNum; ++i)
        if (crossMask[i])
            crossedIds.emplace_back (group[i].second);

    return crossedIds;
}

TrIndexes OrganizedTriangles::cross(
    const IndexedTrGroup& ftGroup,
    const IndexedTrGroup& sdGroup )
{
    size_t ftTrNum = ftGroup.size ();
    size_t sdTrNum = sdGroup.size ();

    std::vector<bool> ftCrossMask (ftTrNum, false);
    std::vector<bool> sdCrossMask (sdTrNum, false);

    for (size_t ftId = 0; ftId < ftTrNum; ++ftId)
    for (size_t sdId = 0; sdId < sdTrNum; ++sdId)
        if (ftGroup[ftId].first.crosses (sdGroup[sdId].first))
            ftCrossMask[ftId] = sdCrossMask[sdId] = true;

    TrIndexes crossedIds {};

    for (size_t i = 0; i < ftTrNum; ++i)
        if (ftCrossMask[i])
            crossedIds.push_back (ftGroup[i].second);
    
    for (size_t i = 0; i < sdTrNum; ++i)
        if (sdCrossMask[i])
            crossedIds.push_back (sdGroup[i].second);

    return crossedIds;
}

namespace
{

struct PointSplitter : Point
{
    // Counts average grouped triangles coordinates.
    PointSplitter( const IndexedTrGroup& group )
    {
        coord_ = {0,0,0};
        size_t trNum = group.size ();

        for (size_t i = 0; i < trNum; ++i)
        {
            Coordinates MC = group[i].first;

            for (size_t j = 0; j < DNUM; ++j)
                coord_[j] += MC[j];
        }

        for (size_t j = 0; j < DNUM; ++j)
            coord_[j] /= trNum;
    }

    enum SpaceEighth : char
    {
        First, Second, Third, Fourth,
        Fifth, Sixth, Seventh, Eighth,

        // This object is placed in several space eighths.
        Several,
    };

    SpaceEighth getPointEighth( const Point& P )
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (fpCmpW {P[i]} == coord_[i])
                return Several;

        std::array<bool, DNUM> isLess {};

        for (size_t i = 0; i < DNUM; ++i)
            isLess[i] = fpCmpW {P[i]} < coord_[i];

        char mask = isLess[X] + (isLess[Y] << 1) + (isLess[Z] << 2);

        return SpaceEighth (mask);
    }

    SpaceEighth getTringleEighth( const Triangle& tr )
    {
        std::array<SpaceEighth, 3> eighths {};

        for (size_t i = 0; i < 3; ++i)
            eighths[i] = getPointEighth (tr[i]);

        if (eighths[0] == Several)
            return Several;

        if (eighths[0] != eighths[1] || eighths[1] != eighths[2])
            return Several;

        return eighths[0];
    }
};

} // namespace

void OrganizedTriangles::splitGroups()
{
    for (size_t currDepth = 0; currDepth < splitDepth_; ++currDepth)
    {
        std::vector<IndexedTrGroup> newGroups {};

        auto oldIt = internalTrGroups_.begin ();
        auto oldEnd = internalTrGroups_.end ();

        for (; oldIt != oldEnd; ++oldIt)
        {
            auto splittedGroup = splitGroup (*oldIt);
            borderTrGroups_.push_back (splittedGroup.back ());
            splittedGroup.pop_back ();

            concatVectors (newGroups, std::move (splittedGroup));
        }

        internalTrGroups_ = std::move (newGroups);
    }
}

std::vector<IndexedTrGroup>
    OrganizedTriangles::splitGroup( const IndexedTrGroup& group )
{
    size_t trNum = group.size ();

    PointSplitter splitter {group};
    std::vector<IndexedTrGroup> subGroups {9};

    for (size_t i = 0; i < trNum; ++i)
    {
        auto currTrInfo = group[i];
        auto eighth = splitter.getTringleEighth (currTrInfo.first);
        subGroups[eighth].emplace_back (currTrInfo);
    }

    return subGroups;
}

} // namespace geom3D