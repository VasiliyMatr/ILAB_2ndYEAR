
#include "geom3D.hh"
#include <gtest/gtest.h>

using namespace geom3D;

namespace
{

// Bounds for test values.
const fp_t MAX_TEST_FP_VAL = std::numeric_limits<fp_t>::max () / 1e34;
const fp_t MIN_TEST_FP_VAL = std::numeric_limits<fp_t>::lowest () / 1e34;

// Generates fp_t value in range:
// [MIN_TEST_FP_VAL; MAX_TEST_FP_VAL] \ {0}
// Used to gen test values.
fp_t genFPVal()
{
    fp_t positivePart = MAX_TEST_FP_VAL * (static_cast<fp_t> (std::rand ()) / RAND_MAX);
    fp_t negativePart = MIN_TEST_FP_VAL * (static_cast<fp_t> (std::rand ()) / RAND_MAX);

    fp_t sum = positivePart + negativePart;

    return isEqual (sum, 0) ? 1 : sum;
}

// Generates Vector value for tests.
Vector genVec()
{
    return Vector {genFPVal (), genFPVal (), genFPVal ()};
}

} // namespace

TEST( UtilsTests, isValidTest )
{
    ASSERT_FALSE (isValid (geom3D::nan));
    ASSERT_FALSE (isValid (geom3D::inf));
    ASSERT_TRUE (isValid (genFPVal ()));
}

TEST( UtilsTests, isEqualTest )
{
    fp_t a = genFPVal ();
    fp_t b = a;

    ASSERT_TRUE (isEqual (a, b));
    ASSERT_FALSE (isEqual (a, b + 0.01));
}

TEST( PointTests, NanCoordsTests )
{
    Point A {geom3D::nan, genFPVal (), genFPVal ()};
    Point B {genFPVal (), geom3D::nan, genFPVal ()};
    Point C {genFPVal (), genFPVal (), geom3D::nan};

    ASSERT_FALSE (A.isValid ());
    ASSERT_FALSE (B.isValid ());
    ASSERT_FALSE (C.isValid ());
}

TEST( PointTests, InfCoordsTests )
{
    Point A {geom3D::inf, genFPVal (), genFPVal ()};
    Point B {genFPVal (), geom3D::inf, genFPVal ()};
    Point C {genFPVal (), genFPVal (), geom3D::inf};

    ASSERT_FALSE (A.isValid ());
    ASSERT_FALSE (B.isValid ());
    ASSERT_FALSE (C.isValid ());
}

TEST( PointTests, DefaultCtorTest )
{
    // Default ctor should give invalid value.
    ASSERT_FALSE (Point {}.isValid ());
}

TEST( PointTests, ThreeCoordCtorTests )
{
    fp_t x = genFPVal (), y = genFPVal (), z = genFPVal ();
    Point P {x, y, z};

    ASSERT_FLOAT_EQ (x, P.x_);
    ASSERT_FLOAT_EQ (y, P.y_);
    ASSERT_FLOAT_EQ (z, P.z_);
    ASSERT_TRUE (P.isValid ());
}

TEST( VectorTests, NanCoordsTests )
{
    Vector a {geom3D::nan, genFPVal (), genFPVal ()};
    Vector b {genFPVal (), geom3D::nan, genFPVal ()};
    Vector c {genFPVal (), genFPVal (), geom3D::nan};

    ASSERT_FALSE (a.isValid ());
    ASSERT_FALSE (b.isValid ());
    ASSERT_FALSE (c.isValid ());
}

TEST( VectorTests, InfCoordsTests )
{
    Vector a {geom3D::inf, genFPVal (), genFPVal ()};
    Vector b {genFPVal (), geom3D::inf, genFPVal ()};
    Vector c {genFPVal (), genFPVal (), geom3D::inf};

    ASSERT_FALSE (a.isValid ());
    ASSERT_FALSE (b.isValid ());
    ASSERT_FALSE (c.isValid ());
}

TEST( VectorTests, DefaultCtorTest )
{
    Vector a{};
    ASSERT_FALSE (a.isValid ());
}

TEST( VectorTests, ThreeCoordCtorTest )
{
    fp_t x = genFPVal (), y = genFPVal (), z = genFPVal ();
    Vector vec {x, y, z};

    ASSERT_TRUE (vec.isValid ());
    ASSERT_FLOAT_EQ (x, vec.x_);
    ASSERT_FLOAT_EQ (y, vec.y_);
    ASSERT_FLOAT_EQ (z, vec.z_);
}

