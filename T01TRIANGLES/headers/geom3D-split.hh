
#include <cassert>

#ifndef GEOM3D_SPLIT_HH_INCL
#define GEOM3D_SPLIT_HH_INCL

#include "geom3D.hh"

namespace geom3D
{

// Used to compute bounds for space domains containing grouped triangles.
// Compare should always return false for nan values. Or it is UB.
template<class Compare>
struct PointBound : Point
{
    PointBound() = default;
    PointBound( const Point& P ) : Point {P} {}

    // All nan fields are automatically replaced with sd values.
    // It is safe to extend default constructed PointBound with valid sd.
    void extend( const PointBound& sd, const Compare& cmp )
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (!cmp (sd.coord_[i], coord_[i]))
                coord_[i] = sd.coord_[i];
    }

    // It is unsafe to use this method with tr that have invalid points.
    // Result coordinates could be whatever.
    PointBound( const Triangle& tr ) : Point {tr[0]}
    {
        this->extend (tr[1], Compare {});
        this->extend (tr[2], Compare {});
    }

    // It is unsafe to use this method with group that have invalid points.
    // Result coordinates could be whatever.
    PointBound( const IndexedTrsGroup& group ) : Point {}
    {
        size_t groupSize = group.size ();
        for (size_t i = 0; i < groupSize; ++i)
            extend (PointBound {group[i].first}, Compare{});
    }
};

using UpperBound = PointBound<std::less<fp_t>>;
using LowerBound = PointBound<std::greater<fp_t>>;

// Represents 3D space domain.
class SpaceDomain
{
    // upper_[i] >= lower_[i] is a protected invariant
    UpperBound upper_ {};
    LowerBound lower_ {};

public:
    Point upper() const { return upper_; }
    Point lower() const { return lower_; }

    SpaceDomain() = default;
    SpaceDomain( const Point& up, const Point& lo ) : upper_ {up}, lower_ {lo}
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (upper_[i] < lower_[i])
            {
                lower_ = Point{};
                upper_ = Point{};
                return;
            }
    }

    SpaceDomain( const IndexedTrsGroup& gr ) : upper_ {gr}, lower_ {gr} {}

    // Does this space domain crosses given triangle?
    // Works properly only for border triangles! 
    bool crosses( const Triangle& tr ) const
    {
        for (size_t coordId = 0; coordId < DNUM; ++coordId)
        {
            size_t j = 0;
            for (; j < TR_POINT_NUM; ++j)
                if (fpCmpW {tr[j][coordId]} >= lower_[coordId])
                    break;
            if (j == TR_POINT_NUM) return false;
            for (j = 0; j < TR_POINT_NUM; ++j)
                if (fpCmpW {tr[j][coordId]} <= upper_[coordId])
                    break;
            if (j == TR_POINT_NUM) return false;
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

            for (size_t coordId = 0; coordId < DNUM; ++coordId)
                coord_[coordId] += trMassCenter[coordId];
        }

        for (size_t i = 0; i < DNUM; ++i)
            coord_[i] /= trNum;
    }

    SpaceOctant getOctant( const Point& P )
    {
        for (size_t i = 0; i < DNUM; ++i)
            if (fpCmpW {P[i]} == coord_[i])
                return SpaceOctant::SEVERAL;

        char octant = 0;
        for (size_t i = 0; i < DNUM; ++i)
            octant += (fpCmpW {P[i]} > coord_[i]) << i;

        return SpaceOctant (octant);
    }

    SpaceOctant getOctant( const Triangle& tr )
    {
        std::array<SpaceOctant, TR_POINT_NUM> eighths {};

        for (size_t i = 0; i < TR_POINT_NUM; ++i)
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

    // Iterator used for passes on specified depth.
    struct DepthIter
    {
        SubGroup* node_ = nullptr;
        const size_t depth_;

    private:
        // This node branches ids
        std::vector<size_t> branchesIds {};

    public:
        DepthIter( SubGroup* root, size_t depth ) :
            node_ (root), depth_ (depth), branchesIds (depth + 1, 0)
        {
            for (size_t i = 0; i < depth && node_ != nullptr; ++i)
                node_ = node_->children_[0];
        }

        DepthIter operator++();

        bool operator==( const DepthIter& sd ) const
            { return node_ == sd.node_; }

        static DepthIter end()
            { return DepthIter { nullptr, 0 }; }
    };

    const size_t splitDepth_;
    SubGroup* root_;

    fp_t avgGroupsSize_ = 0;
    fp_t avgBordsSize_ = 0;

public:

    SplittedTrsGroup( const IndexedTrsGroup& group, size_t targetGroupSize );
   ~SplittedTrsGroup();

    // Will implement later.
    SplittedTrsGroup( const SplittedTrsGroup& ) = delete;
    SplittedTrsGroup( SplittedTrsGroup&& sd ) = delete;
    SplittedTrsGroup operator=( const SplittedTrsGroup& ) = delete;
    SplittedTrsGroup operator=( SplittedTrsGroup&& sd ) = delete;

    TrsIndexes cross() const;

private:
    // Methods for ctor.
    void splitGroups( const IndexedTrsGroup& group );
    void calcBorders();
    static void splitGroup( SubGroup* );
    static void caclBorder( SubGroup* );

public:
    // Testing stuff - implemented in tests files.
    static bool testSplitting();
};

// Returns intersecting trianlges indexes.
TrsIndexes cross( const IndexedTrsGroup& );
TrsIndexes cross( const IndexedTrsGroup&, const IndexedTrsGroup& );

} // namespace geom3D

#endif // #ifndef GEOM3D_SPLIT_HH_INCL