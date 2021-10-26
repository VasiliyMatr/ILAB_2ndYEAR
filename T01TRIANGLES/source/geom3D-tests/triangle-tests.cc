
#include "geom3D-tests.hh"

namespace geom3D
{

TEST( trCrossTests, segNSeg )
{
    Triangle t1 {{1,1,1}, {2,2,2}, {3,3,3}}; // Main segment in test.
    Triangle t2 {{1,1,0}, {2,2,1}, {3,3,2}}; // Parallel to main segment.
    Triangle t3 {{1,1,0}, {3,3,0}, {2,2,0}}; // Not crossed with main segment.
    Triangle t4 {{1,1,1}, {0,0,0}, {0,0,0}}; // 1 common corner point with main segment. Lies on one line with main segment.
    Triangle t5 {{1,1,1}, {2,2,1}, {3,3,1}}; // 1 common corner point with main segment.
    Triangle t6 {{2,1,1}, {2,3,3}, {2,3,3}}; // Crosses main segment.
    Triangle t7 {{2,2,2}, {2.5,2.5,2.5}, {2,2,2}}; // Have inf num of crosses with main segment.
    Triangle t8 {{2,2,2}, {2,2,2}, {2,2,2}}; // One point. Belongs to main segment.
    Triangle t9 {{0,0,1}, {0,0,1}, {0,0,1}}; // One point. Don't belongs to main segment.

    ASSERT_FALSE (t1.crosses (t2) || t2.crosses (t1));
    ASSERT_FALSE (t1.crosses (t3) || t3.crosses (t1));
    ASSERT_FALSE (t1.crosses (t9) || t9.crosses (t1));

    ASSERT_TRUE (t1.crosses (t1));
    ASSERT_TRUE (t1.crosses (t4) && t4.crosses (t1));
    ASSERT_TRUE (t1.crosses (t5) && t5.crosses (t1));
    ASSERT_TRUE (t1.crosses (t6) && t6.crosses (t1));
    ASSERT_TRUE (t1.crosses (t7) && t7.crosses (t1));
    ASSERT_TRUE (t8.crosses (t8));
    ASSERT_TRUE (t1.crosses (t8) && t8.crosses (t1));
}

TEST( trCrossTests, trNSeg )
{
    Triangle t01 {{1,1,1}, {1,5,1}, {5,1,1}}; // Triangle to cross with segments.
    Triangle t02 {{1,1,2}, {1,1,2}, {2,2,2}}; // Segment in parallel plane.
    Triangle t03 {{1,1,5}, {1,1,3}, {1,1,4}}; // Not crossed segment (doesn't belong to triangles plane).
    Triangle t04 {{1,1,1}, {1,1,5}, {1,1,4}}; // Crosses with triangle vertex (doesn't belong to triangle plane).
    Triangle t05 {{1,1,4}, {3,3,0}, {3,3,0}}; // Crosses with triangle inside area.
    Triangle t06 {{2,2,2}, {2,2,2}, {2,2,2}}; // Point outside triangle.
    Triangle t07 {{2,2,1}, {2,2,1}, {2,2,1}}; // Point inside triangle.
    Triangle t08 {{-1,-1,1}, {-2,-2,1}, {-3,-3,1}}; // Not crossed segment (belongs to triangle plane).
    Triangle t09 {{1,1,1}, {-1,-1,1}, {1,1,1}}; // Crossed with trangle vertex (belongs to triangle plane).
    Triangle t10 {{0,2,1}, {5,0,1}, {0,1,1}}; // Crossed with triangle (belongs to triangle plane).
    Triangle t11 {{0,1,1}, {6,1,1}, {0,1,1}}; // Crossed with triangle side (belongs to trianlge plane).

    ASSERT_FALSE (t01.crosses (t02) || t02.crosses (t01));
    ASSERT_FALSE (t01.crosses (t03) || t03.crosses (t01));
    ASSERT_FALSE (t01.crosses (t06) || t06.crosses (t01));
    ASSERT_FALSE (t01.crosses (t08) || t08.crosses (t01));

    ASSERT_TRUE (t01.crosses (t01));
    ASSERT_TRUE (t01.crosses (t04) && t04.crosses (t01));
    ASSERT_TRUE (t01.crosses (t05) && t05.crosses (t01));
    ASSERT_TRUE (t01.crosses (t07) && t07.crosses (t01));
    ASSERT_TRUE (t01.crosses (t09) && t09.crosses (t01));
    ASSERT_TRUE (t01.crosses (t10) && t10.crosses (t01));
    ASSERT_TRUE (t01.crosses (t11) && t11.crosses (t01));
}

TEST( trCrossTests, trNTr )
{
    Triangle t1 {{1,1,1}, {5,1,1}, {3,4,1}}; // Main triangle
    Triangle t2 {{3,1,-3}, {3,1,3}, {3,4,0}}; // Crosses main triangle by Segment.
    Triangle t3 {{-10,5,0}, {2,3,0}, {1,6,0}}; // In parallel plane with main triangle.
    Triangle t4 {{1,1,1}, {2,1,1}, {1,2,1}}; // In same plane with main triangle - crossed by side.
    Triangle t5 {{1,1,1}, {1,0,0}, {0,1,0}}; // One common point with main triangle.
    Triangle t6 {{1.1,1.1,1}, {2,1.5,1}, {1.5,2,1}}; // Lies inside main triangle.
    Triangle t7 {{-1,-1,1}, {2,-2,1}, {-2,2,1}}; // In same plane with main triangle. Not crossed with main triangle.
    Triangle t8 {{2,2,4}, {5,5,4}, {4,4,1.5}}; // Not crossed with main triangle.

    ASSERT_FALSE (t1.crosses (t3) || t3.crosses (t1));
    ASSERT_FALSE (t1.crosses (t7) || t7.crosses (t1));
    ASSERT_FALSE (t1.crosses (t8) || t8.crosses (t1));

    ASSERT_TRUE (t1.crosses (t1));
    ASSERT_TRUE (t1.crosses (t2) && t2.crosses (t1));
    ASSERT_TRUE (t1.crosses (t4) && t4.crosses (t1));
    ASSERT_TRUE (t1.crosses (t5) && t5.crosses (t1));
    ASSERT_TRUE (t1.crosses (t6) && t6.crosses (t1));
}

} // namespace geom3D