TEST( VectorTests, CopyCtorTest )
{
    fp_t x = genFPVal (), y = genFPVal (), z = genFPVal ();
    Vector a {x, y, z};
    Vector b = a;

    ASSERT_TRUE (b.isValid ());
    ASSERT_FLOAT_EQ (x, b.x_);
    ASSERT_FLOAT_EQ (y, b.y_);
    ASSERT_FLOAT_EQ (z, b.z_);
}

TEST( VectorTests, MoveCtorTest )
{
    fp_t x = genFPVal (), y = genFPVal (), z = genFPVal ();
    Vector vec = Vector {x, y, z};

    ASSERT_TRUE (vec.isValid ());
    ASSERT_FLOAT_EQ (x, vec.x_);
    ASSERT_FLOAT_EQ (y, vec.y_);
    ASSERT_FLOAT_EQ (z, vec.z_);
}

TEST( VectorTests, CopyAssignTest )
{
    Vector a = genVec ();
    Vector b {};
    b = a;

    ASSERT_TRUE (b.isValid ());
    ASSERT_FLOAT_EQ (a.x_, b.x_);
    ASSERT_FLOAT_EQ (a.y_, b.y_);
    ASSERT_FLOAT_EQ (a.z_, b.z_);
}

TEST( VectorTests, MoveAssignTest )
{
    fp_t x = genFPVal (), y = genFPVal (), z = genFPVal ();
    Vector vec{};
    vec = Vector {x, y, z};

    ASSERT_TRUE (vec.isValid ());
    ASSERT_FLOAT_EQ (x, vec.x_);
    ASSERT_FLOAT_EQ (y, vec.y_);
    ASSERT_FLOAT_EQ (z, vec.z_);
}

TEST( VectorTests, OperatorUnMinusTest )
{
    Vector a = genVec ();
    Vector b = -a;

    ASSERT_TRUE (b.isValid ());
    ASSERT_FLOAT_EQ (-(a.x_), b.x_);
    ASSERT_FLOAT_EQ (-(a.y_), b.y_);
    ASSERT_FLOAT_EQ (-(a.z_), b.z_);
}

namespace
{

// Vector class binary operators test status.
enum class vecOprTestStatus_t
{
    OK                  , // Test passed.
    NOT_VALID_RESULT    , // Got not valid result
    WRONG_CALC      // Wrong calculated result.
};

using scalOperator_t = std::function<fp_t( fp_t, fp_t )>;

// Function to test Vector class binary operators.
vecOprTestStatus_t vecOprTest (Vector a, Vector b, Vector c, scalOperator_t scalOperator)
{
    if (!c.isValid ())
        return vecOprTestStatus_t::NOT_VALID_RESULT;

    if (isEqual (scalOperator (a.x_, b.x_), c.x_) &&
        isEqual (scalOperator (a.y_, b.y_), c.y_) &&
        isEqual (scalOperator (a.z_, b.z_), c.z_))
        return vecOprTestStatus_t::OK;

    return vecOprTestStatus_t::WRONG_CALC;
}

vecOprTestStatus_t vecOprTest (Vector a, fp_t b, Vector c, scalOperator_t scalOperator)
{
    if (!c.isValid ())
        return vecOprTestStatus_t::NOT_VALID_RESULT;

    if (isEqual (scalOperator (a.x_, b), c.x_) &&
        isEqual (scalOperator (a.y_, b), c.y_) &&
        isEqual (scalOperator (a.z_, b), c.z_))
        return vecOprTestStatus_t::OK;

    return vecOprTestStatus_t::WRONG_CALC;
}

vecOprTestStatus_t vecOprTest (fp_t a, Vector b, Vector c, scalOperator_t scalOperator)
{
    if (!c.isValid ())
        return vecOprTestStatus_t::NOT_VALID_RESULT;

    if (isEqual (scalOperator (a, b.x_), c.x_) &&
        isEqual (scalOperator (a, b.y_), c.y_) &&
        isEqual (scalOperator (a, b.z_), c.z_))
        return vecOprTestStatus_t::OK;

    return vecOprTestStatus_t::WRONG_CALC;
}

} // namespace

TEST( VectorTests, OperatorPlusTest )
{
    Vector a = genVec (), b = genVec ();
    vecOprTestStatus_t status =
        vecOprTest (a, b, a + b, [=](fp_t a, fp_t b) -> fp_t {return a + b;});
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);
}

