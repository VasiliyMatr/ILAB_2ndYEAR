
#include <gtest/gtest.h>

#include "geom3D-gen.hh"
#include "geom3D.hh"

namespace geom3D
{

TEST(VectorTests, ThreeCoordCtorTest)
{
    fp_t x = genFP(), y = genFP(), z = genFP();
    Vector vec{x, y, z};

    ASSERT_FLOAT_EQ(x, vec[X]);
    ASSERT_FLOAT_EQ(y, vec[Y]);
    ASSERT_FLOAT_EQ(z, vec[Z]);
}

TEST(VectorTests, TwoPointCtorTest)
{
    Point A = genP();
    Point B = genP();
    Vector vec{A, B};

    ASSERT_FLOAT_EQ(B[X] - A[X], vec[X]);
    ASSERT_FLOAT_EQ(B[Y] - A[Y], vec[Y]);
    ASSERT_FLOAT_EQ(B[Z] - A[Z], vec[Z]);
}

TEST(VectorTests, ValidationTest)
{
    for (int i = 0; i < 3; ++i)
    {
        fp_t withNan[3] = {};
        withNan[i] = nan;
        Vector nanVec{withNan[0], withNan[1], withNan[2]};
        fp_t withInf[3] = {};
        withInf[i] = inf;
        Vector infVec{withInf[0], withInf[1], withInf[2]};

        ASSERT_FALSE(nanVec.isValid());
        ASSERT_FALSE(infVec.isValid());
    }

    ASSERT_FALSE(Vector{}.isValid());
}

namespace
{

using scalOperator_t = std::function<fp_t(fp_t, fp_t)>;

// Function to test Vector class binary operators. Returns true if values are equal.
bool vecOprTest(Vector a, Vector b, Vector res, scalOperator_t scalOperator)
{
    return fpCmpW{res[X]} == scalOperator(a[X], b[X]) && fpCmpW{res[Y]} == scalOperator(a[Y], b[Y]) &&
           fpCmpW{res[Z]} == scalOperator(a[Z], b[Z]);
}

bool vecOprTest(Vector a, fp_t b, Vector res, scalOperator_t scalOperator)
{
    return fpCmpW{res[X]} == scalOperator(a[X], b) && fpCmpW{res[Y]} == scalOperator(a[Y], b) &&
           fpCmpW{res[Z]} == scalOperator(a[Z], b);
}

bool vecOprTest(fp_t a, Vector b, Vector res, scalOperator_t scalOperator)
{
    return fpCmpW{res[X]} == scalOperator(a, b[X]) && fpCmpW{res[Y]} == scalOperator(a, b[Y]) &&
           fpCmpW{res[Z]} == scalOperator(a, b[Z]);
}

} // namespace

TEST(VectorTests, OperatorUnMinusTest)
{
    Vector a = genVec();
    Vector b = -a;

    ASSERT_FLOAT_EQ(-(a[X]), b[X]);
    ASSERT_FLOAT_EQ(-(a[Y]), b[Y]);
    ASSERT_FLOAT_EQ(-(a[Z]), b[Z]);
}

fp_t plusOperator(fp_t a, fp_t b)
{
    return a + b;
}
TEST(VectorTests, OperatorPlusTest)
{
    Vector a = genVec(), b = genVec();
    bool status = vecOprTest(a, b, a + b, plusOperator);
    ASSERT_TRUE(status);
}

fp_t minusOperator(fp_t a, fp_t b)
{
    return a - b;
}
TEST(VectorTests, OperatorMinusTest)
{
    Vector a = genVec(), b = genVec();
    bool status = vecOprTest(a, b, a - b, minusOperator);
    ASSERT_TRUE(status);
}

fp_t mulOperator(fp_t a, fp_t b)
{
    return a * b;
}
TEST(VectorTests, OperatorMulTest)
{
    Vector a = genVec();
    fp_t b = genFP();

    bool status = vecOprTest(a, b, a * b, mulOperator);
    ASSERT_TRUE(status);

    status = vecOprTest(b, a, b * a, mulOperator);
    ASSERT_TRUE(status);
}

fp_t divOperator(fp_t a, fp_t b)
{
    return a / b;
}
TEST(VectorTests, OperatorDivTest)
{
    Vector a = genVec();
    fp_t b = genFP();
    bool status = vecOprTest(a, b, a / b, divOperator);
    ASSERT_TRUE(status);
    ASSERT_FALSE((genVec() / 0).isValid());
}

TEST(VectorTests, equalCmpOperatorTest)
{
    Vector a = genVec();

    ASSERT_TRUE(a == a);
    ASSERT_FALSE(a + Vector::e1() * fpCmpW<valueOrder_t::FIRST>::CMP_PRECISION * 2 == a);
    ASSERT_FALSE(a + Vector::e2() * fpCmpW<valueOrder_t::FIRST>::CMP_PRECISION * 2 == a);
    ASSERT_FALSE(a + Vector::e3() * fpCmpW<valueOrder_t::FIRST>::CMP_PRECISION * 2 == a);
}

TEST(VectorTests, LenTest)
{
    Vector a = genVec();
    fp_t sqLen = a[X] * a[X] + a[Y] * a[Y] + a[Z] * a[Z];
    ASSERT_FLOAT_EQ(sqLen, a.sqLen());
    ASSERT_FLOAT_EQ(std::sqrt(sqLen), a.len());
}

TEST(VectorTests, ScalarProdTest)
{
    fp_t scalProd1 =
        Vector::scalarProduct(Vector::e1() + Vector::e2() * 2 + Vector::e3() * 3, -Vector::e2() - Vector::e3() * 1.5);

    fp_t scalProd2 = Vector::scalarProduct(Vector::e3() * 3 - Vector::e2() * 4,
                                           Vector::e1() * 100 + Vector::e2() * 3.75 + Vector::e3() * 6);

    ASSERT_FLOAT_EQ(scalProd1, -6.5);
    ASSERT_FLOAT_EQ(scalProd2, 3);
}

TEST(VectorTests, CrossProdTest)
{
    Vector crossProd1 = Vector::crossProduct(Vector::e1() + Vector::e2() + Vector::e3(), Vector::e1());

    Vector crossProd2 = Vector::crossProduct(Vector::e1() + Vector::e2() + Vector::e3(), Vector::e2());

    Vector crossProd3 = Vector::crossProduct(Vector::e1() + Vector::e2() + Vector::e3(), Vector::e3());

    ASSERT_TRUE(crossProd1 == Vector::e2() - Vector::e3());
    ASSERT_TRUE(crossProd2 == Vector::e3() - Vector::e1());
    ASSERT_TRUE(crossProd3 == Vector::e1() - Vector::e2());
}

} // namespace geom3D
