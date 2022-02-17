
#include "linear.hh"
#include <iostream>

int main()
{
    containers::Vector vec = {1, 2, 3};

    std::cout << vec[0] << vec[1] << vec[2];

    size_t size = 0;
    std::cin >> size;

    linear::SquareMatrix<int> mat(size);

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
            mat[i][j] = (i + 1) * (j + 1);

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
            std::cout << "mat[" << i << "][" << j << "] = " << mat[i][j] << std::endl;

    return 0;
}