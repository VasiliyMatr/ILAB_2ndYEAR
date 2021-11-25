
#include <cassert>

#ifndef GEOM3D_SPLIT_HH_INCL
#define GEOM3D_SPLIT_HH_INCL

#include "geom3D.hh"

namespace geom3D
{

// Used to compute bounds for space domains containing grouped triangles.
// Compare should always return false for nan values.
// Otherwise result coords are undefined.
template <class Compare> struct PointBound : Point
{
    PointBound() = default;
    PointBound(const Point &P) : Point{P}
    {
    }

    // WARNED.
    void extend(const PointBound &sd)
    {
        if (!sd.isValid())
            *this = Point{};

        const Compare cmp{};
        for (size_t i = 0; i < DNUM; ++i)
            if (cmp(coord_[i], sd.coord_[i]))
                coord_[i] = sd.coord_[i];
    }

    // WARNED.
    PointBound(const Triangle &tr) : Point{tr[0]}
    {
        this->extend(tr[1]);
        this->extend(tr[2]);
    }

    // WARNED.
    // Result is invalid for empty IndexedTrsGroup.
    PointBound(const IndexedTrsGroup &group) : Point{}
    {
        size_t groupSize = group.size();
        if (groupSize == 0)
            return;
        *this = PointBound{group[0].first};
        for (size_t i = 1; i < groupSize; ++i)
            extend(PointBound{group[i].first});
    }
};

using UpperBound = PointBound<std::less<fp_t>>;
using LowerBound = PointBound<std::greater<fp_t>>;

// Represents 3D space domain.
class SpaceDomain
{
    // (upper_[i] >= lower_[i] or upper_ and lower_ are invalid)
    // is a protected invariant.
    UpperBound upper_;
    LowerBound lower_;

  public:
    Point upper() const
    {
        return upper_;
    }
    Point lower() const
    {
        return lower_;
    }

    SpaceDomain() = default;
    SpaceDomain(const Point &up, const Point &lo);
    SpaceDomain(const IndexedTrsGroup &gr) : upper_{gr}, lower_{gr}
    {
    }

    // Does this space domain crosses given triangle?
    // Works properly only for valid border triangles!
    // Always false for invalid upper_ or lower_.
    bool crosses(const Triangle &tr) const;
};

// Octants are not in right geometrical order.
enum SpaceOctant
{
    FIRST,
    SECOND,
    THIRD,
    FOURTH,
    FIFTH,
    SIXTH,
    SEVENTH,
    EIGHTH,

    // This object is placed in several space octants.
    SEVERAL,
    INVALID
};

// Used to split triangles groups into subgroups.
struct PointSplitter : Point
{
    // Counts average grouped triangles coordinates.
    // Splitter is invalid for empty group.
    PointSplitter(const IndexedTrsGroup &group);
    PointSplitter(const SpaceDomain &domain)
    {
        for (size_t i = 0; i < DNUM; ++i)
            coord_[i] = (domain.lower()[i] + domain.upper()[i]) / 2;
    }

    // Always returns SEVERAL for invalid PointSplitter.
    SpaceOctant getOctant(const Point &P) const;
    SpaceOctant getOctant(const Triangle &tr) const;
};

template <class Data> inline void concatVectors(std::vector<Data> &dest, const std::vector<Data> &src)
{
    dest.insert(dest.end(), src.begin(), src.end());
}

template <class Data> inline void concatVectors(std::vector<Data> &dest, std::vector<Data> &&src)
{
    dest.insert(dest.end(), src.begin(), src.end());
}

// Octo-tree is used to split triangles into smaller groups.
class SplittedTrsGroup
{
    static constexpr size_t SUB_GROUPS_NUM = 8;

    // Sub group encased with space domain.
    struct SubGroup
    {
        SubGroup *parent_ = nullptr;
        std::array<SubGroup *, SUB_GROUPS_NUM> children_{nullptr};

        SpaceDomain spaceDomain_{};
        // For leafs - stores triangles that crosses this group space domain borders.
        IndexedTrsGroup borderTrs_{};
        // For leafs - contains this sub group space domain internal triangles.
        IndexedTrsGroup internalTrs_{};
    };

    // Iterator used for passes on specified depth.
    struct DepthIter
    {
        SubGroup *node_ = nullptr;
        const size_t depth_;

      private:
        // This node branches ids
        std::vector<size_t> branchesIds_;

      public:
        DepthIter(SubGroup *root, size_t depth) : node_(root), depth_(depth), branchesIds_(depth + 1, 0)
        {
            for (size_t i = 0; i < depth && node_ != nullptr; ++i)
                node_ = node_->children_[0];
        }

        DepthIter operator++();

        bool operator==(const DepthIter &sd) const noexcept
        {
            return node_ == sd.node_;
        }

        static DepthIter end()
        {
            return DepthIter{nullptr, 0};
        }
    };

    const size_t splitDepth_;
    SubGroup *root_ = nullptr;

  public:
    SplittedTrsGroup(const IndexedTrsGroup &group, size_t targetGroupSize);
    ~SplittedTrsGroup();

    // Will implement later.
    SplittedTrsGroup(const SplittedTrsGroup &) = delete;
    SplittedTrsGroup(SplittedTrsGroup &&sd) = delete;
    SplittedTrsGroup operator=(const SplittedTrsGroup &) = delete;
    SplittedTrsGroup operator=(SplittedTrsGroup &&sd) = delete;

    TrsIndexes cross() const;

  private:
    // Calc SplittedTrsGroup computational complexity related to
    // cross (root_->internalTrs_) call computational complexity.
    fp_t calcСomplexityRatio() const;
    // Methods for ctor.
    void splitGroups(const IndexedTrsGroup &group);
    void calcBorders();
    static void splitGroup(SubGroup *);
    static void caclBorder(SubGroup *);

  public:
    // Testing stuff - implemented in tests files.
    static bool testSplitting();
};

// Returns intersecting trianlges indexes.
TrsIndexes cross(const IndexedTrsGroup &);
TrsIndexes cross(const IndexedTrsGroup &, const IndexedTrsGroup &);

} // namespace geom3D

#endif // #ifndef GEOM3D_SPLIT_HH_INCL
