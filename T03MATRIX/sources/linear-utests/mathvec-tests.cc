
#include <gtest/gtest.h>

#include "linear-gen.hh"

namespace linear
{

TEST(dummy, dummy)
{
    std::cout << "test for tests))" << std::endl;

    auto tVal = genFPMathVec<100>();

    for (size_t i = 0; i < 100; ++i)
        std::cout << "val[" << i << "]" << tVal[i] << std::endl;
}

} // namespace linear