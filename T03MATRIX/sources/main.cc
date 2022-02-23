
#include "linear.hh"
#include <iostream>

int main()
{
    linear::SquareMatrix<int> mat = {
        101,2,1337, 6,5,6, 666,8,9
    };

    std::cout << mat.det() << std::endl;

    return 0;
}
