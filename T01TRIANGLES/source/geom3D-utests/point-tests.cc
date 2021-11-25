
#include <gtest/gtest.h>

#include "geom3D-gen.hh"
#include "geom3D.hh"

namespace geom3D
{

TEST(PointTests, ValidationTest)
{
    for (int i = 0; i < 3; ++i)
    {
        fp_t withNan[3] = {};
        withNan[i] = nan;
        Point nanPoint{withNan[0], withNan[1], withNan[2]};
        fp_t withInf[3] = {};
        withInf[i] = inf;
        Point infPoint{withInf[0], withInf[1], withInf[2]};

        ASSERT_FALSE(nanPoint.isValid());
        ASSERT_FALSE(infPoint.isValid());
    }

    ASSERT_FALSE(Point{}.isValid());
}

TEST(PointTests, CmpTest)
{
    fp_t x = genFP(), y = genFP(), z = genFP();
    Point A = {x, y, z}, B = {x, y, z}, C = {x, y, z + fpCmpW::CMP_PRECISION * 2};

    ASSERT_TRUE(A == B);
    ASSERT_FALSE(A == C);
}

TEST(PointTests, PlusVecOperatorTest)
{
    Point A = {genFP(), genFP(), genFP()};
    Vector v = genVec();
    Point B = A + v;

    ASSERT_FLOAT_EQ(B[X], A[X] + v[X]);
    ASSERT_FLOAT_EQ(B[Y], A[Y] + v[Y]);
    ASSERT_FLOAT_EQ(B[Z], A[Z] + v[Z]);
}

TEST(PointTests, sqDstTest)
{
    Point p1 = genP();
    Point p2 = genP();
    fp_t sqDst1 =
        (p1[X] - p2[X]) * (p1[X] - p2[X]) + (p1[Y] - p2[Y]) * (p1[Y] - p2[Y]) + (p1[Z] - p2[Z]) * (p1[Z] - p2[Z]);

    fp_t sqDst2 = sqDst(p1, p2);

    ASSERT_FLOAT_EQ(sqDst1, sqDst2);
}

} // namespace geom3D
