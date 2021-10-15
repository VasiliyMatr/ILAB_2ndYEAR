
#include "geom3D-tests.hh"
namespace geom3D
{

TEST( PointTests, ValidationTests )
{
    for (int i = 0; i < 3; ++i)
    {
        fp_t withNan[3] = {}; withNan[i] = nan;
        Point nanPoint {withNan[0], withNan[1], withNan[2]};
        fp_t withInf[3] = {}; withInf[i] = inf;
        Point infPoint {withInf[0], withInf[1], withInf[2]};

        ASSERT_FALSE (nanPoint.isValid ());
        ASSERT_FALSE (infPoint.isValid ());
    }

    ASSERT_FALSE (Point {}.isValid ());
}

TEST( PointTests, CmpTest )
{
    fp_t x = genFP (), y = genFP (), z = genFP();
    Point A = {x,y,z}, B = {x,y,z}, C = {x,y,z + FP_CMP_PRECISION};

    ASSERT_TRUE (A == B);
    ASSERT_FALSE (A == C);
}

TEST( PointTests, PlusVecOperatorTest )
{
    Point A = {genFP (), genFP (), genFP ()};
    Vector v = genVec ();
    Point B = A + v;

    ASSERT_FLOAT_EQ (B.x_, A.x_ + v.x_);
    ASSERT_FLOAT_EQ (B.y_, A.y_ + v.y_);
    ASSERT_FLOAT_EQ (B.z_, A.z_ + v.z_);
}

TEST( PointTests, sqDstTest )
{
    Point p1 = genP ();
    Point p2 = genP ();
    fp_t sqDst1 = (p1.x_-p2.x_)*(p1.x_-p2.x_) +
                  (p1.y_-p2.y_)*(p1.y_-p2.y_) +
                  (p1.z_-p2.z_)*(p1.z_-p2.z_);

    fp_t sqDst2 = sqDst (p1, p2);

    ASSERT_FLOAT_EQ (sqDst1, sqDst2);
}

} // namespace geom3D