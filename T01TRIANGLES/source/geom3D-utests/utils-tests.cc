

#include <gtest/gtest.h>

#include "geom3D.hh"
#include "geom3D-gen.hh"

namespace geom3D
{

TEST( UtilsTests, ValidationTest )
{
    ASSERT_FALSE (isValid (geom3D::nan));
    ASSERT_FALSE (isValid (geom3D::inf));
    ASSERT_TRUE (isValid (genFP ()));
}

TEST( UtilsTests, ComparisonTest )
{
    fp_t a = genFP ();

    ASSERT_TRUE (fpCmpW {a} == a);
    ASSERT_TRUE (fpCmpW {a} != a + fpCmpW::CMP_PRECISION * 2);
    ASSERT_TRUE (fpCmpW {a} < a + fpCmpW::CMP_PRECISION * 2);
    ASSERT_TRUE (fpCmpW {a} > a - fpCmpW::CMP_PRECISION * 2);
}

TEST( UtilsTests, DeterminantTest )
{
    fp_t det1 = det ({1,1,1},
                     {0,1,1},
                     {0,0,1});

    fp_t det2 = det ({1,0,0},
                     {11,11,0},
                     {38,5,6});

    fp_t det3 = det ({82734,324,12312},
                     {123123,1231,223},
                     {1444,123,323123});

    ASSERT_FLOAT_EQ (det1, 1);
    ASSERT_FLOAT_EQ (det2, 66);
    ASSERT_FLOAT_EQ (det3, 20181070220628.0);
}

} // namespace geom3D