TEST( VectorTests, OperatorMinusTest )
{
    Vector a = genVec (), b = genVec ();
    vecOprTestStatus_t status =
        vecOprTest (a, b, a - b, [=](fp_t a, fp_t b) -> fp_t {return a - b;});
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);
}

TEST( VectorTests, SumAssignTest )
{
    Vector a = genVec (), b = genVec (), old_a = a;
    vecOprTestStatus_t status =
        vecOprTest (old_a, b, a += b, [=](fp_t a, fp_t b) -> fp_t {return a += b;});
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);
}

TEST( VectorTests, DiffAssignTest )
{
    Vector a = genVec (), b = genVec (), old_a = a;
    vecOprTestStatus_t status =
        vecOprTest (old_a, b, a -= b, [=](fp_t a, fp_t b) -> fp_t {return a -= b;});
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);
}

TEST( VectorTests, OperatorMulTest )
{
    Vector a = genVec ();
    fp_t b = genFPVal ();
    auto mulOperator = [=](fp_t a, fp_t b) -> fp_t {return a * b;};

    vecOprTestStatus_t status = vecOprTest (a, b, a * b, mulOperator);
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);

    status = vecOprTest (b, a, b * a, mulOperator);
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);
}

TEST( VectorTests, OperatorDivTest )
{
    Vector a = genVec ();
    fp_t b = genFPVal ();
    vecOprTestStatus_t status =
        vecOprTest (a, b, a / b, [=](fp_t a, fp_t b) -> fp_t {return a / b;});
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);
    ASSERT_FALSE ((genVec () / 0).isValid ());
}

TEST( VectorTests, MulAssignTest )
{
    Vector a = genVec (), old_a = a;
    fp_t b = genFPVal ();
    vecOprTestStatus_t status =
        vecOprTest (old_a, b, a *= b, [=](fp_t a, fp_t b) -> fp_t {return a *= b;});
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);
}

TEST( VectorTests, DivAssignTest )
{
    Vector a = genVec (), old_a = a;
    fp_t b = genFPVal ();
    vecOprTestStatus_t status =
        vecOprTest (old_a, b, a /= b, [=](fp_t a, fp_t b) -> fp_t {return a /= b;});
    ASSERT_TRUE (status == vecOprTestStatus_t::OK);
    ASSERT_FALSE ((genVec () /= 0).isValid ());
}

TEST( VectorTests, equalCmpOperatorTest )
{
    Vector a = genVec ();
    Vector b = genVec ();

    ASSERT_TRUE (a == a);
    ASSERT_TRUE (b == b);
    ASSERT_FALSE (a + b == a);
    ASSERT_FALSE (a + b == b);
}

TEST( VectorTests, SquareLenTest )
{
    Vector a = genVec ();
    ASSERT_FLOAT_EQ (a.x_ * a.x_ + a.y_ * a.y_ + a.z_ * a.z_, a.squareLen ());
}

TEST( VectorTests, LenTest )
{
    Vector a = genVec ();
    ASSERT_FLOAT_EQ (std::sqrt (a.x_ * a.x_ + a.y_ * a.y_ + a.z_ * a.z_), a.len ());
}

TEST( VectorTests, ScalarProdTest )
{
    Vector e1 {1,0,0}, e2 {0,1,0}, e3 {0,0,1};
    fp_t scalProd1 = Vector::scalarProduct (e1 + e2 * 2 + e3 * 3, -e2 - e3 * 1.5);
    fp_t scalProd2 = Vector::scalarProduct (e3 * 3 - e2 * 4, e1 * 100 + e2 * 3.75 + e3 * 6);

    ASSERT_FLOAT_EQ (scalProd1, -6.5);
    ASSERT_FLOAT_EQ (scalProd2, 3);
}

TEST( VectorTests, CrossProdTest )
{
    Vector e1 {1,0,0}, e2{0,1,0}, e3{0,0,1};
    Vector crossProd1 = Vector::crossProduct (e1 + e2 + e3, e1);
    Vector crossProd2 = Vector::crossProduct (e1 + e2 + e3, e2);
    Vector crossProd3 = Vector::crossProduct (e1 + e2 + e3, e3);

    ASSERT_TRUE (crossProd1 == -e3 + e2);
    ASSERT_TRUE (crossProd2 == e3 - e1);
    ASSERT_TRUE (crossProd3 == -e2 + e1); 
}

int main( int argc, char ** argv )
{
    testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}