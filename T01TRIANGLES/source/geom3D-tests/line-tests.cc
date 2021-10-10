
#include "geom3D-tests.hh"
namespace geom3D
{

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

TEST( LineTests, ValidationTests )
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

TEST( LineTests, SegCrossOperatorTests )
{

    Segment seg {{4, 3, 0}, {2, 3, -2}};
    Line line {{1, 1, 1}, {2, 2, -2}};

    Point cross = line | seg;

    ASSERT_FLOAT_EQ (cross.x_, 3);
    ASSERT_FLOAT_EQ (cross.y_, 3);
    ASSERT_FLOAT_EQ (cross.z_, -1);

}

} // namespace geom3D