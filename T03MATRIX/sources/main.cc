
#include "linear-gen.hh"
#include "linear.hh"
#include <iostream>

int main()
{
    size_t matSize = 0;
    std::cin >> matSize;

    auto mat = linear::genUpperTriangularMatirx<int>(std::vector<int>(matSize, 1));
    linear::randomlyMAddSquareMatCols(linear::shuffleSquareMatCols(mat));

    std::cout << mat.det() << std::endl;

    return 0;
}
