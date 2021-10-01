
#include "geom3D.hh"
#include <gtest/gtest.h>

TEST( PointTest, ValidationTests )
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

    ASSERT_FALSE (A.isValid ());
    ASSERT_FALSE (B.isValid ());
    ASSERT_FALSE (C.isValid ());
    ASSERT_FALSE (D.isValid ());
    ASSERT_FALSE (E.isValid ());
    ASSERT_FALSE (F.isValid ());
    ASSERT_FALSE (G.isValid ());
}

TEST( PointTest, ValidDataTests )
{
    // Generating some data in choosen range.
    for (geom3D::fp_t i = 0; i < 1024; ++i)
    {
        geom3D::fp_t x = i * std::rand () / RAND_MAX;
        geom3D::fp_t y = i * std::rand () / RAND_MAX;
        geom3D::fp_t z = i * std::rand () / RAND_MAX;

        geom3D::Point P { x, y, z };

        ASSERT_TRUE (P.x_ == x);
        ASSERT_TRUE (P.y_ == y);
        ASSERT_TRUE (P.z_ == z);
        ASSERT_TRUE (P.isValid ());
    }
}

TEST( VectorTest, ValidationTests )
{
    std::vector<geom3D::Vector> invalidVectors;

    geom3D::Vector validVector (1, 2, 3);
    ASSERT_TRUE (validVector.isValid ());

    // Generating invalid data.
    for (int i = 0; i < 6; ++i)
    {
        geom3D::fp_t coords[3];
        coords[i] = i < 3 ? geom3D::nan : geom3D::inf;
        coords[(i + 1) % 3] = 1;
        coords[(i + 2) % 3] = 2;

        invalidVectors.emplace_back (geom3D::Vector (coords[0], coords[1], coords[2]));
    }

    for (auto iter = invalidVectors.begin (); iter != invalidVectors.end (); iter++)
    {
        //! Should have default 5-rule methods,
        //! or new tests should be added for custom ones.

        // TODO: Add default methods usage checks.

        geom3D::fp_t fp_value = 3.1415926535;
        geom3D::Vector invalidCopy = *iter;

        ASSERT_FALSE (invalidCopy.isValid ());
        ASSERT_FALSE ((invalidCopy + validVector).isValid ());
        ASSERT_FALSE ((invalidCopy - validVector).isValid ());
        ASSERT_FALSE ((invalidCopy * fp_value).isValid ());
        ASSERT_FALSE ((invalidCopy / fp_value).isValid ());
        ASSERT_FALSE ((validVector * geom3D::nan).isValid ());
        ASSERT_FALSE ((validVector / geom3D::nan).isValid ());
        ASSERT_FALSE ((validVector * geom3D::inf).isValid ());
        ASSERT_FALSE ((validVector / geom3D::inf).isValid ());
        ASSERT_FALSE ((invalidCopy += validVector).isValid ());
        ASSERT_FALSE ((invalidCopy -= validVector).isValid ());
        ASSERT_FALSE (geom3D::isValid (geom3D::Vector::scalarProduct
            (invalidCopy, validVector)));
        ASSERT_FALSE (geom3D::isValid (geom3D::Vector::scalarProduct
            (validVector, invalidCopy)));
        ASSERT_FALSE ((geom3D::Vector::crossProduct
            (invalidCopy, validVector)).isValid ());
        ASSERT_FALSE ((geom3D::Vector::crossProduct
            (validVector, invalidCopy)).isValid ());
    }
}

TEST( VectorTest, ValidDataTests )
{
    geom3D::Vector e1 {1, 0, 0};
    geom3D::Vector e2 {0, 1, 0};
    geom3D::Vector e3 {0, 0, 1};

    geom3D::Vector a = e1 + e2 * 2 + e3 * 3;
    geom3D::Vector b = -e1 * 3 - e2 * 2 - e3;

    ASSERT_TRUE (a.x_ == 1 && a.y_ == 2 && a.z_ == 3);
    ASSERT_TRUE (b.x_ == -3 && b.y_ == -2 && b.z_ == -1);

    a += b;
    ASSERT_TRUE (a.x_ == -2 && a.y_ == 0 && a.z_ == 2);
    b -= a;
    ASSERT_TRUE (b.x_ == -1 && b.y_ == -2 && b.z_ == -3);
    a *= 2;
    ASSERT_TRUE (a.x_ == -4 && a.y_ == 0 && a.z_ == 4);
    b /= 2;
    ASSERT_TRUE (b.x_ == -0.5 && b.y_ == -1 && b.z_ == -1.5);

    geom3D::Vector c = e1 + e2 + e3;
    geom3D::Vector d = e1 + e3 * 2;
    geom3D::Vector crossProduct = geom3D::Vector::crossProduct (c, d);
    ASSERT_TRUE (c.squareLen () == 3);
    ASSERT_TRUE (c.len () == std::sqrt (3));
    ASSERT_TRUE (crossProduct.x_ == 2 && crossProduct.y_ == 3 && crossProduct.z_ == 1);
    ASSERT_TRUE (geom3D::Vector::scalarProduct (c, d) == 3);

    c = c.normalized ();
    ASSERT_TRUE (c.x_ == 1/std::sqrt (3) &&
                 c.y_ == 1/std::sqrt (3) &&
                 c.z_ == 1/std::sqrt (3));

}

int main( int argc, char ** argv )
{
    testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}