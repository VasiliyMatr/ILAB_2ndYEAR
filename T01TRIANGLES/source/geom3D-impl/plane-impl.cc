
#include "geom3D.hh"

namespace geom3D
{

Plane::Plane( const Vector& normVec, fp_t D ) : normVec_(normVec), D_(D) {}

Plane::Plane( const Point& A, const Point& B, const Point& C ) :
    normVec_(Vector::crossProduct ({A, B}, {B, C})),
    D_(-normVec_.x_ * A.x_ - normVec_.y_ * A.y_ - normVec_.z_ * A.z_)
{}

bool Plane::isValid() const
{
    bool isValid (fp_t); // redecl for this scope
    return normVec_.isValid () && isValid (D_);
}

Point operator|( const Plane& plane, const Line& line )
{
    const Vector& dir = line.dirVec_;
    const Vector& norm = plane.normVec_;
    const Point& pt = line.point_;

    fp_t dirNormScal = Vector::scalarProduct (dir, norm);
    fp_t planeExprVal = pt.x_*norm.x_ + pt.y_*norm.y_ + pt.z_*norm.z_ + plane.D_;

    fp_t coeff = -planeExprVal / dirNormScal;

    return Point
        {pt.x_ + coeff*dir.x_, pt.y_ + coeff*dir.y_, pt.z_ + coeff*dir.z_};
}

} // namespace geom3D