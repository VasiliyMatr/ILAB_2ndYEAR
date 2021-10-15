
#include "geom3D.hh"
#include <gtest/gtest.h>

#ifndef GEOM3D_TESTS_HH_INCL
#define GEOM3D_TESTS_HH_INCL

namespace geom3D
{

// Bounds for test values.
extern const fp_t MAX_TEST_FP_VAL;
extern const fp_t MIN_TEST_FP_VAL;

// Generates fp_t value in range:
// [MIN_TEST_FP_VAL; MAX_TEST_FP_VAL] \ {0}
// Used to gen test values.
fp_t genFP();

// Generates Point value for tests.
Point genP();

// Generates Vector value for tests.
Vector genVec();

} // namespace geom3D

#endif // #ifndef GEOM3D_TESTS_HH_INCL