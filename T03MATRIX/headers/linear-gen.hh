
#ifndef LINEAR_GEN_HH_INCL
#define LINEAR_GEN_HH_INCL

#include "linear.hh"

namespace linear
{

const float MIN_FP_GEN_VAL_ = -100;
const float MAX_FP_GEN_VAL_ = 100;

float genFP()
{
    return std::rand() * MIN_FP_GEN_VAL_ / RAND_MAX +
           std::rand() * MAX_FP_GEN_VAL_ / RAND_MAX;
}

template <size_t SIZE>
MathVector<float> genFPMathVec()
{
    MathVector<float> toRet{SIZE};

    for (size_t i = 0; i < SIZE; ++i)
        toRet.component(i) = genFP();

    return toRet;
}

} // namespace linear

#endif
