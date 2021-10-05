
#include "geom3D.hh"
#include <gtest/gtest.h>

#ifndef GEOM3D_TESTS_HH_INCL
#define GEOM3D_TESTS_HH_INCL

// Bounds for test values.
extern const geom3D::fp_t MAX_TEST_FP_VAL;
extern const geom3D::fp_t MIN_TEST_FP_VAL;

// Generates fp_t value in range:
// [MIN_TEST_FP_VAL; MAX_TEST_FP_VAL] \ {0}
// Used to gen test values.
geom3D::fp_t genFPVal();

// Generates Vector value for tests.
geom3D::Vector genVec();

#endif // #ifndef GEOM3D_TESTS_HH_INCL