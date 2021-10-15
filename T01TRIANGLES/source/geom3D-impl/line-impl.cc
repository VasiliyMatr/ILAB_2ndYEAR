
#include "geom3D.hh"
namespace geom3D
{

Line::Line( const Vector& dir, const Point& p ) :
    dir_ (dir == Vector {0,0,0} ? Vector {} : dir),
    p_ (p)
{}

Line::Line( const Segment& seg ) :
    dir_ (seg.A_ == seg.B_ ? Vector {} : Vector {seg.A_, seg.B_}),
    p_ (seg.A_)
{}

bool Line::isValid() const
{
    return dir_.isValid () && p_.isValid ();
}

bool Line::contains( const Point& toCheck ) const
{
    return isEqual ((p_.x_ - toCheck.x_) * dir_.y_,
                    (p_.y_ - toCheck.y_) * dir_.x_) &&
           isEqual ((p_.y_ - toCheck.y_) * dir_.z_,
                    (p_.z_ - toCheck.z_) * dir_.y_);
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
    const Vector d {p_, sd.p_};

    // Solving system: a*k1 + b*k2 + c*k3 = d
    const fp_t D = det (a, b, c); // Not zero if there are solutions.
    const fp_t D3 = det (a, b, d); // Should be zero.
    if (isEqual (D, 0) || !isEqual (D3, 0))
        return Point {};

    const fp_t k = det (d, b, c) / D; // k1

    return p_ + a*k;
}

bool Line::operator==( const Line& sd ) const
{
    return sd.contains (p_) && sd.contains (p_ + dir_);
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