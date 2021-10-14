
#include "geom3D-tests.hh"
using namespace geom3D;

// Bounds for test values.
const fp_t MAX_TEST_FP_VAL = 1000;
const fp_t MIN_TEST_FP_VAL = -1000;

// Generates fp_t value in range:
// [MIN_TEST_FP_VAL; MAX_TEST_FP_VAL] \ {0}
// Used to gen test values.
fp_t genFP()
{
    fp_t positivePart = MAX_TEST_FP_VAL * (static_cast<fp_t> (std::rand ()) / RAND_MAX);
    fp_t negativePart = MIN_TEST_FP_VAL * (static_cast<fp_t> (std::rand ()) / RAND_MAX);

    fp_t sum = positivePart + negativePart;

    return isEqual (sum, 0) ? 1 : sum;
}

// Generates Vector value for tests.
Vector genVec()
{
    return Vector {genFP (), genFP (), genFP ()};
}

int main( int argc, char ** argv )
{
    testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}