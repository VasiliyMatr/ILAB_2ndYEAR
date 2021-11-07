
#include "geom3D.hh"

#ifndef GEOM3D_SPLIT_HH_INCL
#define GEOM3D_SPLIT_HH_INCL

namespace geom3D
{

// Triangles will be splitted in smaller groups for
// asymptotic computational complexity reduction
using IndexedTrsGroup = std::vector<std::pair<Triangle, size_t>>;
using TrsIndexes = std::vector<size_t>;

TrsIndexes cross( const IndexedTrsGroup& );
TrsIndexes cross( const IndexedTrsGroup&, const IndexedTrsGroup& );

// Used to compute bounds for space domains with grouped triangles.
template<class Compare> // Callable to compare coords
struct PointBound : Point
{
    PointBound() = default;
    PointBound( const Point& P ) : Point {P} {}

    void extend( const PointBound& sd )
    {
        for (size_t i = 0; i < DNUM; ++i)
            coord_[i] = Compare{} (coord_[i], sd.coord_[i]);
    }

    PointBound( const Triangle& tr ) : Point {tr[0]}
    {
        this->extend (tr[1]);
        this->extend (tr[2]);
    }

    PointBound( const IndexedTrsGroup& group ) : Point {}
    {
        size_t groupSize = group.size ();

        if (groupSize == 0) return;

        *this = PointBound {group[0].first};

        for (size_t i = 1; i < groupSize; ++i)
            this->extend (PointBound {group[i].first});
    }
};

// Strict comparator for accurate bounds
struct UpperBoundComparator
{
    fp_t operator()( fp_t ft, fp_t sd ) const
        { return std::max (ft, sd); }
};

// Strict comparator for accurate bounds
struct LowerBoundComparator
{
    fp_t operator()( fp_t ft, fp_t sd ) const
        { return std::min (ft, sd); }
};

using UpperBound = PointBound<UpperBoundComparator>;
using LowerBound = PointBound<LowerBoundComparator>;

using Bounds = std::pair<UpperBound, LowerBound>;

// Represents 3D space domain.
struct SpaceDomain
{
    UpperBound upper_ {};
    LowerBound lower_ {};

    SpaceDomain() = default;
    SpaceDomain( const Point& upper, const Point& lower ) :
        upper_ {upper}, lower_ {lower} {}
    SpaceDomain( const IndexedTrsGroup& group ) : upper_ {group}, lower_ {group} {}

    bool contains( const Point& P ) const
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (fpCmpW {upper_[i]} < P[i] || fpCmpW {lower_[i]} > P[i])
                return false;

        return true;
    }

    bool crosses( const Triangle& tr ) const
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (fpCmpW {tr[0][i]} < lower_[i] &&
                fpCmpW {tr[1][i]} < lower_[i] &&
                fpCmpW {tr[2][i]} < lower_[i])
            return false;

        for (size_t i = 0; i < DNUM; ++i)
            if (fpCmpW {tr[0][i]} > upper_[i] &&
                fpCmpW {tr[1][i]} > upper_[i] &&
                fpCmpW {tr[2][i]} > upper_[i])
            return false;

        return true;
    }
};

// Octants could not be in right geometrical order.
enum class SpaceOctant
{
    FIRST, SECOND, THIRD, FOURTH,
    FIFTH, SIXTH, SEVENTH, EIGHTH,

    // This object is placed in several space octants.
    SEVERAL,
};

struct PointSplitter : Point
{
    // Counts average grouped triangles coordinates.
    PointSplitter( const IndexedTrsGroup& group ) : Point {0,0,0}
    {
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

    SpaceOctant getPointEighth( const Point& P )
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (fpCmpW {P[i]} == coord_[i])
                return SpaceOctant:: SEVERAL;

        std::array<bool, DNUM> isLess {};

        for (size_t i = 0; i < DNUM; ++i)
            isLess[i] = fpCmpW {P[i]} < coord_[i];

        char mask = isLess[X] + (isLess[Y] << 1) + (isLess[Z] << 2);

        return SpaceOctant (mask);
    }

    SpaceOctant getTriangleEighth( const Triangle& tr )
    {
        std::array<SpaceOctant, 3> eighths {};

        for (size_t i = 0; i < 3; ++i)
            eighths[i] = getPointEighth (tr[i]);

        if (eighths[0] != eighths[1] || eighths[1] != eighths[2])
            return SpaceOctant::SEVERAL;

        return eighths[0];
    }
};

template<class Data>
void concatVectors( std::vector<Data>& dest, const std::vector<Data>& src )
    { dest.insert (dest.end (), src.begin (), src.end ()); }

class SplittedTrsGroup
{
    static constexpr size_t SUB_GROUPS_NUM = 8;
    const size_t treeHeight_;

    // Sub group encased with space domain.
    struct SubGroup
    {
        SubGroup* parent_ = nullptr;
        std::array<SubGroup*, SUB_GROUPS_NUM> subGroups_ {nullptr};

        SpaceDomain containingSpaceDomain_ {};
        // For not leaf nodes - stores triangles crosseed by sub groups
        // partitioning borders (if they are not contained by parents groups borderTrs_)
        // For leafs - stores triangles that crosses this group domain border.
        IndexedTrsGroup borderTrs_ {};
        // Empty for not leaf nodes.
        // Contains this sub group triangles.
        IndexedTrsGroup internalTrs_ {};
    };

    SubGroup* root_;

public:
    SplittedTrsGroup( const IndexedTrsGroup& group, size_t targetGroupSize );
    TrsIndexes cross();

private:

    // Sub funcs for ctor.
    template <class LeafsHandler>
    void leafsBypass( LeafsHandler& );
    void splitGroups( const IndexedTrsGroup& group );
    static void splitGroup( SubGroup* );
    void calcBorders();

    struct SplitBypassHandler
    {
        void operator()( SubGroup* group )
            { splitGroup (group); }
    };

    struct CrossBypassHandler
    {
        TrsIndexes crossedIds_ {};

        void operator()( SubGroup* group )
        {
            TrsIndexes cross( const IndexedTrsGroup& );
            TrsIndexes cross( const IndexedTrsGroup&, const IndexedTrsGroup& );
            concatVectors (crossedIds_, cross (group->internalTrs_));
            concatVectors (crossedIds_, cross (group->borderTrs_));
            concatVectors (crossedIds_,
                cross (group->borderTrs_, group->internalTrs_));
        }
    };

    struct CalcBordersBypassHandler
    {
        void operator()( SubGroup* group )
        {
            IndexedTrsGroup allPossibleBorderTrs {};

            SubGroup* curNode = group->parent_;
            for (; curNode != nullptr; curNode = curNode->parent_)
                concatVectors (allPossibleBorderTrs, curNode->borderTrs_);

            size_t trNum = allPossibleBorderTrs.size ();
            for (size_t i = 0; i < trNum; ++i)
            {
                auto curTr = allPossibleBorderTrs[i];
                if (group->containingSpaceDomain_.crosses (curTr.first))
                    group->borderTrs_.push_back (curTr);
            }
        }
    };
};

} // namespace geom3D

#endif // #ifndef GEOM3D_SPLIT_HH_INCL