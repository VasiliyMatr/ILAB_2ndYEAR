
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
// fp_t == operator equivalent - compares with accuracy.
int isEqual( fp_t a, fp_t b );

// Common 3D point.
struct Point
{

    fp_t x_ = nan;
    fp_t y_ = nan;
    fp_t z_ = nan;

    Point( fp_t x, fp_t y, fp_t z );
    Point() = default;
    bool isValid() const;

};

// Common 3D vector.
struct Vector
{

    fp_t x_ = nan;
    fp_t y_ = nan;
    fp_t z_ = nan;

    Vector( fp_t x, fp_t y, fp_t z );
    Vector() = default;
    Vector( const Vector& ) = default;

    bool isValid() const;

    Vector operator-() const;
    Vector operator+( const Vector& ) const;
    Vector operator-( const Vector& ) const;
    Vector operator+=( const Vector& );
    Vector operator-=( const Vector& );

    Vector operator*( const fp_t ) const;
    Vector operator/( const fp_t ) const;
    Vector operator*=( const fp_t );
    Vector operator/=( const fp_t );

    bool operator==( const Vector& ) const;

    fp_t squareLen() const;
    fp_t len() const;
    Vector normalized() const;

    static fp_t scalarProduct( const Vector& , const Vector& );
    static Vector crossProduct( const Vector& , const Vector& );

};

Vector operator*( fp_t, Vector );

struct Segment;

// Lines are stored as point + direction vector
struct Line
{

    const Vector dirVec_;
    const Point point_;

    Line( const Vector&, const Point& );
    Point operator|( const Segment& ) const;

};

// Segments are stored as 2 Points
struct Segment
{

    const Point A_;
    const Point B_;

    Segment( const Point&, const Point& );
    Point operator|( const Line& ) const;

};

// Planes are stored as normal vector + D coeff:
// normVec_.x_ * x + normVec_.y_ * y + normVec_.z_ * z + D = 0
struct Plane
{

    const Vector normVec_;
    const fp_t D_;

    Plane( const Vector&, const fp_t& );

    Line operator|( const Plane& ) const;

};

// Store some extra info in TriangleInfo structure - for more efficient calculations.
struct TriangleInfo
{

    const Plane plane_;
    const Segment AB_;
    const Segment BC_;
    const Segment CA_;

    TriangleInfo( const Point&, const Point&, const Point& );

    bool isValid() const;
    bool isDegenerate() const;

    bool isInter( const TriangleInfo& ) const;

};

} // namespace geom3D

#endif // #ifndef GEOM3D_HH_INCL