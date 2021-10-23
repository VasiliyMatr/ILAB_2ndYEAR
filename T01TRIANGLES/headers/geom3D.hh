
#include <limits>
#include <cmath>
#include <type_traits>

#ifndef GEOM3D_HH_INCL
#define GEOM3D_HH_INCL

namespace geom3D
{

// Choosen floating point type for all geom3D functionallity.
using fp_t = float;

const fp_t nan = std::numeric_limits<fp_t>::quiet_NaN ();
const fp_t inf = std::numeric_limits<fp_t>::infinity ();

// fp_t validation function (checks for nan and inf).
inline bool isValid( fp_t value )
{
    return !(std::isnan (value) || std::isinf (value));
}

// isEqual cmp func accuracy.
constexpr fp_t FP_CMP_PRECISION = 0.01;
// fp_t == operator equivalent - compares with accuracy.
inline int isEqual( fp_t a, fp_t b )
{
    return std::abs (a - b) <= FP_CMP_PRECISION;
}

// Geometrical primitives validation rules:
// 1) Primitives with nan fields are called invalid.
// 2) Primitives with inf fields are called half-invalid (if they are not invalid).
// 3) It is guaranteed that functions marked as SAFE return invalid output values
//    for invalid inputs and half-invalid/invalid output values for half-invalid inputs.
// 4) It is guaranteed that bool functions marked as SAFE return false for invalid and
//    half-invalid inputs.
// 5) It is guaranteed that functions marked as WARNED return invalid values for invalid inputs.
// 6) It is guaranteed that bool functions marked as WARNED return false for invalid inputs.
// 7) All methods are considered SAFE unless otherwise specified.

// Geometrical primitives:
struct Point;
struct Vector;
struct Line;
struct Segment;
struct Plane;

struct Point
{
    fp_t x_ = nan;
    fp_t y_ = nan;
    fp_t z_ = nan;

    bool isValid() const
    {
        return geom3D::isValid (x_) &&
               geom3D::isValid (y_) &&
               geom3D::isValid (z_);
    }

    // WARNED.
    bool operator==( const Point& sd ) const
    {
        return isEqual (x_, sd.x_) &&
               isEqual (y_, sd.y_) &&
               isEqual (z_, sd.z_);
    }

    Point operator+( const Vector& vec ) const;
};

fp_t sqDst( const Point&, const Point& );

struct Vector
{
    fp_t x_ = nan;
    fp_t y_ = nan;
    fp_t z_ = nan;

    Vector( fp_t x, fp_t y, fp_t z ) :
        x_(x), y_(y), z_(z) {}

    Vector( const Point& P1, const Point& P2 ) :
        x_(P2.x_-P1.x_), y_(P2.y_-P1.y_), z_(P2.z_-P1.z_) {}

    // Default constructed vector is invalid.
    Vector() = default;

    bool isValid() const
    {
        return geom3D::isValid (x_) &&
               geom3D::isValid (y_) &&
               geom3D::isValid (z_);
    }

    Vector operator-() const { return {-x_, -y_, -z_}; }

    Vector operator+( const Vector& sd ) const
    {
        return {x_ + sd.x_, y_ + sd.y_, z_ + sd.z_};
    }

    Vector operator-( const Vector& sd ) const
    {
        return {x_ - sd.x_, y_ - sd.y_, z_ - sd.z_};
    }

    Vector operator*( const fp_t num ) const
    {
        return {x_ * num, y_ * num, z_ * num};
    }

    // WARNED.
    Vector operator/( const fp_t num ) const
    {
        return {x_ / num, y_ / num, z_ / num};
    }

    // WARNED.
    bool operator==( const Vector& sd ) const
    {
        return isEqual (x_, sd.x_) && isEqual (y_, sd.y_) && isEqual (z_, sd.z_);
    }

    fp_t sqLen() const
    {
        return x_*x_ + y_*y_ + z_*z_;
    }

    fp_t len() const
    {
        return std::sqrt (sqLen ());
    }

    static fp_t scalarProduct( const Vector& ft, const Vector& sd )
    {
        return ft.x_*sd.x_ + ft.y_*sd.y_ + ft.z_*sd.z_;
    }

    static Vector crossProduct( const Vector& ft, const Vector& sd )
    {
        return {ft.y_*sd.z_ - ft.z_*sd.y_,
               -ft.x_*sd.z_ + ft.z_*sd.x_,
                ft.x_*sd.y_ - ft.y_*sd.x_};
    }

    // Few useful Vector constants.
    static Vector e1 () { return {1,0,0}; }
    static Vector e2 () { return {0,1,0}; }
    static Vector e3 () { return {0,0,1}; }

    static Vector zero () { return {0,0,0}; }
};

inline Vector operator*( fp_t num, Vector vec )
{
    return vec * num;
}

// Calculates 3x3 determinant.
inline fp_t det( const Vector& a, const Vector& b, const Vector& c )
{
    return a.x_ * (b.y_*c.z_ - b.z_*c.y_) -
           a.y_ * (b.x_*c.z_ - b.z_*c.x_) +
           a.z_ * (b.x_*c.y_ - b.y_*c.x_);
}

// Lines are stored as point + direction vector
struct Line
{
    // dir_ != geom3D::Vector::zero () is a protected invariant.
private:
    Vector dir_ {};
    Point P_ {};

public:
    Vector dir() const { return dir_; }
    Point P() const { return P_; }

