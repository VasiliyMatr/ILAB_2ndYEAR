
#include <cassert>
#include "geom3D.hh"

#ifndef GEOM3D_SPLIT_HH_INCL
#define GEOM3D_SPLIT_HH_INCL

namespace geom3D
{

// Triangles will be splitted in smaller groups for
// asymptotic computational complexity reduction.
using IndexedTrsGroup = std::vector<std::pair<Triangle, size_t>>;
using TrsIndexes = std::vector<size_t>;

// Used to compute bounds for space domains containing grouped triangles.
template<class Compare>
struct PointBound : Point
{
    PointBound() = default;
    PointBound( const Point& P ) : Point {P} {}

    void extend( const PointBound& sd, const Compare& cmp )
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (cmp (coord_[i], sd.coord_[i]))
                coord_[i] = sd.coord_[i];
    }

    PointBound( const Triangle& tr ) : Point {tr[0]}
    {
        this->extend (tr[1], Compare {});
        this->extend (tr[2], Compare {});
    }

    PointBound( const IndexedTrsGroup& group ) : Point {}
    {
        size_t groupSize = group.size ();
        if (groupSize == 0) return;

        *this = PointBound {group[0].first};
        for (size_t i = 1; i < groupSize; ++i)
            extend (PointBound {group[i].first}, Compare{});
    }
};

using UpperBound = PointBound<std::less<fp_t>>;
using LowerBound = PointBound<std::greater<fp_t>>;

// Represents 3D space domain.
struct SpaceDomain
{
    UpperBound upper_ {};
    LowerBound lower_ {};

    SpaceDomain() = default;
    SpaceDomain( const Point& up, const Point& lo ) : upper_ {up}, lower_ {lo} {}
    SpaceDomain( const IndexedTrsGroup& gr ) : upper_ {gr}, lower_ {gr} {}

    // Does this space domain crosses given !BORDER! triangle?
    bool crosses( const Triangle& tr ) const
    {
        for (size_t i = 0; i < DNUM; ++i)
        {
            size_t j = 0;
            for (; j < 3; ++j) if (fpCmpW {tr[j][i]} >= lower_[i]) break;
            if (j == 3) return false;
            for (j = 0; j < 3; ++j) if (fpCmpW {tr[j][i]} <= upper_[i]) break;
            if (j == 3) return false;
        }
        return true;
    }
};

// Octants are not in right geometrical order.
enum SpaceOctant
{
    FIRST, SECOND, THIRD, FOURTH,
    FIFTH, SIXTH, SEVENTH, EIGHTH,

    // This object is placed in several space octants.
    SEVERAL,
};

// Used to split triangles groups into subgroups.
struct PointSplitter : Point
{
    // Counts average grouped triangles coordinates.
    PointSplitter( const IndexedTrsGroup& group ) : Point {0,0,0}
    {
        size_t trNum = group.size ();

        for (size_t i = 0; i < trNum; ++i)
        {
            Coordinates trMassCenter = group[i].first;

            for (size_t j = 0; j < DNUM; ++j)
                coord_[j] += trMassCenter[j];
        }

        for (size_t i = 0; i < DNUM; ++i)
            coord_[i] /= trNum;
    }

    SpaceOctant getOctant( const Point& P )
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (fpCmpW {P[i]} == coord_[i])
                return SpaceOctant:: SEVERAL;

        char octant = 0;
        for (size_t i = 0; i < DNUM; ++i)
            octant += (fpCmpW {P[i]} > coord_[i]) << i;

        return SpaceOctant (octant);
    }

    SpaceOctant getOctant( const Triangle& tr )
    {
        std::array<SpaceOctant, 3> eighths {};

        for (size_t i = 0; i < 3; ++i)
            eighths[i] = getOctant (tr[i]);

        if (eighths[0] != eighths[1] || eighths[1] != eighths[2])
            return SpaceOctant::SEVERAL;

        return eighths[0];
    }
};

template<class Data>
void concatVectors( std::vector<Data>& dest, const std::vector<Data>& src )
    { dest.insert (dest.end (), src.begin (), src.end ()); }

template<class Data>
void concatVectors( std::vector<Data>& dest, std::vector<Data>&& src )
    { dest.insert (dest.end (), src.begin (), src.end ()); }

// Octo-tree is used to split triangles into smaller groups.
class SplittedTrsGroup
{
    static constexpr size_t SUB_GROUPS_NUM = 8;
    const size_t splitDepth_;
    size_t height_ = 1;

    // Sub group encased with space domain.
    struct SubGroup
    {
        SubGroup* parent_ = nullptr;
        std::array<SubGroup*, SUB_GROUPS_NUM> children_ {nullptr};

        SpaceDomain spaceDomain_ {};
        // For leafs - stores triangles that crosses this group space domain borders.
        IndexedTrsGroup borderTrs_ {};
        // For leafs - contains this sub group space domain internal triangles.
        IndexedTrsGroup internalTrs_ {};
    };

    SubGroup* root_;

public:
    SplittedTrsGroup( const IndexedTrsGroup& group, size_t targetGroupSize );
    TrsIndexes cross();

private:
    // Used to perform leafs bypass:
    template <class LeafsHandler>
    void leafsBypass( LeafsHandler& );

    // Methods for ctor.
    void splitGroups( const IndexedTrsGroup& group );
    void calcBorders();
    static void splitGroup( SubGroup* );

    // Handlers for leafsBypass:
    struct SplitBypassHandler
    {
        void operator()( SubGroup* group )
            { splitGroup (group); }
    };

    struct CrossBypassHandler
    {
        TrsIndexes crossedIds_ {};
        void operator()( SubGroup* );
    };

    struct CalcBordersBypassHandler
    {
        void operator()( SubGroup* );
    };
};

// Returns intersecting trianlges indexes.
TrsIndexes cross( const IndexedTrsGroup& );
TrsIndexes cross( const IndexedTrsGroup&, const IndexedTrsGroup& );

} // namespace geom3D

#endif // #ifndef GEOM3D_SPLIT_HH_INCL