
#include "geom3D-tests.hh"
namespace geom3D
{

TEST( PlaneTests, VecNDCtorTest )
{
    Vector norm = genVec ();
    fp_t D = genFP ();

    Plane plane {norm, D};

    ASSERT_FLOAT_EQ (plane.n_.x_, norm.x_);
    ASSERT_FLOAT_EQ (plane.n_.y_, norm.y_);
    ASSERT_FLOAT_EQ (plane.n_.z_, norm.z_);
    ASSERT_FLOAT_EQ (plane.D_, D);
}

TEST( PlaneTests, ThreePtsCtorTest )
{
    Point A {1, 0, 0}, B {0, 1, 0}, C {0, 0, 1};
    Plane plane {A, B, C};
    Vector crossProd = Vector::crossProduct (plane.n_, {1, 1, 1});
    fp_t CPLen = crossProd.len ();

    ASSERT_FLOAT_EQ (0, CPLen);
    ASSERT_FLOAT_EQ (-1, plane.D_);
}

TEST( PlaneTests, ValidationTests )
{
    for (int i = 0; i < 4; ++i)
    {
        fp_t withNan[4] = {}; withNan[i] = nan;
        Plane nanPlane {Vector {withNan[0], withNan[1], withNan[2]}, withNan[3]};
        fp_t withInf[4] = {}; withInf[i] = inf;
        Plane infPlane {Vector {withInf[0], withInf[1], withInf[2]}, withInf[3]};
    
        ASSERT_FALSE (nanPlane.isValid ());
        ASSERT_FALSE (infPlane.isValid ());
    }

    ASSERT_FALSE (Plane {}.isValid ());
}

} // namespace geom3D