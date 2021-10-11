
#include "geom3D-tests.hh"

namespace geom3D
{

TEST( TriangleInfoTests, END2END )
{
    TriangleInfo first {{1,1,1}, {5,1,1}, {3,4,1}};
    TriangleInfo second {{3,1,-3}, {3,1,3}, {3,4,0}};
    TriangleInfo third {{-10,5,0}, {2,3,0}, {1,6,0}};

    ASSERT_TRUE (first.isInter (second));
    ASSERT_TRUE (second.isInter (first));

    ASSERT_FALSE (third.isInter (first));
    ASSERT_FALSE (third.isInter (second));
    ASSERT_FALSE (first.isInter (third));
    ASSERT_FALSE (second.isInter (third));
}

} // namespace geom3D