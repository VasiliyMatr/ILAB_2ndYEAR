
#include "geom3D-tests.hh"
using namespace geom3D;

TEST( PointTests, NanCoordsTests )
{
    Point A {geom3D::nan, genFPVal (), genFPVal ()};
    Point B {genFPVal (), geom3D::nan, genFPVal ()};
    Point C {genFPVal (), genFPVal (), geom3D::nan};

    ASSERT_FALSE (A.isValid ());
    ASSERT_FALSE (B.isValid ());
    ASSERT_FALSE (C.isValid ());
}

TEST( PointTests, InfCoordsTests )
{
    Point A {geom3D::inf, genFPVal (), genFPVal ()};
    Point B {genFPVal (), geom3D::inf, genFPVal ()};
    Point C {genFPVal (), genFPVal (), geom3D::inf};

    ASSERT_FALSE (A.isValid ());
    ASSERT_FALSE (B.isValid ());
    ASSERT_FALSE (C.isValid ());
}

TEST( PointTests, DefaultCtorTest )
{
    // Default ctor should give invalid value.
    ASSERT_FALSE (Point {}.isValid ());
}

TEST( PointTests, ThreeCoordCtorTests )
{
    fp_t x = genFPVal (), y = genFPVal (), z = genFPVal ();
    Point P {x, y, z};

    ASSERT_FLOAT_EQ (x, P.x_);
    ASSERT_FLOAT_EQ (y, P.y_);
    ASSERT_FLOAT_EQ (z, P.z_);
    ASSERT_TRUE (P.isValid ());
}