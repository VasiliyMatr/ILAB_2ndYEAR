
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
            crossedIds.push_back (data_[i].second);
    
    for (size_t i = 0; i < sdTrNum; ++i)
        if (sdCrossMask[i])
            crossedIds.push_back (sd.data_[i].second);

    return crossedIds;
}

namespace
{

struct PointSplitter : Point
{
    // Counts average triangles coordinates.
    PointSplitter( const std::vector<TrianglesUnion::trNId>& data )
    {
        size_t trNum = data.size ();

        for (size_t i = 0; i < trNum; ++i)
        {
            Coordinates MC = data[i].first;

            for (size_t j = 0; j < DNUM; ++j)
                coord_[j] += MC[j];
        }

        for (size_t j = 0; j < DNUM; ++j)
            coord_[j] /= trNum;
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

std::vector<TrianglesUnion> TrianglesUnion::split() const
{
    size_t trNum = data_.size ();
    if (trNum < 100)
        return std::vector<TrianglesUnion> {*this};

    PointSplitter splitter {data_};
    std::vector<TrianglesUnion> unions {9};

    for (size_t i = 0; i < trNum; ++i)
    {
        auto currTrInfo = data_[i];
        auto eighth = splitter.getTringleEighth (currTrInfo.first);
        unions[eighth].data_.emplace_back (currTrInfo);
    }

    return unions;
}

} // namespace geom3D