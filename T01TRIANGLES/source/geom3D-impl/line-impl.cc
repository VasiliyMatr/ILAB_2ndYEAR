
#include "geom3D.hh"
namespace geom3D
{

Line::Line( const Vector& dir, const Point& P ) :
    dir_ (dir == Vector {0,0,0} ? Vector {} : dir),
    P_ (P)
{}

Line::Line( const Segment& seg ) :
    dir_ (seg.A () == seg.B () ? Vector {} : Vector {seg.A (), seg.B ()}),
    P_ (seg.A ())
{}

Vector Line::dir() const
{
    return dir_;
}

Point Line::P() const
{
    return P_;
}

bool Line::isValid() const
{
    return dir_.isValid () && P_.isValid ();
}

bool Line::contains( const Point& toCheck ) const
{
    return isEqual ((P_.x_ - toCheck.x_) * dir_.y_,
                    (P_.y_ - toCheck.y_) * dir_.x_) &&
           isEqual ((P_.y_ - toCheck.y_) * dir_.z_,
                    (P_.z_ - toCheck.z_) * dir_.y_);
}

bool Line::parallelTo( const Line& sd ) const
{
    return Vector::crossProduct (dir_, sd.dir_) == Vector {0,0,0};
}

Point Line::operator|( const Line& sd ) const
{
    const Vector& a = dir_;
    const Vector& b = sd.dir_;
    const Vector c = Vector::crossProduct (dir_, sd.dir_);
    const Vector d {P_, sd.P_};

    // Solving system: a*k1 + b*k2 + c*k3 = d
    const fp_t D = det (a, b, c); // Not zero if there are solutions.
    const fp_t D3 = det (a, b, d); // Should be zero.
    if (isEqual (D, 0) || !isEqual (D3, 0))
        return Point {};

    const fp_t k = det (d, b, c) / D; // k1

    return P_ + a*k;
}

bool Line::operator==( const Line& sd ) const
{
    return sd.contains (P_) && sd.contains (P_ + dir_);
}

Point Line::operator|( const Segment& seg ) const
{
    Point linesCross = *this | Line {seg};
    return seg.linearContains (linesCross) ? linesCross : Point {};
}

Point Line::operator|( const Plane& plane ) const
{
    return plane | *this;
}

} // namespace geom3D