
#include "geom3D.hh"
#include <gtest/gtest.h>

TEST( PointTest, InvalidDataTest )
{
    // Points with nan fields.
    geom3D::Point A {geom3D::nan, 1.0, 2.0};
    geom3D::Point B {1.0, geom3D::nan, 2.0};
    geom3D::Point C {1.0, 2.0, geom3D::nan};
    // Points with inf fields.
    geom3D::Point D {geom3D::inf, 1.0, 2.0};
    geom3D::Point E {1.0, geom3D::inf, 2.0};
    geom3D::Point F {1.0, 2.0, geom3D::inf};

    // Invalid default constr point.
    geom3D::Point G {};

    ASSERT_TRUE (!A.isValid ());
    ASSERT_TRUE (!B.isValid ());
    ASSERT_TRUE (!C.isValid ());
    ASSERT_TRUE (!D.isValid ());
    ASSERT_TRUE (!E.isValid ());
    ASSERT_TRUE (!F.isValid ());
    ASSERT_TRUE (!G.isValid ());
}

TEST( PointTest, ValidDataTest )
{
    // generating some data in choosen range
    for (size_t i = 0; i < 1024; ++i)
    {
        geom3D::fp_t x = (geom3D::fp_t)i * std::rand () / RAND_MAX;
        geom3D::fp_t y = (geom3D::fp_t)i * std::rand () / RAND_MAX;
        geom3D::fp_t z = (geom3D::fp_t)i * std::rand () / RAND_MAX;

        geom3D::Point P { x, y, z };

        ASSERT_TRUE (P.x_ == x);
        ASSERT_TRUE (P.y_ == y);
        ASSERT_TRUE (P.z_ == z);
        ASSERT_TRUE (P.isValid ());
    }
}

int main( int argc, char ** argv )
{
    testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}