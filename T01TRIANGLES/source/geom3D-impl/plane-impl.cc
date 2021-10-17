
#include "geom3D.hh"

namespace geom3D
{

Plane::Plane( const Vector& n, fp_t D ) :
    n_(n == O ? Vector {} : n), D_(D) {}

Plane::Plane( const Point& A, const Point& B, const Point& C ) :
    n_(Vector::crossProduct ({A, B}, {B, C})),
    D_(n_ == O ? nan : -n_.x_*A.x_ - n_.y_*A.y_ - n_.z_*A.z_)
{}

Vector Plane::n() const
{
    return n_;
}

fp_t Plane::D() const
{
    return D_;
}

bool Plane::isValid() const
{
    bool isValid (fp_t);
    return n_.isValid () && isValid (D_);
}

Point Plane::operator|( const Line& line ) const
{
    const Vector& dir = line.dir ();
    const Point& p = line.P ();

    fp_t dirNormScal = Vector::scalarProduct (dir, n_);
    fp_t palneEVal = p.x_*n_.x_ + p.y_*n_.y_ + p.z_*n_.z_ + D_;

    fp_t k = -palneEVal / dirNormScal;

    return p + k*dir;
}

Point Plane::operator|( const Segment& seg ) const
{
    return seg | *this;
}

bool Plane::contains( const Point& P ) const
{
    return isEqual (n_.x_*P.x_ + n_.y_*P.y_ + n_.z_*P.z_ + D_, 0);
}

} // namespace geom3D