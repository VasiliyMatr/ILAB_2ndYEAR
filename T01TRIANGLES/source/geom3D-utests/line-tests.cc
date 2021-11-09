
#include <gtest/gtest.h>

#include "geom3D.hh"
#include "geom3D-gen.hh"

namespace geom3D
{

TEST( LineTests, VecPointCtorTest )
{
    Vector vec = genVec ();
    Point point = genP ();

    Line line {vec, point};

    ASSERT_TRUE (line.contains (point));
    ASSERT_TRUE (line.contains (point + vec));
}

TEST( LineTests, TwoPointsCtorTest )
{
    Segment seg {{1,1,1}, {2,2,2}};
    Line l {seg};

    ASSERT_TRUE (l.contains (seg.P1 ()) && l.contains (seg.P2 ()));
}

TEST( LineTests, ValidationTest )
{
    for (int i = 0; i < 6; ++i)
    {
        fp_t withNan[6] = {}; withNan[i] = nan;
        Line nanLine{Vector {withNan[0], withNan[1], withNan[2]},
                     Point {withNan[3], withNan[4], withNan[5]}};
        fp_t withInf[6] = {}; withInf[i] = inf;
        Line infLine{Vector {withInf[0], withInf[1], withInf[2]},
                     Point {withInf[3], withInf[4], withInf[5]}};

        ASSERT_FALSE (nanLine.isValid ());
        ASSERT_FALSE (infLine.isValid ());
    }

    ASSERT_FALSE (Line {}.isValid ());
}

TEST( LineTests, ContainsTest )
{
    Point p1 = genP ();
    Point p2 = p1 + Vector {2,1,1};
    Vector vec {1,1,1};

    Line line {vec, p1};

    ASSERT_TRUE (line.contains (p1) && line.contains (p1 + vec));
    ASSERT_FALSE (line.contains (p2));
}

TEST( LineTests, parallelToTest )
{
    Vector vec1 = {1,1,1};
    Vector vec2 = {1,0.9,1};
    
    Line l1 {vec1, genP ()};
    Line l2 {vec1, genP ()};
    Line l3 {vec2, genP ()};

    ASSERT_TRUE (l1.parallelTo (l2));
    ASSERT_FALSE (l1.parallelTo (l3));
}

TEST( LineTests, CrossLineOperatorTest )
{
    Line l1 {{1,1,1}, genP ()};
    Line l2 {{1,2,1}, l1.P ()};
    Line l3 {l1.dir (), l1.P () + Vector::e1 ()};
    Point p1 = l1 | l2;
    Point p2 = l1 | l3;

    ASSERT_TRUE (p1 == l1.P ());
    ASSERT_FALSE (p2.isValid ());
}

TEST( LineTests, CrossPlaneOperatorTest )
{
    Plane pl {{1,0,0}, {0,1,0}, {0,0,1}};
    Line l1 {{1,1,1}, {0,0,0}};
    Line l2 {{-1,1,0}, {0,0,0}};
    Point pll1Cross = {1.0/3, 1.0/3, 1.0/3};

    ASSERT_TRUE ((pl | l1) == pll1Cross);
    ASSERT_TRUE ((l1 | pl) == pll1Cross);
    ASSERT_FALSE ((pl | l2).isValid ());
}

} // namespace geom3D