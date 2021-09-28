
#include <cmath>

namespace geom3D
{

// Choosen floating point type.
using fp_t = _Float32;

// Common 3D point.
struct Point
{

    fp_t x_ = std::nan("");
    fp_t y_ = std::nan("");
    fp_t z_ = std::nan("");

    Point( fp_t x, fp_t y, fp_t z );
    bool isValid() const;

};

// Common 3D vector.
struct Vector
{

    fp_t x_ = std::nan("");
    fp_t y_ = std::nan("");
    fp_t z_ = std::nan("");

    Vector( fp_t x, fp_t y, fp_t z );
    bool isValid() const;

    Vector operator+( const Vector& second ) const;
    Vector operator-( const Vector& second ) const;
    Vector operator+=( const Vector& second ) const;
    Vector operator-=( const Vector& second ) const;

    static Vector scalarProduct( const Vector& first, const Vector& second );
    static Vector crossProduct( const Vector& first, const Vector& second );

};

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