
#include "geom3D.hh"
namespace geom3D
{

Line::Line( const Segment& seg ) :
    dir_ (seg.A () == seg.B () ? Vector {} : Vector {seg.A (), seg.B ()}),
    P_ (seg.A ()) {}

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

Point Line::operator|( const Plane& plane ) const
{
    const Vector n = plane.n ();

    fp_t dirNormScal = Vector::scalarProduct (dir_, n);
    fp_t palneEVal = P_.x_*n.x_ + P_.y_*n.y_ + P_.z_*n.z_ + plane.D ();

    fp_t k = -palneEVal / dirNormScal;

    return P_ + k*dir_;
}

Point Line::operator|( const Segment& seg ) const
{
    Point linesCross = *this | Line {seg};
    return seg.linearContains (linesCross) ? linesCross : Point {};
}

} // namespace geom3D