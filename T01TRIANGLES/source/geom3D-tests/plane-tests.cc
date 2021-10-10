
#include "geom3D-tests.hh"
namespace geom3D
{

TEST( PlaneTests, CtorTest )
{
    Vector norm = genVec ();
    fp_t D = genFP ();

    Plane plane {norm, D};

    ASSERT_FLOAT_EQ (plane.normVec_.x_, norm.x_);
    ASSERT_FLOAT_EQ (plane.normVec_.y_, norm.y_);
    ASSERT_FLOAT_EQ (plane.normVec_.z_, norm.z_);
    ASSERT_FLOAT_EQ (plane.D_, D);
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

TEST( PlaneTests, CrossTests )
{
    Plane G {Vector {1, 1, 1}, 1};
    Plane T {Vector {10, 0, 5}, 113};

    // Results to check
    Line cross1 = G | T;
    Line cross2 = T | G;

    // Known points, that should belong to crosses
    Point A {11.3, -10.3, 0};
    Point B {14.3, -7.3, -6};

    // Checking that known points belongs to crosses
    ASSERT_FLOAT_EQ ((A.x_ - cross1.point_.x_) / cross1.dirVec_.x_,
        (A.y_ - cross1.point_.y_) / cross1.dirVec_.y_);
    ASSERT_FLOAT_EQ ((A.y_ - cross1.point_.y_) / cross1.dirVec_.y_,
        (A.z_ - cross1.point_.z_) / cross1.dirVec_.z_);

    ASSERT_FLOAT_EQ ((B.x_ - cross2.point_.x_) / cross2.dirVec_.x_,
        (B.y_ - cross2.point_.y_) / cross2.dirVec_.y_);
    ASSERT_FLOAT_EQ ((B.y_ - cross2.point_.y_) / cross2.dirVec_.y_,
        (B.z_ - cross2.point_.z_) / cross2.dirVec_.z_);
}

} // namespace geom3D