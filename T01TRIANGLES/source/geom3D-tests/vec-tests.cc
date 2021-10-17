
#include "geom3D-tests.hh"
namespace geom3D
{

TEST( VectorTests, ThreeCoordCtorTest )
{
    fp_t x = genFP (), y = genFP (), z = genFP ();
    Vector vec {x, y, z};

    ASSERT_FLOAT_EQ (x, vec.x_);
    ASSERT_FLOAT_EQ (y, vec.y_);
    ASSERT_FLOAT_EQ (z, vec.z_);
}

TEST( VectorTests, TwoPointCtorTest )
{
    Point A = genP ();
    Point B = genP ();
    Vector vec {A, B};

    ASSERT_FLOAT_EQ (B.x_ - A.x_, vec.x_);
    ASSERT_FLOAT_EQ (B.y_ - A.y_, vec.y_);
    ASSERT_FLOAT_EQ (B.z_ - A.z_, vec.z_);
}

TEST( VectorTests, ValidationTests )
{
    for (int i = 0; i < 3; ++i)
    {
        fp_t withNan[3] = {}; withNan[i] = nan;
        Vector nanVec {withNan[0], withNan[1], withNan[2]};
        fp_t withInf[3] = {}; withInf[i] = inf;
        Vector infVec {withInf[0], withInf[1], withInf[2]};

        ASSERT_FALSE (nanVec.isValid ());
        ASSERT_FALSE (infVec.isValid ());
    }

    ASSERT_FALSE (Vector {}.isValid ());
}

namespace
{

using scalOperator_t = std::function<fp_t( fp_t, fp_t )>;

// Function to test Vector class binary operators. Returns true if values are equal.
bool vecOprTest (Vector a, Vector b, Vector c, scalOperator_t scalOperator)
{
    return isEqual (scalOperator (a.x_, b.x_), c.x_) &&
           isEqual (scalOperator (a.y_, b.y_), c.y_) &&
           isEqual (scalOperator (a.z_, b.z_), c.z_);
}

bool vecOprTest (Vector a, fp_t b, Vector c, scalOperator_t scalOperator)
{
    return isEqual (scalOperator (a.x_, b), c.x_) &&
           isEqual (scalOperator (a.y_, b), c.y_) &&
           isEqual (scalOperator (a.z_, b), c.z_);
}

bool vecOprTest (fp_t a, Vector b, Vector c, scalOperator_t scalOperator)
{
    return isEqual (scalOperator (a, b.x_), c.x_) &&
           isEqual (scalOperator (a, b.y_), c.y_) &&
           isEqual (scalOperator (a, b.z_), c.z_);
}

} // namespace

TEST( VectorTests, OperatorUnMinusTest )
{
    Vector a = genVec ();
    Vector b = -a;

    ASSERT_FLOAT_EQ (-(a.x_), b.x_);
    ASSERT_FLOAT_EQ (-(a.y_), b.y_);
    ASSERT_FLOAT_EQ (-(a.z_), b.z_);
}

fp_t plusOperator( fp_t a, fp_t b ) {return a + b;}
TEST( VectorTests, OperatorPlusTest )
{
    Vector a = genVec (), b = genVec ();
    bool status = vecOprTest (a, b, a + b, plusOperator);
    ASSERT_TRUE (status);
}

fp_t minusOperator( fp_t a, fp_t b ) {return a - b;}
TEST( VectorTests, OperatorMinusTest )
{
    Vector a = genVec (), b = genVec ();
    bool status = vecOprTest (a, b, a - b, minusOperator);
    ASSERT_TRUE (status);
}

fp_t mulOperator( fp_t a, fp_t b ) {return a * b;}
TEST( VectorTests, OperatorMulTest )
{
    Vector a = genVec ();
    fp_t b = genFP ();

    bool status = vecOprTest (a, b, a * b, mulOperator);
    ASSERT_TRUE (status);

    status = vecOprTest (b, a, b * a, mulOperator);
    ASSERT_TRUE (status);
}

fp_t divOperator( fp_t a, fp_t b ) {return a / b;}
TEST( VectorTests, OperatorDivTest )
{
    Vector a = genVec ();
    fp_t b = genFP ();
    bool status = vecOprTest (a, b, a / b, divOperator);
    ASSERT_TRUE (status);
    ASSERT_FALSE ((genVec () / 0).isValid ());
}

TEST( VectorTests, equalCmpOperatorTest )
{
    Vector a = genVec ();

    ASSERT_TRUE (a == a);
    ASSERT_FALSE (a + e1 * FP_CMP_PRECISION * 2 == a);
    ASSERT_FALSE (a + e2 * FP_CMP_PRECISION * 2 == a);
    ASSERT_FALSE (a + e3 * FP_CMP_PRECISION * 2 == a);
}

TEST( VectorTests, LenTest )
{
    Vector a = genVec ();
    fp_t sqLen = a.x_*a.x_ + a.y_*a.y_ + a.z_*a.z_;
    ASSERT_FLOAT_EQ (sqLen, a.sqLen ());
    ASSERT_FLOAT_EQ (std::sqrt (sqLen), a.len ());
}

TEST( VectorTests, ScalarProdTest )
{
    fp_t scalProd1 = Vector::scalarProduct (e1 + e2 * 2 + e3 * 3, -e2 - e3 * 1.5);
    fp_t scalProd2 = Vector::scalarProduct (e3 * 3 - e2 * 4, e1 * 100 + e2 * 3.75 + e3 * 6);

    ASSERT_FLOAT_EQ (scalProd1, -6.5);
    ASSERT_FLOAT_EQ (scalProd2, 3);
}

TEST( VectorTests, CrossProdTest )
{
    Vector crossProd1 = Vector::crossProduct (e1 + e2 + e3, e1);
    Vector crossProd2 = Vector::crossProduct (e1 + e2 + e3, e2);
    Vector crossProd3 = Vector::crossProduct (e1 + e2 + e3, e3);

    ASSERT_TRUE (crossProd1 == -e3 + e2);
    ASSERT_TRUE (crossProd2 == e3 - e1);
    ASSERT_TRUE (crossProd3 == -e2 + e1); 
}

} // namespace geom3D