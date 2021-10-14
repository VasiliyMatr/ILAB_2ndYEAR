
#include "geom3D-tests.hh"

namespace geom3D
{

TEST( trCrossTests, SegNSeg )
{
    TriangleInfo t1 {{1,1,1}, {2,2,2}, {3,3,3}}; // Main segment in test.
    TriangleInfo t2 {{1,1,0}, {2,2,1}, {3,3,2}}; // Parallel to main segment.
    TriangleInfo t3 {{1,1,0}, {3,3,0}, {2,2,0}}; // Not crossed with main segment.
    TriangleInfo t4 {{1,1,1}, {0,0,0}, {0,0,0}}; // 1 common corner point with main segment. Lies on one line with main segment.
    TriangleInfo t5 {{1,1,1}, {2,2,1}, {3,3,1}}; // 1 common corner point with main segment.
    TriangleInfo t6 {{2,1,1}, {2,3,3}, {2,3,3}}; // Crosses main segment.
    TriangleInfo t7 {{2,2,2}, {2.5,2.5,2.5}, {2,2,2}}; // Have inf num of crosses with main segment.
    TriangleInfo t8 {{2,2,2}, {2,2,2}, {2,2,2}}; // One point. Belongs to main segment.
    TriangleInfo t9 {{0,0,1}, {0,0,1}, {0,0,1}}; // One point. Don't belongs to main segment.

    ASSERT_FALSE (t1.isInter (t2) || t2.isInter (t1));
    ASSERT_FALSE (t1.isInter (t3) || t3.isInter (t1));
    ASSERT_FALSE (t1.isInter (t9) || t9.isInter (t1));

    ASSERT_TRUE (t1.isInter (t4) && t4.isInter (t1));
    ASSERT_TRUE (t1.isInter (t5) && t5.isInter (t1));
    ASSERT_TRUE (t1.isInter (t6) && t6.isInter (t1));
    ASSERT_TRUE (t1.isInter (t7) && t7.isInter (t1));
    ASSERT_TRUE (t1.isInter (t8) && t8.isInter (t1));
}

TEST( trCrossTests, END2END )
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