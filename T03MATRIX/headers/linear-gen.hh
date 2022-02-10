
#ifndef LINEAR_GEN_HH_INCL
#define LINEAR_GEN_HH_INCL

#include "linear.hh"

namespace linear
{

template <size_t SIZE>
MathVector<float> genFPMathVec()
{
    MathVector<float> toRet{SIZE};

    for (size_t i = 0; i < SIZE; ++i)
        toRet.component(i) = (std::rand() * 2.0 / RAND_MAX - 1) * 100;

    return toRet;
}

} // namespace linear

#endif
