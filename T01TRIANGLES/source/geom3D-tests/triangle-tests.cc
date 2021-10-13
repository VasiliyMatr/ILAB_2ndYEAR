
#include "geom3D-tests.hh"

namespace geom3D
{

TEST( TriangleInfoTests, END2END )
{
    TriangleInfo t1 {{1,1,1}, {5,1,1}, {3,4,1}};
    TriangleInfo t2 {{3,1,-3}, {3,1,3}, {3,4,0}};
    TriangleInfo t3 {{-10,5,0}, {2,3,0}, {1,6,0}};

    ASSERT_TRUE (t1.isInter (t2));
    ASSERT_TRUE (t2.isInter (t1));

    ASSERT_FALSE (t3.isInter (t1));
    ASSERT_FALSE (t3.isInter (t2));
    ASSERT_FALSE (t1.isInter (t3));
    ASSERT_FALSE (t2.isInter (t3));

    TriangleInfo t4 {{1,1,1}, {2,1,1}, {1,2,1}};
    TriangleInfo t5 {{1,1,1}, {1,0,0}, {0,1,0}};
    ASSERT_TRUE (t4.isInter (t5));
    ASSERT_TRUE (t5.isInter (t4));

    TriangleInfo t6 {{1,1,1}, {2,1.5,1}, {1.5,2,1}};
    ASSERT_TRUE (t6.isInter (t4));
    ASSERT_TRUE (t4.isInter (t6));

}

} // namespace geom3D