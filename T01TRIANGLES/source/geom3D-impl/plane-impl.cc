
#include "geom3D.hh"

namespace geom3D
{

Plane::Plane( const Vector& n, fp_t D ) :
    n_(n == Vector {0,0,0} ? Vector {} : n), D_(D) {}

Plane::Plane( const Point& A, const Point& B, const Point& C ) :
    n_(Vector::crossProduct ({A, B}, {B, C})),
    D_(n_ == Vector {0,0,0} ? nan :
      -n_.x_*A.x_ - n_.y_*A.y_ - n_.z_*A.z_)
{}

bool Plane::isValid() const
{
    bool isValid (fp_t);
    return n_.isValid () && isValid (D_);
}

Point Plane::operator|( const Line& line ) const
{
    const Vector& dir = line.dir_;
    const Point& p = line.p_;

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