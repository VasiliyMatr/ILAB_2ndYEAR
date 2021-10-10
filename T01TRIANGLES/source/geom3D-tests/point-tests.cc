
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

} // namespace geom3D