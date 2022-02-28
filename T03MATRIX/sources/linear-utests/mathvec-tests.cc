
#include <gtest/gtest.h>

#include "linear-gen.hh"

namespace linear
{

template <class IMVDerived> class IMVTests : public testing::Test
{
};

TYPED_TEST_SUITE_P(IMVTests);

namespace
{

template <class IMVDerived> using IMVp = std::unique_ptr<IMVDerived>;

template <class IMVDerived> IMVDerived CreateIMVInstance(size_t);

template <> MathVector<int> CreateIMVInstance(size_t size)
{
    return MathVector<int>(size);
}
template <> MathVector<double> CreateIMVInstance(size_t size)
{
    return MathVector<double>(size);
}

template <> SquareMatrix<int> CreateIMVInstance(size_t size)
{
    return SquareMatrix<int>(size);
}
template <> SquareMatrix<double> CreateIMVInstance(size_t size)
{
    return SquareMatrix<double>(size);
}

template <class T> void fillIMV(IMathVector<T> &vec)
{
    for (int i = 0, end = vec.dim(); i < end; ++i)
        vec.at(i) = std::rand() % 0xff;
}

const size_t TEST_IMV_SIZE_ = 30;

} // namespace

TYPED_TEST_P(IMVTests, ElemsAccessTests)
{
    auto mv = CreateIMVInstance<TypeParam>(TEST_IMV_SIZE_);
    ASSERT_TRUE(mv.dim() > 0);

    for (int i = 0, dim = mv.dim(); i < dim; ++i)
        mv.at(i) = i;

    for (int i = 0, dim = mv.dim(); i < dim; ++i)
        ASSERT_TRUE(mv.at(i) == i);
}

TYPED_TEST_P(IMVTests, NegationTest)
{
    auto mv = CreateIMVInstance<TypeParam>(TEST_IMV_SIZE_);
    fillIMV(mv);
    auto mvcp = mv;

    -mv;
    for (int i = 0, dim = mv.dim(); i < dim; ++i)
        ASSERT_TRUE(mv.at(i) == -mvcp.at(i));
}

TYPED_TEST_P(IMVTests, AddNSubTests)
{
    auto mv = CreateIMVInstance<TypeParam>(TEST_IMV_SIZE_);
    fillIMV(mv);

    for (int i = 1, end = 100; i < end; ++i)
    {
        auto addMv = CreateIMVInstance<TypeParam>(TEST_IMV_SIZE_);
        for (int j = 0; j < i; ++j)
            addMv += mv;

        auto subMv = CreateIMVInstance<TypeParam>(TEST_IMV_SIZE_);
        for (int j = 0; j < i; ++j)
            subMv -= mv;

        for (int index = 0, end = addMv.dim(); index < end; ++index)
        {
            ASSERT_TRUE(addMv.at(index) == mv.at(index) * i);
            ASSERT_TRUE(subMv.at(index) == -mv.at(index) * i);
        }
    }
}

TYPED_TEST_P(IMVTests, MulDivTests)
{
    auto mv = CreateIMVInstance<TypeParam>(TEST_IMV_SIZE_);
    auto mvcp = mv;

    for (int mul = -50; mul < 50; ++mul)
    {
        if (mul == 0)
            continue;

        mv *= mul;

        for (int i = 0, end = mv.dim(); i < end; ++i)
            ASSERT_TRUE(mv.at(i) == mvcp.at(i) * mul);

        mv /= mul;

        for (int i = 0, end = mv.dim(); i < end; ++i)
            ASSERT_TRUE(mv.at(i) == mvcp.at(i));
    }
}

REGISTER_TYPED_TEST_SUITE_P(IMVTests, ElemsAccessTests, NegationTest, AddNSubTests, MulDivTests);

using MathVectorImpls = testing::Types<MathVector<int>, MathVector<double>>;

INSTANTIATE_TYPED_TEST_SUITE_P(MathVectorInstance, IMVTests, MathVectorImpls);

using SquareMatrixImpls = testing::Types<SquareMatrix<int>, SquareMatrix<double>>;

INSTANTIATE_TYPED_TEST_SUITE_P(SquareMatrixInstance, IMVTests, SquareMatrixImpls);

} // namespace linear
