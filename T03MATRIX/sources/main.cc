
#include "linear.hh"
#include <iostream>

int main()
{
    size_t matSize = 0;
    std::cin >> matSize;
    linear::SquareMatrix<int> mat(matSize);
    std::cin >> mat;

    std::cout << mat.det();

    return 0;
}
