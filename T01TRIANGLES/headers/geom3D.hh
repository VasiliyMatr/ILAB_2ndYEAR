
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

// fp_t validation function
bool isValid( fp_t value );

// == accuracy
constexpr fp_t FP_CMP_PRECISION = 1e-5;
// fp_t == operator equivalent - compares with accuracy.
int isEqual( fp_t a, fp_t b );

// Common 3D point.
struct Point
{

    const fp_t x_ = nan;
    const fp_t y_ = nan;
    const fp_t z_ = nan;

    bool isValid() const;

};

// Common 3D vector.
struct Vector
{

    const fp_t x_ = nan;
    const fp_t y_ = nan;
    const fp_t z_ = nan;

    Vector( const fp_t, const fp_t, const fp_t );
    Vector( const Point&, const Point& );
    Vector( const Vector& ) = default;
    Vector() = default;

    bool isValid() const;

    Vector operator-() const;
    Vector operator+( const Vector& ) const;
    Vector operator-( const Vector& ) const;

    Vector operator*( const fp_t ) const;
    Vector operator/( const fp_t ) const;

    bool operator==( const Vector& ) const;

    fp_t squareLen() const;
    fp_t len() const;
    Vector normalized() const;

    static fp_t scalarProduct( const Vector& , const Vector& );
    static Vector crossProduct( const Vector& , const Vector& );

};

Vector operator*( fp_t, Vector );

// Calculates 3x3 det
fp_t det( const Vector&, const Vector&, const Vector& );

// Lines are stored as point + direction vector
struct Line
{

    const Vector dirVec_ {};
    const Point point_ {};

    Line( const Vector&, const Point& );
    Line( const Line& ) = default;
    Line() = default;

    bool isValid() const;

    Point operator|( const Line& ) const;

};

// Segments are stored as 2 Points
struct Segment
{

    const Point A_ {};
    const Point B_ {};

    Segment( const Point&, const Point& );
    Segment( const Segment& ) = default;
    Segment() = default;

    bool isValid() const;

};

Point operator|( const Segment&, const Line& );
Point operator|( const Line&, const Segment& );

// Planes are stored as normal vector + D coeff:
// normVec_.x_ * x + normVec_.y_ * y + normVec_.z_ * z + D = 0
struct Plane
{

    const Vector normVec_ {};
    const fp_t D_ = nan;

    Plane( const Vector&, fp_t );
    Plane( const Plane& ) = default;
    Plane() = default;

    bool isValid () const;

    Line operator|( const Plane& ) const;

};

// Store some extra info in TriangleInfo structure - for more efficient calculations.
struct TriangleInfo
{

    const Plane plane_ {};
    const Segment AB_ {};
    const Segment BC_ {};
    const Segment CA_ {};

    TriangleInfo( const Point&, const Point&, const Point& );
    TriangleInfo( const TriangleInfo& ) = default;
    TriangleInfo() = default;

    bool isValid() const;

    bool isInter( const TriangleInfo& ) const;

};

} // namespace geom3D

#endif // #ifndef GEOM3D_HH_INCL