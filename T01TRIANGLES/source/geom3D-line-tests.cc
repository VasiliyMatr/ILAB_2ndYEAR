
#include "geom3D-tests.hh"
using namespace geom3D;

TEST( LineTests, CtorTest )
{
    Vector vec = genVec ();
    Point point {genFPVal (), genFPVal (), genFPVal ()};

    Line line {vec, point};

    ASSERT_FLOAT_EQ (line.dirVec_.x_, vec.x_);
    ASSERT_FLOAT_EQ (line.dirVec_.y_, vec.y_);
    ASSERT_FLOAT_EQ (line.dirVec_.z_, vec.z_);
    ASSERT_FLOAT_EQ (line.point_.x_, point.x_);
    ASSERT_FLOAT_EQ (line.point_.y_, point.y_);
    ASSERT_FLOAT_EQ (line.point_.z_, point.z_);

}

TEST( LineTests, SegCrossOperatorTests )
{

    Segment seg {{4, 3, 0}, {2, 3, -2}};
    Line line {{1, 1, 1}, {2, 2, -2}};

    Point cross = line | seg;

    ASSERT_FLOAT_EQ (cross.x_, 3);
    ASSERT_FLOAT_EQ (cross.y_, 3);
    ASSERT_FLOAT_EQ (cross.z_, -1);

}