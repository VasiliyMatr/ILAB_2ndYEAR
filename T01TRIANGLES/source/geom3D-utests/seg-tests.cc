
#include <gtest/gtest.h>

#include "geom3D.hh"
#include "geom3D-gen.hh"

namespace geom3D
{

TEST( SegTests, ValidationTest )
{
    for (int i = 0; i < 6; ++i)
    {
        fp_t withNan[6] = {}; withNan[i] = nan;
        Segment nanSeg {Point {withNan[0], withNan[1], withNan[2]},
                        Point {withNan[3], withNan[4], withNan[5]}};
        fp_t withInf[6] = {}; withInf[i] = inf;
        Segment infSeg {Point {withInf[0], withInf[1], withInf[2]},
                        Point {withInf[3], withInf[4], withInf[5]}};

        ASSERT_FALSE (nanSeg.isValid ());
        ASSERT_FALSE (infSeg.isValid ());
    }

    ASSERT_FALSE (Segment {}.isValid ());
}

TEST( SegTests, LineCrossOperatorTest )
{
    Segment seg {{2, 3, 0}, {4, 1, 0}};
    Line line {{1, 0, 0}, {0, 2, 0}};

    Point p1 = seg | line;
    Point p2 = line | seg;

    ASSERT_FLOAT_EQ (p1[X], 3);
    ASSERT_FLOAT_EQ (p1[Y], 2);
    ASSERT_FLOAT_EQ (p1[Z], 0);
    ASSERT_TRUE (p1 == p2);
}

TEST( SegTests, LinearContainsTest )
{
    Segment seg {{1,1,1}, {3,3,3}};
    Point P1 {2,2,2};
    Point P2 {5,5,5};

    ASSERT_TRUE (seg.linearContains (P1));
    ASSERT_FALSE (seg.linearContains (P2));
}

TEST( SegTests, PlaneCrossOperatorTest )
{
    Plane plane {{0,0,1}, {0,1,0}, {1,0,0}};
    Segment seg1 {{0,0,0}, {2,2,2}};
    Segment seg2 {{0,0,0}, {-1,-1,-1}};

    Point p11 = seg1 | plane;
    Point p12 = plane | seg1;
    Point p21 = seg2 | plane;
    Point p22 = plane | seg2;

    ASSERT_FLOAT_EQ (p11[X], 1.0 / 3);
    ASSERT_FLOAT_EQ (p11[Y], 1.0 / 3);
    ASSERT_FLOAT_EQ (p11[Z], 1.0 / 3);
    ASSERT_TRUE (p11 == p12);
    ASSERT_FALSE (p21.isValid () || p22.isValid ());
}

} // namespace geom3D
