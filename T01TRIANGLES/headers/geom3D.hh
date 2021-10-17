
#include <limits>
#include <cmath>
#include <type_traits>

#ifndef GEOM3D_HH_INCL
#define GEOM3D_HH_INCL

namespace geom3D
{

// Choosen floating point type.
using fp_t = _Float32;

const fp_t nan = std::numeric_limits<fp_t>::quiet_NaN ();
const fp_t inf = std::numeric_limits<fp_t>::infinity ();

// fp_t validation function (checks for nan and inf).
bool isValid( fp_t value );

// isEqual cmp func accuracy.
constexpr fp_t FP_CMP_PRECISION = 0.01;
// fp_t == operator equivalent - compares with accuracy.
int isEqual( fp_t a, fp_t b );

// Geometrical primitives validation rules:
// 1) Primitives with nan fields are called invalid.
// 2) Primitives with inf fields are called half-invalid (if they are not invalid).
// 3) It is guaranteed that functions marked as SAFE return invalid output values
//    for invalid inputs and half-invalid/invalid output values for half-invalid inputs.
// 4) It is guaranteed that bool functions marked as SAFE return false for invalid and
//    half-invalid inputs.
// 5) It is guaranteed that functions marked as WARNED return invalid values for invalid inputs.
// 6) It is guaranteed that bool functions marked as WARNED return false for invalid inputs.

// Geometrical primitives:
struct Point;
struct Vector;
struct Line;
struct Segment;
struct Plane;

struct Point
{
    // No invariants to protect.
    fp_t x_ = nan;
    fp_t y_ = nan;
    fp_t z_ = nan;

    // SAFE.
    bool isValid() const;

    // WARNED.
    bool operator==( const Point& ) const;

    // SAFE.
    Point operator+( const Vector& ) const;
};

fp_t sqDst( const Point&, const Point& );

struct Vector
{
    // No invariants to protect.
    fp_t x_ = nan;
    fp_t y_ = nan;
    fp_t z_ = nan;

    // SAFE.
    Vector( const fp_t, const fp_t, const fp_t );
    Vector( const Point&, const Point& );
    Vector( const Vector& ) = default;
    // Default constructed vector is invalid.
    Vector() = default;

    // SAFE.
    bool isValid() const;

    // SAFE.
    Vector operator-() const;
    Vector operator+( const Vector& ) const;
    Vector operator-( const Vector& ) const;
    Vector operator*( const fp_t ) const;
    // WARNED.
    Vector operator/( const fp_t ) const;

    // WARNED.
    bool operator==( const Vector& ) const;

    // SAFE.
    fp_t sqLen() const;
    fp_t len() const;

    // SAFE.
    static fp_t scalarProduct( const Vector& , const Vector& );
    static Vector crossProduct( const Vector& , const Vector& );
};

// SAFE.
Vector operator*( fp_t, Vector );

// Calculates 3x3 det. SAFE.
fp_t det( const Vector&, const Vector&, const Vector& );

// Segments are stored as 2 Points + stores self square length (length is used frequently).
struct Segment
{
    // sqLen_ == sqDst(A_, B_) is a protected invariant.
private:
    Point A_ {};
    Point B_ {};
    fp_t sqLen_ = nan;

public:
    // Getters:
    Point A() const;
    Point B() const;
    fp_t sqLen() const;

    // SAFE.
    Segment( const Point&, const Point& );
    Segment( const Segment& ) = default;
    // Default constructed Segment is invalid.
    Segment() = default;

    // SAFE.
    bool isValid() const;

    // Does this segment contains the point?
    // Works properly only for point & segment on one line.
    // WARNED.
    bool linearContains( const Point& ) const;

    // Returns line and segment cross.
    // If there is infinite number of solutions, or no solutions,
    // or Segment length is zero - returns invalid point.
    // WARNED.
    Point operator|( const Line& ) const;

    // Returns segment and plane cross.
    // If there is infinite number of solutions, or no solutions,
    // or Segment length is zero - returns invalid point.
    // WARNED.
    Point operator|( const Plane& ) const;
};

// Lines are stored as point + direction vector
struct Line
{
    // dir_ != {0,0,0} is a protected invariant.
private:
    Vector dir_ {};
    Point P_ {};

public:
    // Getters:
    Vector dir() const;
    Point P() const;

    // SAFE.
    // Line is invalid if Vector == {0,0,0}
    Line( const Vector&, const Point& );
    // Line is invalid if Segment.A_ == Segment.B_.
    Line( const Segment& );
    Line( const Line& ) = default;
    // Default constructed Line is invalid.
    Line() = default;

    // SAFE.
    bool isValid() const;

    // Does this line contains the point?
    // WARNED.
    bool contains( const Point& ) const;
    // Is this line parallel to the second line?
    // SAFE.
    bool parallelTo( const Line& ) const;

    // Returns lines cross.
    // If lines are equal or parallel or skew - returns invalid point.
    // SAFE.
    Point operator|( const Line& ) const;
    // WARNED.
    bool operator==( const Line& ) const;
    // Returns lines and segments cross.
    // If there is infinite number of solutions, or no solutions,
    // or Segment length is zero - returns invalid point.
    // WARNED.
    Point operator|( const Segment& ) const;

    // Returns lines and planes cross.
    // If there is infinite number of solutions or no solutions - returns invalid point.
    // WARNED.
    Point operator|( const Plane& ) const;
};

// Planes are stored as normal vector + D coeff:
// n_.x_*x + n_.y_*y + n_.z_*z + D = 0
struct Plane
{
    // n_ != {0,0,0} is a protected invariant.
private:
    Vector n_ {};
    fp_t D_ = nan;

public:
    // Getters:
    Vector n() const;
    fp_t D() const;

    // SAFE.
    // Plane is invalid if Vector == {0,0,0}.
    Plane( const Vector&, fp_t );
    // Plane is invalid if points lies on one line (or equal).
    Plane( const Point&, const Point&, const Point& );
    Plane( const Plane& ) = default;
    // Default constructed Plane is invalid.
    Plane() = default;

    // SAFE.
    bool isValid() const;

    //
    bool contains( const Point& ) const;

    // Returns lines and planes cross.
    // If there is infinite number of solutions or no solutions - returns invalid point.
    // WARNED.
    Point operator|( const Line& ) const;

    // Returns segment and plane cross.
    // If there is infinite number of solutions, or no solutions,
    // or Segment length is zero - returns invalid point.
    // WARNED.
    Point operator|( const Segment& ) const;
};

// Stored triangle info - not a geometrical primitive.
struct TriangleInfo
{
    // Many invariants to protect.
private:
    Plane plane_ {};
    bool isDegen_ = true;
    // Max segment is stored in AB_ for degenerate triangles.
    // Other segments should not be used in degenerate case. 
    Segment AB_ {}; // ft + sd point (in this order) - for not degen.
    Segment BC_ {}; // sd + tr point (in this order) - for not degen.
    Segment CA_ {}; // tr + ft point (in this order) - for not degen.

public:
    // Getters:
    Plane plane() const;
    bool isDegen() const;
    Segment AB() const;
    Segment BC() const;
    Segment CA() const;

    TriangleInfo( const Point&, const Point&, const Point& );
    TriangleInfo( const TriangleInfo& ) = default;
    TriangleInfo() = default;

    // Any return value for invalid or half-invalid points.
    bool crosses( const TriangleInfo& ) const;
};

} // namespace geom3D

#endif // #ifndef GEOM3D_HH_INCL