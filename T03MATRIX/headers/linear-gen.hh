
#ifndef LINEAR_GEN_HH_INCL
#define LINEAR_GEN_HH_INCL

#include "linear.hh"

namespace linear
{

template <size_t SIZE>
MathVector<float, SIZE> genFPMathVec()
{
    MathVector<float, SIZE> toRet{};

    for (size_t i = 0; i < SIZE; ++i)
        toRet[i] = (std::rand() * 2.0 / RAND_MAX - 1) * 100;

    return toRet;
}

} // namespace linear

#endif
