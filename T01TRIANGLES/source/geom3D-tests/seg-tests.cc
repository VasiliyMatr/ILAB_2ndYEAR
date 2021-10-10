
#include "geom3D-tests.hh"
namespace geom3D
{

TEST( SegTests, CtorTest )
{
    Point A {genFP (), genFP (), genFP ()};
    Point B {genFP (), genFP (), genFP ()};

    Segment seg {A, B};

    ASSERT_FLOAT_EQ (seg.A_.x_, A.x_);
    ASSERT_FLOAT_EQ (seg.A_.y_, A.y_);
    ASSERT_FLOAT_EQ (seg.A_.z_, A.z_);
    ASSERT_FLOAT_EQ (seg.B_.x_, B.x_);
    ASSERT_FLOAT_EQ (seg.B_.y_, B.y_);
    ASSERT_FLOAT_EQ (seg.B_.z_, B.z_);
}

TEST( segTests, ValidatinTests )
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

TEST( segTests, LineCrossOperatorTests )
{
    Segment seg {{2, 3, 0}, {4, 1, 0}};
    Line line {{1, 0, 0}, {0, 2, 0}};

    Point cross = seg | line;

    ASSERT_FLOAT_EQ (cross.x_, 3);
    ASSERT_FLOAT_EQ (cross.y_, 2);
    ASSERT_FLOAT_EQ (cross.z_, 0);
}

} // namespace geom3D