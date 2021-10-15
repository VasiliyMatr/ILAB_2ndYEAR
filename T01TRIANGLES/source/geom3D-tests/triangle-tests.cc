
#include "geom3D-tests.hh"

namespace geom3D
{

TEST( trCrossTests, segNSeg )
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

    ASSERT_TRUE (t1.isInter (t1));
    ASSERT_TRUE (t1.isInter (t4) && t4.isInter (t1));
    ASSERT_TRUE (t1.isInter (t5) && t5.isInter (t1));
    ASSERT_TRUE (t1.isInter (t6) && t6.isInter (t1));
    ASSERT_TRUE (t1.isInter (t7) && t7.isInter (t1));
    ASSERT_TRUE (t8.isInter (t8));
    ASSERT_TRUE (t1.isInter (t8) && t8.isInter (t1));
}

TEST( trCrossTests, trNSeg )
{
    TriangleInfo t01 {{1,1,1}, {1,5,1}, {5,1,1}}; // Triangle to cross with segments.
    TriangleInfo t02 {{1,1,2}, {1,1,2}, {2,2,2}}; // Segment in parallel plane.
    TriangleInfo t03 {{1,1,5}, {1,1,3}, {1,1,4}}; // Not crossed segment (doesn't belong to triangles plane).
    TriangleInfo t04 {{1,1,1}, {1,1,5}, {1,1,4}}; // Crosses with triangle vertex (doesn't belong to triangle plane).
    TriangleInfo t05 {{1,1,4}, {3,3,0}, {3,3,0}}; // Crosses with triangle inside area.
    TriangleInfo t06 {{2,2,2}, {2,2,2}, {2,2,2}}; // Point outside triangle.
    TriangleInfo t07 {{2,2,1}, {2,2,1}, {2,2,1}}; // Point inside triangle.
    TriangleInfo t08 {{-1,-1,1}, {-2,-2,1}, {-3,-3,1}}; // Not crossed segment (belongs to triangle plane).
    TriangleInfo t09 {{1,1,1}, {-1,-1,1}, {1,1,1}}; // Crossed with trangle vertex (belongs to triangle plane).
    TriangleInfo t10 {{0,2,1}, {5,0,1}, {0,1,1}}; // Crossed with triangle (belongs to triangle plane).
    TriangleInfo t11 {{0,1,1}, {6,1,1}, {0,1,1}}; // Crossed with triangle side (belongs to trianlge plane).

    ASSERT_FALSE (t01.isInter (t02) || t02.isInter (t01));
    ASSERT_FALSE (t01.isInter (t03) || t03.isInter (t01));
    ASSERT_FALSE (t01.isInter (t06) || t06.isInter (t01));
    ASSERT_FALSE (t01.isInter (t08) || t08.isInter (t01));

    ASSERT_TRUE (t01.isInter (t01));
    ASSERT_TRUE (t01.isInter (t04) && t04.isInter (t01));
    ASSERT_TRUE (t01.isInter (t05) && t05.isInter (t01));
    ASSERT_TRUE (t01.isInter (t07) && t07.isInter (t01));
    ASSERT_TRUE (t01.isInter (t09) && t09.isInter (t01));
    ASSERT_TRUE (t01.isInter (t10) && t10.isInter (t01));
    ASSERT_TRUE (t01.isInter (t11) && t11.isInter (t01));
}

TEST( trCrossTests, trNTr )
{
    TriangleInfo t1 {{1,1,1}, {5,1,1}, {3,4,1}}; // Main triangle
    TriangleInfo t2 {{3,1,-3}, {3,1,3}, {3,4,0}}; // Crosses main triangle by Segment.
    TriangleInfo t3 {{-10,5,0}, {2,3,0}, {1,6,0}}; // In parallel plane with main triangle.
    TriangleInfo t4 {{1,1,1}, {2,1,1}, {1,2,1}}; // In same plane with main triangle - crossed by side.
    TriangleInfo t5 {{1,1,1}, {1,0,0}, {0,1,0}}; // One common point with main triangle.
    TriangleInfo t6 {{1.1,1.1,1}, {2,1.5,1}, {1.5,2,1}}; // Lies inside main triangle.
    TriangleInfo t7 {{-1,-1,1}, {2,-2,1}, {-2,2,1}}; // In same plane with main triangle. Not crossed with main triangle.
    TriangleInfo t8 {{2,2,4}, {5,5,4}, {4,4,1.5}}; // Not crossed with main triangle.

    ASSERT_FALSE (t1.isInter (t3) || t3.isInter (t1));
    ASSERT_FALSE (t1.isInter (t7) || t7.isInter (t1));
    ASSERT_FALSE (t1.isInter (t8) || t8.isInter (t1));

    ASSERT_TRUE (t1.isInter (t1));
    ASSERT_TRUE (t1.isInter (t2) && t2.isInter (t1));
    ASSERT_TRUE (t1.isInter (t4) && t4.isInter (t1));
    ASSERT_TRUE (t1.isInter (t5) && t5.isInter (t1));
    ASSERT_TRUE (t1.isInter (t6) && t6.isInter (t1));
}

} // namespace geom3D