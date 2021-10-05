
#include "geom3D-tests.hh"
using namespace geom3D;

TEST( SegTests, CtorTest )
{

    Point A {genFPVal (), genFPVal (), genFPVal ()};
    Point B {genFPVal (), genFPVal (), genFPVal ()};

    Segment seg {A, B};

    ASSERT_FLOAT_EQ (seg.A_.x_, A.x_);
    ASSERT_FLOAT_EQ (seg.A_.y_, A.y_);
    ASSERT_FLOAT_EQ (seg.A_.z_, A.z_);
    ASSERT_FLOAT_EQ (seg.B_.x_, B.x_);
    ASSERT_FLOAT_EQ (seg.B_.y_, B.y_);
    ASSERT_FLOAT_EQ (seg.B_.z_, B.z_);

}

TEST( segTests, LineCrossOperatorTests )
{

    Segment seg {{2, 3, 0}, {4, 1, 0}};
    Line line {{1, 0, 0}, {2, 0, 0}};

    Point cross = seg | line;

    ASSERT_FLOAT_EQ (cross.x_, 3);
    ASSERT_FLOAT_EQ (cross.y_, 2);
    ASSERT_FLOAT_EQ (cross.z_, 0);

}