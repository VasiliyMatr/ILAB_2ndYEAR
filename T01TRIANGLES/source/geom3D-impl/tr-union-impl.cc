
#include "geom3D.hh"

namespace geom3D
{

std::vector<size_t> TrianglesUnion::cross() const
{
    size_t trNum = data_.size ();

    std::vector<bool> crossMask
        (static_cast<std::vector<bool>::size_type> (trNum), false);

    for (size_t i = 0; i < trNum; ++i)
    for (size_t j = i + 1; j < trNum; j++)
        if (data_[i].first.crosses (data_[j].first))
            crossMask [i] = crossMask [j] = true;

    std::vector<size_t> crossedIds {};

    for (size_t i = 0; i < trNum; ++i)
        if (crossMask[i])
            crossedIds.emplace_back (data_[i].second);

    return crossedIds;
}

std::vector<size_t> TrianglesUnion::cross( const TrianglesUnion & sd ) const
{
    size_t ftTrNum = data_.size ();
    size_t sdTrNum = sd.data_.size ();

    std::vector<bool> ftCrossMask
        (static_cast<std::vector<bool>::size_type> (ftTrNum), false);
    std::vector<bool> sdCrossMask
        (static_cast<std::vector<bool>::size_type> (sdTrNum), false);

    for (size_t ftId = 0; ftId < ftTrNum; ++ftId)
    for (size_t sdId = 0; sdId < sdTrNum; ++sdId)
        if (data_[ftId].first.crosses (sd.data_[sdId].first))
            ftCrossMask[ftId] = sdCrossMask[sdId] = true;

    std::vector<size_t> crossedIds {};

    for (size_t i = 0; i < ftTrNum; ++i)
        if (ftCrossMask[i])
            crossedIds.emplace_back (data_[i].second);
    
    for (size_t i = 0; i < sdTrNum; ++i)
        if (sdCrossMask[i])
            crossedIds.emplace_back (sd.data_[i].second);

    return crossedIds;
}

namespace
{

template<class Compare> // Callable to compare coords
struct CoordBound
{
    Coordinates bounds {nan, nan, nan};

    CoordBound( const Triangle & tr )
    {
        std::array<Coordinates, 3> coords =
            { tr.getPoint (0), tr.getPoint (1), tr.getPoint (2) };

        for (size_t i = 0; i < DNUM; ++i)
            bounds[i] =
                Compare{} (Compare{} (coords[0][i], coords[1][i]), coords[2][i]);
    }

    CoordBound extend( const CoordBound& sd )
    {
        for (size_t i = 0; i < DNUM; ++i)
            bounds[i] = Compare{} (bounds[i], sd.bounds[i]);

        return *this;
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

struct PointSplitter : Point
{
    PointSplitter( const CoordBound<UpperBoundComparator>& upper,
                   const CoordBound<LowerBoundComparator>& lower )
    {
        for (size_t i = 0; i < DNUM; ++i)
            coord_[i] = (upper.bounds[i] + lower.bounds[i]) / 2;
    }

    enum SpaceEighth : char
    {
        First,
        Second,
        Third,
        Fourth,
        Fifth,
        Sixth,
        Seventh,
        Eighth,

        // This object is placed in several eighths ().
        Several,
    };

    SpaceEighth getPointEighth( const Point& P )
    {
        std::array<std::partial_ordering, DNUM> cmp =
            {cmpFP (P[X], coord_[X]),
             cmpFP (P[Y], coord_[Y]),
             cmpFP (P[Z], coord_[Z])};

        for (size_t i = 0; i < DNUM; ++i)
            if (cmp[i] == std::partial_ordering::equivalent)
                return Several;

        std::array<bool, DNUM> isLess {};

        for (size_t i = 0; i < DNUM; ++i)
            isLess[i] = cmp[i] == std::partial_ordering::less;

        char mask = isLess[X] + (isLess[Y] << 1) + (isLess[Z] << 2);

        return SpaceEighth (mask);
    }

    SpaceEighth getTringleEighth( const Triangle& tr )
    {
        std::array<SpaceEighth, 3> eighths {};

        for (size_t i = 0; i < 3; ++i)
            eighths[i] = getPointEighth (tr.getPoint (i));

        if (eighths[0] == Several)
            return Several;

        if (eighths[0] != eighths[1] || eighths[1] != eighths[2])
            return Several;

        return eighths[0];
    }
};

} // namespace

std::vector<TrianglesUnion> TrianglesUnion::split() const
{
    size_t trNum = data_.size ();
    if (trNum < 100)
        return std::vector<TrianglesUnion> {*this};

    CoordBound<UpperBoundComparator> upperBound {data_[0].first};
    CoordBound<LowerBoundComparator> lowerBound {data_[0].first};

    for (size_t i = 1; i < trNum; ++i)
    {
        upperBound.extend (data_[i].first);
        lowerBound.extend (data_[i].first);
    }

    PointSplitter splitter {upperBound, lowerBound};
    std::vector<TrianglesUnion> unions {9};

    for (size_t i = 0; i < trNum; ++i)
    {
        const Triangle& currTr = data_[i].first;
        unions[splitter.getTringleEighth (currTr)].data_.emplace_back (currTr);
    }

    return unions;
}

} // namespace geom3D