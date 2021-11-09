
#include <gtest/gtest.h>

#include "geom3D.hh"
#include "geom3D-split.hh"

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

    return fpCmpW {} == sum ? genFP () : sum;
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

constexpr fp_t SMALL_FACTOR = 20 / (MAX_TEST_FP_VAL - MIN_TEST_FP_VAL);
inline Vector genSmallVec()
{
    return genVec () * SMALL_FACTOR;
}

// Generates small triangle near (0, 0, 0) for tests.
inline Triangle genSmallTr()
{
    Point A = Point {0, 0, 0} + genSmallVec ();
    return Triangle {A, A + genSmallVec (), A + genSmallVec ()};
}

} // namespace geom3D

#endif // #ifndef GEOM3D_TESTS_HH_INCL