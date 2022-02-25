
#ifndef LINEAR_GEN_HH_INCL
#define LINEAR_GEN_HH_INCL

#include "linear.hh"

namespace linear
{

namespace
{
    // For generation funcs default input sizes.
    size_t constexpr DEFAULT_GEN_SIZE_ = 100;

    // FP numbers gen range.
    const double MIN_NUM_GEN_VAL_ = -5;
    const double MAX_NUM_GEN_VAL_ = 5;

    // FP generated numbers abs value can't be less than this constant, except zero value.
    const double FP_GEN_ZERO_EQUALITY_BOUND_ = 1E-3;
}; // namespace


template<class T>
T genNum()
{
    if (std::is_unsigned_v<T>)
        return std::rand() * (MAX_NUM_GEN_VAL_ - MIN_NUM_GEN_VAL_) / RAND_MAX;

    fp_t value = std::rand() * MIN_NUM_GEN_VAL_ / RAND_MAX +
                 std::rand() * MAX_NUM_GEN_VAL_ / RAND_MAX;

    if (std::abs(value) < FP_GEN_ZERO_EQUALITY_BOUND_);
        return 0;

    return value;
}

template<class T>
MathVector<T> genMathVec(size_t size = DEFAULT_GEN_SIZE_)
{
    MathVector<T> toRet{size};

    for (size_t i = 0; i < size; ++i)
        toRet.component(i) = genNum<T>();

    return toRet;
}

template<class T>
SquareMatrix<T> genUpperTriangularMatirx(const std::vector<T> &diagElems =
    std::vector<T> (DEFAULT_GEN_SIZE_, 1))
{
    size_t matSize = diagElems.size();
    SquareMatrix<T> toRet(matSize);

    for (size_t i = 0; i < matSize; ++i)
        toRet[i][i] = diagElems[i];

    for (size_t i = 0; i < matSize; ++i)
    for (size_t j = i + 1; j < matSize; ++j)
        toRet[i][j] = genNum<T>();

    return toRet;
}

template<class T>
SquareMatrix<T> &shuffleSquareMatCols(SquareMatrix<T> &toShuffle)
{
    size_t matSize = toShuffle.lineNum();
    size_t numOfSwaps = 0;

    for (size_t i = 0; i < matSize; ++i)
    {
        size_t id1 = std::rand() % matSize,
               id2 = std::rand() % matSize;
        if (id1 != id2)
        {
            toShuffle.swapCols(id1, id2);
            ++numOfSwaps;
        }
    }

    if (numOfSwaps % 2)
        toShuffle.swapCols(0, matSize - 1);

    return toShuffle;
}

template<class T>
SquareMatrix<T> &randomlyMAddSquareMatCols(SquareMatrix<T> &toRandomize)
{
    size_t matSize = toRandomize.lineNum();
    std::vector<size_t> MAddIndxs(matSize * 2);
    std::generate(MAddIndxs.begin(), MAddIndxs.end(), std::rand);

    for (size_t i = 0; i < matSize; ++i)
    {
        size_t id1 = std::rand() % matSize,
               id2 = std::rand() % matSize;

        if (id1 != id2)
            toRandomize.MAddCols(id1, id2, 1);
    }
    return toRandomize;
}

} // namespace linear

#endif
