
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

bool isValid( fp_t value );

int diff( fp_t a, fp_t b );

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

// Common 3-point 3D triangle.
struct Triangle
{

    Point A_;
    Point B_;
    Point C_;

    Triangle( Point A, Point B, Point C );
    bool isValid() const;
    bool isDegenerate() const;

};

// Stores normal vector for plane containing this triangle
// Points are indexed as shown: A - first; B - second; C - third.
// Point indexes increase counterclockwise as viewed from the end of the normal vector.
struct OrientedTriangle : Triangle
{

    Point A_;
    Point B_;
    Point C_;

    Vector normVector_;

    OrientedTriangle( Point A1, Point B1, Point C1 );
    OrientedTriangle( Triangle input );
    bool isValid() const;
    bool isDegenerate() const;

};

} // namespace geom3D

#endif // #ifndef GEOM3D_HH_INCL