    // Line is invalid if Vector == {0,0,0}
    Line( const Vector& dir, const Point& P ) :
        dir_ (dir == Vector::zero () ? Vector {} : dir), P_ (P) {}

    // Line is invalid if Segment.A_ == Segment.B_.
    Line( const Segment& );

    // Default constructed Line is invalid.
    Line() = default;

    bool isValid() const { return dir_.isValid () && P_.isValid (); }

    // Does this line contains the point?
    // WARNED.
    bool contains( const Point& toCheck ) const
    {
        return isEqual ((P_.x_ - toCheck.x_) * dir_.y_,
                        (P_.y_ - toCheck.y_) * dir_.x_) &&
               isEqual ((P_.y_ - toCheck.y_) * dir_.z_,
                        (P_.z_ - toCheck.z_) * dir_.y_);
    }

    // Is this line parallel to the second line?
    bool parallelTo( const Line& sd ) const
    {
        return Vector::crossProduct (dir_, sd.dir_) == Vector::zero ();
    }

    // Returns lines cross.
    // If lines are equal or parallel or skew - returns invalid point.
    Point operator|( const Line& ) const;

    // Returns lines and planes cross.
    // If there is infinite number of solutions or no solutions - returns invalid point.
    // WARNED.
    Point operator|( const Plane& ) const;

    // Returns lines and segments cross.
    // If there is infinite number of solutions, or no solutions,
    // or Segment length is zero - returns invalid point.
    // WARNED.
    Point operator|( const Segment& seg ) const;

    // WARNED.
    bool operator==( const Line& sd ) const
    {
        return sd.contains (P_) && sd.contains (P_ + dir_);
    }
};

// Segments are stored as 2 Points + stores
// self square length (square length is used frequently).
struct Segment
{
    // sqLen_ == sqDst(A_, B_) is a protected invariant.
private:
    Point A_ {};
    Point B_ {};
    fp_t sqLen_ = nan;

public:
    Point A() const { return A_; }
    Point B() const { return B_; }
    fp_t sqLen() const { return sqLen_; }
    
    Segment( const Point& A, const Point& B ) :
        A_ (A), B_ (B), sqLen_ (sqDst (A, B)) {}

    // Default constructed Segment is invalid.
    Segment() = default;

    bool isValid() const
    {
        return A_.isValid () && B_.isValid () && geom3D::isValid (sqLen_);
    }

    // Does this segment contains the point?
    // Works properly only for point & segment on one line.
    // WARNED.
    bool linearContains( const Point& P ) const
    {
        return sqLen_ + FP_CMP_PRECISION >= sqDst (P, A_) &&
               sqLen_ + FP_CMP_PRECISION >= sqDst (P, B_);
    }

    // WARNED.
    Point operator|( const Line& line ) const { return line | *this; }

    // Returns segment and plane cross.
    // If there is infinite number of solutions, or no solutions,
    // or Segment length is zero - returns invalid point.
    // WARNED.
    Point operator|( const Plane& plane ) const
    {
        Point lineCross = Line {*this} | plane;
        return linearContains (lineCross) ? lineCross : Point {};
    }
};

// n_.x_*x + n_.y_*y + n_.z_*z + D = 0
struct Plane
{
    // n_ != {0,0,0} is a protected invariant.
private:
    Vector n_ {};
    fp_t D_ = nan;

public:
    Vector n() const { return n_; }
    fp_t D() const { return D_; }

    // SAFE.
    // Plane is invalid if Vector == {0,0,0}.
    Plane( const Vector& n, fp_t D ) :
        n_(n == Vector::zero () ? Vector {} : n), D_(D) {}

    // Plane is invalid if points lies on one line (or equal).
    Plane( const Point& A, const Point& B, const Point& C ) :
        n_(Vector::crossProduct ({A, B}, {B, C})),
        D_(n_ == Vector::zero () ? nan : -n_.x_*A.x_ - n_.y_*A.y_ - n_.z_*A.z_) {}

    // Default constructed Plane is invalid.
    Plane() = default;

    bool isValid() const
    {
        return n_.isValid () && geom3D::isValid (D_);
    }

    bool contains( const Point& P ) const
    {
        return isEqual (n_.x_*P.x_ + n_.y_*P.y_ + n_.z_*P.z_ + D_, 0);
    }

    // WARNED.
    Point operator|( const Line& line ) const { return line | *this; }

    // WARNED.
    Point operator|( const Segment& seg ) const { return seg | *this; }
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
    Plane plane() const { return plane_; }
    bool isDegen() const { return isDegen_; }
    Segment AB() const { return AB_; }
    Segment BC() const { return BC_; }
    Segment CA() const { return CA_; }

    TriangleInfo( const Point&, const Point&, const Point& );
    TriangleInfo() = default;

    // Any return value for invalid or half-invalid points.
    bool crosses( const TriangleInfo& ) const;
};

} // namespace geom3D

#endif // #ifndef GEOM3D_HH_INCL