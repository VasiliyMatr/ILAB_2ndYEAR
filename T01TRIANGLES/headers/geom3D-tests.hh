
#include "geom3D.hh"
#include <gtest/gtest.h>

#ifndef GEOM3D_TESTS_HH_INCL
#define GEOM3D_TESTS_HH_INCL

namespace geom3D
{

// Bounds for test values.
constexpr fp_t MAX_TEST_FP_VAL = 1000;
constexpr fp_t MIN_TEST_FP_VAL = -1000;

// Generates fp_t value in range:
// [MIN_TEST_FP_VAL; MAX_TEST_FP_VAL] \ {0}
// Used to gen fp_t test values.
inline fp_t genFP()
{
    fp_t positivePart = MAX_TEST_FP_VAL * (static_cast<fp_t> (std::rand ()) / RAND_MAX);
    fp_t negativePart = MIN_TEST_FP_VAL * (static_cast<fp_t> (std::rand ()) / RAND_MAX);

    fp_t sum = positivePart + negativePart;

    return fpCmpW {} == sum ? 1 : sum;
}

// Generates Point for tests.
inline Point genP()
{
    return Point {genFP (), genFP (), genFP ()};
}

// Generates Vector for tests.
inline Vector genVec()
{
    return Vector {genFP (), genFP (), genFP ()};
}

// Generates small triangle for tests.
inline Triangle genSmallTr()
{
    static constexpr fp_t TR_SIZE_LIMIT = 20;
    static constexpr fp_t VECS_FACTOR =
        TR_SIZE_LIMIT / (MAX_TEST_FP_VAL - MIN_TEST_FP_VAL);
    Point A = genP ();
    return Triangle {A, A + genVec () * VECS_FACTOR, A + genVec () * VECS_FACTOR};
}

} // namespace geom3D

#endif // #ifndef GEOM3D_TESTS_HH_INCL