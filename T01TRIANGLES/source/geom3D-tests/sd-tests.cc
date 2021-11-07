
#include "geom3D.hh"
#include "geom3D-split.hh"
#include "geom3D-tests.hh"

namespace geom3D
{

TEST( BoundsTests, PointCtorTest )
{
    Point P1 = genP ();
    Point P2 = genP ();

    UpperBound upper{P1};
    LowerBound lower{P2};

    for (size_t i = 0; i < DNUM; ++i)
    {
        ASSERT_FLOAT_EQ (upper[i], P1[i]);
        ASSERT_FLOAT_EQ (lower[i], P2[i]);
    }
}

TEST( BoundsTests, ExtendTest )
{
    static constexpr size_t PTS_NUM = 100;
    UpperBound upper{};
    LowerBound lower{};
    std::less<fp_t> cmpUpper{};
    std::greater<fp_t> cmpLower{};

    std::vector<Point> points {};
    for (size_t i = 0; i < PTS_NUM; ++i)
    {
        Point P = genP ();
        points.push_back (P);
        upper.extend (P, cmpUpper);
        lower.extend (P, cmpLower);
    }

    for (size_t i = 0; i < PTS_NUM; ++i)
    {
        for (size_t j = 0; j < DNUM; ++j)
        {
            ASSERT_TRUE (upper[j] >= points[i][j]);
            ASSERT_TRUE (lower[j] <= points[i][j]);
        }
    }
}

TEST( SpaceDomainTests, TwoPointsCtorTest )
{
    Point P1 = genP ();
    Point P2 = genP ();

    UpperBound upper {P1};
    upper.extend (P2, std::less<fp_t> ());
    LowerBound lower {P2};
    lower.extend (P1, std::greater<fp_t> ());

    SpaceDomain domain {upper, lower};
    for (size_t i = 0; i < DNUM; ++i)
    {
        ASSERT_FLOAT_EQ (domain.upper ()[i], upper[i]);
        ASSERT_FLOAT_EQ (domain.lower ()[i], lower[i]);
    }

    SpaceDomain invalDomain {lower, upper};
    ASSERT_FALSE (invalDomain.lower ().isValid ());
    ASSERT_FALSE (invalDomain.upper ().isValid ());
}

TEST( SpaceDomainTests, crossesTest )
{
    SpaceDomain domain {{1,1,1}, {-1,-1,-1}};
    Triangle borderTr {{1,1,1}, {2,2,2}, {1,2,3}};
    Triangle notBorderTr {{2,1,1}, {2,3,4}, {2,-1,-1}};

    ASSERT_TRUE (domain.crosses (borderTr));
    ASSERT_FALSE (domain.crosses (notBorderTr));
}

}