
#include <iostream>
#include "linear.hh"

int main()
{
    linear::SquareMatrix<2, int> mat;

    for(size_t i = 0; i < 2; ++i)
    for(size_t j = 0; j < 2; ++j)
    {
        mat[i][j] = (i + 1) * (j + 1);
        std::cout << "value:" << (i + 1) * (j + 1) << std::endl;
    }

    for(size_t i = 0; i < 2; ++i)
    for(size_t j = 0; j < 2; ++j)
        std::cout << "mat["<<i<<"]["<<j<<"] = " << mat[i][j] << std::endl;

    return 0;
}