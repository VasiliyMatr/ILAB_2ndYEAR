
#include <gtest/gtest.h>

#include "linear-gen.hh"

namespace linear
{

TEST(DetTests, DetGenTest)
{
    for (size_t i = 1; i < 200; ++i)
    {
        // std::cout << i << std::endl;
        auto fmat = genUpperTriangularMatirx(std::vector<double>(i, 1));
        linear::randomlyMAddSquareMatCols(linear::shuffleSquareMatCols(fmat));
        ASSERT_TRUE(std::abs(fmat.det() - 1) < 1. / 1000);

        auto imat = genUpperTriangularMatirx(std::vector<int>(i, 1));
        linear::randomlyMAddSquareMatCols(linear::shuffleSquareMatCols(imat));
        ASSERT_TRUE(imat.det() == 1);
    }
}

} // namespace linear
