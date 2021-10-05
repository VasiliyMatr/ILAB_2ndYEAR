
#include "geom3D-tests.hh"
using namespace geom3D;

TEST( PlaneTests, CtorTest )
{

    Vector norm = genVec ();
    fp_t D = genFPVal ();

    Plane plane {norm, D};

    ASSERT_FLOAT_EQ (plane.normVec_.x_, norm.x_);
    ASSERT_FLOAT_EQ (plane.normVec_.y_, norm.y_);
    ASSERT_FLOAT_EQ (plane.normVec_.z_, norm.z_);
    ASSERT_FLOAT_EQ (plane.D_, D);

}

TEST( PlaneTests, CrossTests )
{

    Plane G {Vector {1, 1, 1}, 1};
    Plane T {Vector {10, 0, 5}, 113};

    // Results to check
    Line cross1 = G | T;
    Line cross2 = T | G;

    // Known points, that should belong to crosses
    Point A {9.3, 11.3, 0};
    Point B {14.3, 16.3, -10};

    // Checking that known points belongs to crosses
    ASSERT_FLOAT_EQ ((A.x_ - cross1.point_.x_) / cross1.dirVec_.x_,
        (A.y_ - cross1.point_.y_) / cross1.point_.y_);
    ASSERT_FLOAT_EQ ((A.y_ - cross1.point_.y_) / cross1.dirVec_.y_,
        (A.z_ - cross1.point_.z_) / cross1.dirVec_.z_);

    ASSERT_FLOAT_EQ ((A.x_ - cross2.point_.x_) / cross2.dirVec_.x_,
        (A.y_ - cross2.point_.y_) / cross2.point_.y_);
    ASSERT_FLOAT_EQ ((A.y_ - cross2.point_.y_) / cross2.dirVec_.y_,
        (A.z_ - cross2.point_.z_) / cross2.dirVec_.z_);

}