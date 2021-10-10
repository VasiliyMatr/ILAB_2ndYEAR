

#include "geom3D-tests.hh"
namespace geom3D
{

TEST( UtilsTests, isValidTest )
{
    ASSERT_FALSE (isValid (geom3D::nan));
    ASSERT_FALSE (isValid (geom3D::inf));
    ASSERT_TRUE (isValid (genFPVal ()));
}

TEST( UtilsTests, isEqualTest )
{
    fp_t a = genFPVal ();
    fp_t b = a;

    ASSERT_TRUE (isEqual (a, b));
    ASSERT_FALSE (isEqual (a, b + 0.01));
}

} // namespace geom3D